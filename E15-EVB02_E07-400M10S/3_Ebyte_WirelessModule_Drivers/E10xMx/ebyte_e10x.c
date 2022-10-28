#include "ebyte_e10x.h"
#include "radio_config_434.h"

/* 辅助识别模块频段 */
#if defined(EBYTE_E10_400M20S)
static uint8e_t E10x_NameString[] = "E10-400M20S";
#elif defined(EBYTE_E10_900M20S)
static uint8e_t E10x_NameString[] = "E10-900M20S";
#endif

/* 辅助识别驱动程序版本号 */
#define EBYTE_E10_PROGRAM_TYPE 0x10

///指定内部FIFO
#define  FIFO_READ           0x00
#define  FIFO_WRITE          0x01

///工作模式切换
#define  E10X_NOCHANGE            0x00
#define  E10X_SLEEP               0x01
#define  E10X_SPI_ACTIVE          0x02
#define  E10X_READY               0x03
#define  E10X_TX_TUNE             0x05
#define  E10X_RX_TUNE             0x06
#define  E10X_TX                  0x07
#define  E10X_RX                  0x08


typedef enum 
{
    GO_INIT          =0x00,        
    GO_BUSY          =0x01, 
    GO_STBY          =0x02,
    GO_RECEIVE       =0x03,
    GO_WAIT_RECEIVE  =0x04,       
    GO_TRANSMIT      =0x05,
    GO_WAIT_TRANSMIT =0x06,
    GO_SLEEP         =0x07,
    GO_ERROR         =0x08
}E10x_Status_t;

/// 状态标识
static E10x_Status_t E10x_Status = GO_INIT;

static uint8e_t E10x_RxBuffer[64];

///WDS生成的配置文件
const uint8e_t E10x_Config_WDS[] = RADIO_CONFIGURATION_DATA_ARRAY;

/*!
 * @brief 判断模块是否完成准备
 *
 * @return 0:正常 1:超时异常
 * @note  通过指令READ_CMD_BUFF(0x44)实现，指令会获取CTS信号(0xFF)
 *        CTS即为 Clear To Send,表示已经准备好响应数据
 *        如果超时: 请检查硬件连接是否正确
 *                  请检查SPI软件通信配置是否正确
 */
static uint8e_t E10x_IsReady( void )
{
    uint8e_t cts;
    uint8e_t result = 0;
    uint8e_t counter_1ms = 0;
    do
    {
        /* SPI CS 选中 */
        Ebyte_Port_SpiCsIoControl( 0 );
        /* 0x44指令  READ_CMD_BUFF */
        Ebyte_Port_SpiTransmitAndReceivce( 0x44 );
        /* 随便发个数据 读回响应数据 正常应获取到CTS(0xFF) */
        cts = Ebyte_Port_SpiTransmitAndReceivce( 0xFF );
        /* SPI CS 拉高 */
        Ebyte_Port_SpiCsIoControl( 1 );
        /* 超时计算 1毫秒节拍 */
        counter_1ms++;
        Ebyte_Port_DelayMs( 1 );
    }
    while( ( cts != 0xFF ) && ( counter_1ms < 200 ) ); //获取到CTS(0xFF)或者 200毫秒超时 会退出循环
    /* 返回值判断 */
    if( counter_1ms >= 200 )
    {
        /* 超时了 */
        result = 1;
    }
    return result;
}

/*!
 * @brief 向模块写入构建好指令帧
 *
 * @param data  指向指令数组
 * @param size 指令数组长度
 * @return 0:正常 1:超时
 */
static uint8e_t E10x_SendCommand( uint8e_t* data, uint8e_t size )
{
    uint8e_t result = 0;
    /* 检查:模块是否准备完成 */
    result = E10x_IsReady();
    if( result != 0 )
    {
        return 1;
    }
    /* SPI CS  */
    Ebyte_Port_SpiCsIoControl( 0 );
    /* 写入 */
    while( size-- )
    {
        Ebyte_Port_SpiTransmitAndReceivce( *data++ );
    }
    /* SPI CS  */
    Ebyte_Port_SpiCsIoControl( 1 );
    return result;
}

/*!
 * @brief 向模块读取指令帧响应
 *
 * @param data  指向存储缓冲区
 * @param size 读取长度
 * @return 0:正常 1:超时
 */
static uint8e_t E10x_GetResponse( uint8e_t* data, uint8e_t size )
{
    uint8e_t result = 0;
    /* 检查:模块是否准备完成 */
    result = E10x_IsReady();
    if( result != 0 )
    {
        return 1;
    }
    /* SPI CS */
    Ebyte_Port_SpiCsIoControl( 0 );
    /* 指令:READ_CMD_BUFF(0x44)  */
    Ebyte_Port_SpiTransmitAndReceivce( 0x44 );
    /* 读取 */
    while( size-- )
    {
        *data++ = Ebyte_Port_SpiTransmitAndReceivce( 0xFF );
    }
    /* SPI CS */
    Ebyte_Port_SpiCsIoControl( 1 );
    return result;
}

/*!
 * @brief 配置功能项
 *
 * @param address 属性的编码
 * @param data    属性值
 * @return 0:正常 1:超时
 */
static uint8e_t E10x_SetProperty( uint16e_t address, uint8e_t data )
{
    uint8e_t result = 0;
    uint8e_t cmd[5];
    /* 指令:SET_PROPERTY(0x11) 表示配置属性 */
    cmd[0] = 0x11;
    cmd[1] = ( uint8e_t )( address >> 8 ); //组编码    GROUP
    cmd[2] = 1;                      //写入长度  NUM_PROPS
    cmd[3] = ( uint8e_t )address;    //起始码    START_PROP
    cmd[4] = data;                   //写入值
    /* 写入 */
    result = E10x_SendCommand( cmd, 5 );
    return result;
}


/*!
 * @brief 切换模块的工作模式
 *
 * @param state 工作模式编码  具体参考EZRadio_RevC2A API 中CHANGE_STATE章节
 *        @arg  0:E10X_NOCHANGE
 *        @arg  1:E10X_SLEEP
 *        @arg  2:E10X_SPI_ACTIVE
 *        @arg  3:E10X_READY
 *
 *        @arg  5:E10X_TX_TUNE
 *        @arg  6:E10X_RX_TUNE
 *        @arg  7:E10X_TX
 *        @arg  8:E10X_RX
 *
 * @return 0:正常 1:切换模式失败
 */
static uint8e_t E10x_SetWorkMode( uint8e_t state )
{
    uint8e_t cmd[3];
    uint8e_t i;
    uint8e_t result = 0;
    /* 指令:CHANGE_STATE(0x34) 切换工作模式 */
    cmd[0] = 0x34;
    cmd[1] = state; //模式编码
    /* 写入 */
    result = E10x_SendCommand( cmd, 2 );
    if( result != 0 )
    {
        return 1;
    }
    /* 循环确认模式变更 */
    for( i = 0 ; i < 5; i++ )
    {
        /* 指令:REQUEST_DEVICE_STATE(0x33) 请求当前设备状态和通道 */
        cmd[0] = 0x33;
        /* 写入 */
        E10x_SendCommand( cmd, 1 );
        /* 读取指令响应 */
        E10x_GetResponse( cmd, 3 );
        /* 比对模式编码 */
        if( ( cmd[1] & 0x0F ) == state )
        {
            break;    //退出循环
        }
        Ebyte_Port_DelayMs( 1 );
    }
    /* 返回值处理 */
    if( i >= 5 )
    {
        /* 模式变更失败 */
        result = 1;
    }
    return result;
}

/*!
 * @brief 清除FIFO内容
 * @param mode 模式
 *           @arg FIFO_WRITE ：指定发送FIFO
 *           @arg FIFO_READ  ：指定接收FIFO
 *
 * @return 0:正常 1:超时
 */
static uint8e_t E10x_ClearFIFO( uint8e_t mode )
{
    uint8e_t cmd[2];
    uint8e_t result;
    if( mode == FIFO_WRITE )
    {
        /* 指令:FIFO_INFO(0x15) 查看FIFO计数与重置FIFO */
        cmd[0] = 0x15;
        cmd[1] = 0x01;//重置发送FIFO
        result = E10x_SendCommand( cmd, 2 );
        if( result != 0 )
        {
            return 1;
        }
    }
    else
    {
        /* 指令:FIFO_INFO(0x15) 查看FIFO计数与重置FIFO */
        cmd[0] = 0x15;
        cmd[1] = 0x02;//重置接收FIFO
        result = E10x_SendCommand( cmd, 2 );
        if( result != 0 )
        {
            return 1;
        }
    }
    return result;
}

/*!
 * @brief 将待发送数据写入内部数据队列
 *
 * @param data 指向写入数据
 * @param size 数据长度
 *
 * @note 默认接收/发送FIFO各自独立，写入数据最大长度为64字节 (长度可能占了一位)
 *       如果GLOBAL_CONFIG:FIFO_MODE被设置，那么FIFO合并且共享，写入数据最大长度为129字节
 */
static void E10x_SetFIFO( uint8e_t* data, uint8e_t size )
{
    /* 检查:模块是否准备完成 */
    E10x_IsReady();
    /* SPI CS */
    Ebyte_Port_SpiCsIoControl( 0 );
    /* 指令:WRITE_TX_FIFO(0x66) 写FIFO */
    Ebyte_Port_SpiTransmitAndReceivce( 0x66 );
    /* 使用了数据包变长模式 第一字节先写入数据长度 */
    Ebyte_Port_SpiTransmitAndReceivce( size );
    /* 循环写入数据 */
    while( size -- )
    {
        Ebyte_Port_SpiTransmitAndReceivce( *data++ );
    }
    /* SPI CS */
    Ebyte_Port_SpiCsIoControl( 1 );
}

/*!
 * @brief 从内部数据队列读取接收的数据
 *
 * @param data 指向存储数据区
 * @param size 指向读取的长度
 *
 * @note 默认接收/发送FIFO各自独立，读取数据最大长度为64字节 (长度可能占了一位)
 *       如果GLOBAL_CONFIG:FIFO_MODE被设置，那么FIFO合并且共享，读取数据最大长度为129字节
 */
static uint8e_t E10x_GetFIFO( uint8e_t* data, uint8e_t* size )
{
    uint8e_t length;
    uint8e_t result ;
    /* 检查模块是否准备完成 */
    result = E10x_IsReady();
    if( result != 0 )
    {
        return 1;
    }
    /* SPI CS */
    Ebyte_Port_SpiCsIoControl( 0 );
    /* 指令:READ_RX_FIFO(0x77) 读FIFO  */
    Ebyte_Port_SpiTransmitAndReceivce( 0x77 );
    /* 可变长数据包 接收数据第一字节为包长度 */
    length = Ebyte_Port_SpiTransmitAndReceivce( 0xFF );
    *size = length;
    /* 循环读取 */
    while( length-- )
    {
        *data++ = Ebyte_Port_SpiTransmitAndReceivce( 0xFF );
    }
    /* SPI CS */
    Ebyte_Port_SpiCsIoControl( 1 );
    return result;
}

/*!
 * @brief 获取内部中断标识
 *
 * @return 中断类型编码
 *        @arg 0x80:FILTER_MATCH_PEND
 *        @arg 0x40:FILTER_MISS_PEND
 *        @arg 0x20:PACKET_SENT_PEND   发送完成
 *        @arg 0x10:PACKET_RX_PEND     接收完成
 *        @arg 0x08:CRC_ERROR_PEND     CRC校验错误
 *        @arg 0x04:ALT_CRC_ERROR_PEND
 *        @arg 0x02:FIFO_ALMOST_EMPTY_PEND
 *        @arg 0x01:FIFO_ALMOST_FULL_PEND
 *
 */
uint8e_t E10x_GetIRQ( void )
{
    uint8e_t buffer[10];
    uint8e_t cmd[4];
    /* 指令:GET_INT_STATUS(0x20) 读取中断状态指令 */
    cmd[0] = 0x20;
    cmd[1] = 0;  //PH_CLR_PEND    不做操作
    cmd[2] = 0;  //MODEM_CLR_PEND 不做操作
    cmd[3] = 0;  //CHIP_CLR_PEND  不做操作
    /* 写入 */
    E10x_SendCommand( cmd, 4 );
    /* 读回指令响应 */
    E10x_GetResponse( buffer, 9 );
    /* 这里只提取了 PH_PEND 字节中的中断状态
       可以查阅 EZRadio_revC2A_API 中GET_INT_STATUS章节 获取更多中断状态*/
    return  buffer[3];
}

/*!
 * @brief 判断模块是否存在
 *
 * @return 0:正常 1:异常
 */
static uint8e_t E10x_IsExist( void )
{
    uint8e_t cmd[4];
    static uint8e_t buffer[10];    
    /* 指令:PART_INFO(0x01) 获取硬件基本信息 */
    cmd[0] = 0x01;
    /* 写入 */
    E10x_SendCommand( cmd, 1 );
    /* 读回指令响应 */
    E10x_GetResponse( buffer, 9 ); 
    /* 正常情况下 读回数据中的PART信息是0x4438  */
    if( (buffer[2] == 0x44) && (buffer[3] == 0x38) ) return 0;//正常返回
    
    return 1;
}

/*!
 * @brief 复位
 * @note 模块内部已配置的参数会丢失
 */
void E10x_Reset( void )
{
    /* SDN引脚置高引起模块复位 */
    Ebyte_Port_SdnIoControl( 1 );
    /* 延时 */
    Ebyte_Port_DelayMs( 20 );
    /* SDN 恢复 */
    Ebyte_Port_SdnIoControl( 0 );
    /* CS 恢复*/
    Ebyte_Port_SpiCsIoControl( 1 );
}

/*!
 * @brief 配置模块参数
 * @note 主要由原厂配置软件 WDS 生成
 *       注意E10系列为26MHz晶振
 *       如果重新生成了配置 直接替换radio_config.h 即可
 */
static void E10x_Config( void )
{
    uint8e_t i;
    uint16e_t j = 0;
    /* 链接WDS自动生成的参数表  */
    const uint8e_t*  ptr = E10x_Config_WDS;
    /* 写入WDS参数表 */
    while( ( i = *( ptr + j ) ) != 0 )
    {
        j += 1;
        E10x_SendCommand( ( uint8e_t* )ptr + j, i );
        j += i;
    }
    /* 配置属性 可变长数据包 */
    E10x_SetProperty( 0x1208, 0x2A ); //大端模式 1Bbyte长度且值存放在FIFO中 第2区域数据包长度可变
    E10x_SetProperty( 0x1209, 0x01 ); //数据包长度放在第1区
    E10x_SetProperty( 0x120D, 0x00 ); //第一区域长度 高字节
    E10x_SetProperty( 0x120E, 0x01 ); //第一区域长度 低字节 0x01:限制为一个字节
    E10x_SetProperty( 0x1211, 0x00 );
    E10x_SetProperty( 0x1212, 0x10 );
    /* 配置输出功率
     * 输入范围:0x00至0x7F   默认值:0x7F
     * 注意;功率并不是线性增加
     * 当接近最大值时，步进可能小于0.1db
     * 当功率偏低时，步进可能大于0.1db  */
    E10x_SetProperty( 0x2201, 0x7F );
}

/*!
 * @brief 开始监听数据
 *
 * @return 0:正常 1:异常
 */
uint8e_t E10x_GoReceive( void )
{
    uint8e_t result = 0;
    uint8e_t cmd[8];
    /* FIFO 复位 */
    result = E10x_ClearFIFO( FIFO_READ );
    if( result != 0 )
    {
        return 1;
    }
    E10x_ClearFIFO( FIFO_WRITE );
    /* 切换模式:  SPI_ACTIVE
     * 请参考  datasheet 中的工作模式切换流程图
     * 接收模式流程为: Sleep或Stby->SPI active->Ready->RxTune->Rx */
    result = E10x_SetWorkMode( E10X_SPI_ACTIVE );
    if( result != 0 )
    {
        return 1;
    }
    /* 切换模式:  RX_TUNE  接收预备状态 */
    result = E10x_SetWorkMode( E10X_RX_TUNE );
    if( result != 0 )
    {
        return 1;
    }
    /* 开始监听(接收) */
    cmd[0] = 0x32;      //指令:START_RX (0x32) 进入Rx工作模式开始监听频段数据
    cmd[1] = 0;         //信道编码:   可用于频率切换
    cmd[2] = 0;         //接收条件:   触发接收时立即进入接收态，不等待唤醒定时器
    cmd[3] = 0;         //接收长度:   高Byte
    cmd[4] = 0;         //接收长度:   低Byte
    cmd[5] = E10X_RX;   //接收超时后: 切换为接收状态
    cmd[6] = E10X_RX;   //接收成功后: 切换为接收状态
    cmd[7] = E10X_RX;   //接收失败后: 切换为接收状态
    E10x_SendCommand( cmd, 8 );
    
    /* 状态记录 */
    E10x_Status = GO_WAIT_RECEIVE;
    
    return result;
}

/*!
 * @brief 开始发送数据
 *
 * @param data 指向发送数据
 * @param size 发送数据长度
 */
uint8e_t E10x_GoTransmit( uint8e_t* data, uint8e_t size )
{
    uint8e_t cmd[6];
    uint8e_t tx_size = size + 1;//可变长数据包额外多出1长度表示位
    uint8e_t result = 1 ;
    /* 防止连续传输 */
    Ebyte_Port_DelayMs( 5 );
    /* FIFO 复位 */
    result = E10x_ClearFIFO( FIFO_READ );
    if( result != 0 )
    {
        return 1;
    }
    E10x_ClearFIFO( FIFO_WRITE );
    /* 模式切换: E10X_SPI_ACTIVE 模式
     * 请参考  datasheet 中的工作模式切换流程图
     * 发送模式流程为: Sleep或Stby->SPI active->Ready->TxTune->Tx */
    result = E10x_SetWorkMode( E10X_SPI_ACTIVE );
    if( result != 0 )
    {
        return 1;
    }
    /* FIFO 写入待发送数据 */
    E10x_SetFIFO( data, size );
    /* 模式切换: E10X_TX_TUNE 发送预备模式 */
    result = E10x_SetWorkMode( E10X_TX_TUNE );
    if( result != 0 )
    {
        return 1;
    }
    /* 开始发送 */
    cmd[0] = 0x31;         //指令:START_TX(0x31) 切换为TX工作模式 开始发送FIFO中的数据
    cmd[1] = 0;            //信道编码:   可用于频率切换
    cmd[2] = 0;            //发送条件:   发送时立即开始发送，不等待唤醒定时器。如果需要发送完成后自动进入其他模式 例如休眠可以配置为 0x10
    cmd[3] = tx_size >> 8; //发送长度:   高字节
    cmd[4] = tx_size;      //发送长度:   低字节
    cmd[5] = 128;          //发送延时128us
    E10x_SendCommand( cmd, 6 );
    
    /* 状态记录 */
    E10x_Status = GO_WAIT_TRANSMIT;
    
    /* 阻塞发送方式 在此等待发送完成 */
    uint8e_t irqStatus = 0;
    do
    {
        irqStatus = E10x_GetIRQ();
    }
    while( !( irqStatus & 0x20 ) );
    
    /* 状态记录 */
    E10x_Status = GO_STBY;
    
    /* 回调用户函数 */
    Ebyte_Port_TransmitCallback( 0x0001 );
    return 0 ;
}

/*!
 * @brief 进入休眠模式
 */
uint8e_t E10x_GoSleep( void )
{
    uint8e_t result = E10x_SetWorkMode( E10X_SLEEP );
    if( result != 0 )
    {
        return 1;
    }
    return 0;
}

/*!
 * @brief 模块初始化
 *
 * @return 0:正常 1:初始化失败
 */
uint8e_t E10x_Init( void )
{
    uint8e_t result;
    /* 复位 */
    E10x_Reset();
    /* 检查: SPI通信是否正常
     * 如果失败: 请检查复位引脚SDN 注意SDN是高电平引起复位
     *           请检查SPI通信引脚硬件连接是否正常
     *           请检查SPI通信软件配置是否正确 注意CPOL=0 CPHA=0  SPI_CS关闭硬件自动控制
     */
    result = E10x_IsReady();
    if( result != 0 )
    {
        return 1;
    }
    /* 检查: SPI获取ID 如果失败 请检查硬件 */
    result = E10x_IsExist();
    if( result != 0 ) return 1;     
    /* 参数配置 */
    E10x_Config();
    /* 开始监听数据 */
    E10x_GoReceive();
    return result;
}

/*!
 * @brief 周期调用函数 辅助完成无线数据接收
 *
 * @return 0
 * @note 需要被主函数循环调用，因为通过轮询内部标识位来判断是否有数据到达
 *       如若考虑中断，在中断中调用本函数即可
 */
uint8e_t E10x_TaskForPoll( void )
{
    uint8e_t irqStatus ;
    /* 获取内部中断标识 */
    irqStatus = E10x_GetIRQ();
    /* 0x10即bit4 表示数据接收完成中断置位 */
    if( irqStatus & 0x10 )
    {
        uint8e_t recvSize = 0;
        /* 获取数据 */
        E10x_GetFIFO( E10x_RxBuffer, &recvSize );
        
        /* 状态记录 */
        E10x_Status = GO_STBY;
        
        /* 回调用户函数 */
        Ebyte_Port_ReceiveCallback( 0x0002, E10x_RxBuffer, recvSize );
    }
    return 0;
}

/*!
 * @brief 保留
 */
void E10x_TaskForIRQ( void )
{
  
}

/*!
 * @brief 获取模块名字
 *
 * @return 指向名字字符串的指针
 * @note 标准字符串 末尾含有结束符 '\0'
 */
uint8e_t* E10x_GetName( void )
{
    return E10x_NameString;
}

/* !
 * @brief 获取模块程序版本
 *
 * @return 8位的编码
 * @note 例如0x10 代表V1.0
 */
uint8e_t E10x_GetDriverVersion( void )
{
    return  EBYTE_E10_PROGRAM_TYPE;
}

/*!
 * @brief 获取模块状态
 * 
 * @return 状态编码
 *        未初始化     GO_INIT          =0x00        
 *        正在切换状态 GO_BUSY          =0x01   
 *        待机/空闲    GO_STBY          =0x02   
 *        准备接收环境 GO_RECEIVE       =0x03   
 *        正在监听数据 GO_WAIT_RECEIVE  =0x04          
 *        准备发送环境 GO_TRANSMIT      =0x05   
 *        等待发送完成 GO_WAIT_TRANSMIT =0x06   
 *        休眠         GO_SLEEP         =0x07   
 *        内部错误     GO_ERROR         =0x08 
 */
uint8e_t E10x_GetStatus(void)
{
  return (uint8e_t)E10x_Status;;
}