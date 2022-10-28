/**
  **********************************************************************************
  * @file      ebyte_e19x.c
  * @brief     EBYTE E19 (sx1278) 系列驱动
  * @details   详情请参见 https://www.ebyte.com/
  * @author    JiangHeng
  * @date      2021-06-16
  * @version   1.0.0
  **********************************************************************************
  * @copyright BSD License
  *            成都亿佰特电子科技有限公司
  *   ______   ____   __     __  _______   ______
  *  |  ____| |  _ \  \ \   / / |__   __| |  ____|
  *  | |__    | |_) |  \ \_/ /     | |    | |__
  *  |  __|   |  _ <    \   /      | |    |  __|
  *  | |____  | |_) |    | |       | |    | |____
  *  |______| |____/     |_|       |_|    |______|
  *
  **********************************************************************************
  */

#include "ebyte_e19x.h"

/// 识别码：模块类别
#if defined(EBYTE_E19_433M20SC)
static uint8e_t E19x_NameString[] = "E19-433M20SC";
#endif

/// 识别码：驱动程序版本号
#define EBYTE_E19_PROGRAM_TYPE 0x10

/// 模块内部收发电路模式
#define ANTENNA_RXEN  0
#define ANTENNA_TXEN  1

/// 频率辅助计算参数
#define FREQ_STEP     61.03515625

/// 中断寄存器标识 支持的类型
#define IRQ_RXTIMEOUT                     0x80
#define IRQ_RXDONE                        0x40
#define IRQ_PAYLOADCRCERROR               0x20
#define IRQ_VALIDHEADER                   0x10
#define IRQ_TXDONE                        0x08
#define IRQ_CADDONE                       0x04
#define IRQ_FHSSCHANGEDCHANNEL            0x02
#define IRQ_CADDETECTED                   0x01

typedef enum
{
    GO_INIT          = 0x00,
    GO_BUSY          = 0x01,
    GO_STBY          = 0x02,
    GO_RECEIVE       = 0x03,
    GO_WAIT_RECEIVE  = 0x04,
    GO_TRANSMIT      = 0x05,
    GO_WAIT_TRANSMIT = 0x06,
    GO_SLEEP         = 0x07,
    GO_ERROR         = 0x08
} E19x_Status_t;

/// 状态标识
static E19x_Status_t E19x_Status = GO_INIT;

/// 接收数据缓存
static uint8e_t E19x_ReceiveBuffer[255] ;



/*!
 * @brief 切换模块内部收发电路
 *
 * @param mode 切换到指定模式
 *      @arg ANTENNA_RXEN: 接收模式
 *      @arg ANTENNA_TXEN: 发送模式
 *
 * @note 收发电路切换不正确会导致发射功率下降 甚至无法正常收发数据
 */
static void E19X_SetAntenna( uint8e_t mode )
{
    if( mode == ANTENNA_TXEN )
    {
        Ebyte_Port_RxenIoControl( 0 );
        Ebyte_Port_TxenIoControl( 1 );
    }
    else
    {
        Ebyte_Port_TxenIoControl( 0 );
        Ebyte_Port_RxenIoControl( 1 );
    }
}

/*!
 * @brief 批量写入寄存器
 *
 * @param address 寄存器起始地址
 * @param data    待写入数据首指针
 * @param size    寄存器数量
 */
static void E19x_SetRegisters( uint8e_t address, uint8e_t* data, uint8e_t size )
{
    uint8e_t i;
    /* CS片选 低电平选中 */
    Ebyte_Port_SpiCsIoControl( 0 );
    /* 寄存器起始地址 */
    Ebyte_Port_SpiTransmitAndReceivce( address | 0x80 ); //bit7=1 表示后续数据包为写入 即 |0x80
    /* 批量写入 */
    for( i = 0; i < size; i++ )
    {
        Ebyte_Port_SpiTransmitAndReceivce( data[i] );
    }
    /* CS片选 高电平 */
    Ebyte_Port_SpiCsIoControl( 1 );
}

/*!
 * @brief 批量读取寄存器
 *
 * @param address 寄存器起始地址
 * @param data    指向读取数据存放缓冲区
 * @param size    寄存器数量
 */
static void E19x_GetRegisters( uint8e_t address, uint8e_t* data, uint8e_t size )
{
    uint8e_t i;
    /* CS片选 低电平选中 */
    Ebyte_Port_SpiCsIoControl( 0 );
    /* 寄存器起始地址 */
    Ebyte_Port_SpiTransmitAndReceivce( address & 0x7F ); //bit7=0 表示后续数据包为读取 即 &0x7F
    /* 批量读取 */
    for( i = 0; i < size; i++ )
    {
        data[i] = Ebyte_Port_SpiTransmitAndReceivce( 0 );
    }
    /* CS片选 高电平 */
    Ebyte_Port_SpiCsIoControl( 1 );
}

/*!
 * @brief 写入单个寄存器
 *
 * @param address 寄存器起始地址
 * @param data    写入数据
 */
static void E19x_SetRegister( uint8e_t address, uint8e_t data )
{
    E19x_SetRegisters( address, &data, 1 );
}

/*!
 * @brief 读取单个寄存器
 *
 * @param address 寄存器起始地址
 * @param data    写入数据
 */
static uint8e_t E19x_GetRegister( uint8e_t address )
{
    uint8e_t data;
    E19x_GetRegisters( address, &data, 1 );
    return data;
}

/*!
 * @brief 将发送数据写入模块内部数据队列
 *
 * @param data 指向待发送数据包
 * @param size 写入数据长度
 *
 * @note 模块切换为发送模式后，会自动从队列中提取数据进行无线传输
 */
static void E19x_SetFIFO( uint8e_t* data, uint8e_t size )
{
    E19x_SetRegisters( 0, data, size );
}

/*!
 * @brief 读取模块内部数据队列
 *
 * @param data 指向用户接收数据缓存
 * @param size 接收数据长度
 *
 * @note 注意时序，接收模式时需要先设置读取地址，其次获取实际接收长度，完成后再读取FIFO
 */
static void E19x_GetFIFO( uint8e_t* data, uint8e_t size )
{
    E19x_GetRegisters( 0, data, size );
}

/*!
 * @brief 模块复位
 *
 * @note 内部寄存器参数将会还原为默认值
 */
static void E19x_Reset( void )
{
    Ebyte_Port_DelayMs( 10 );
    Ebyte_Port_RstIoControl( 0 );
    Ebyte_Port_DelayMs( 20 );
    Ebyte_Port_RstIoControl( 1 );
    Ebyte_Port_DelayMs( 20 );
}

/*!
 * @brief 设置模块GPIO(DIO)的功能
 *
 * @param regDioMapping1  寄存器地址:0x40
 * @param regDioMapping2  寄存器地址:0x41
 *
 * @note 一般用来指定一个引脚在接收或者发送完成时引发MCU中断
 *       引脚能够对应的中断类型是有限制的 请查阅SX1278手册(2.0.1 Digital IO Pin Mapping)
 */
void E19x_SetGPIO( uint8e_t regDioMapping1, uint8e_t regDioMapping2 )
{
    E19x_SetRegister( 0x40, regDioMapping1 );
    E19x_SetRegister( 0x41, regDioMapping2 );
}

/*!
 * @brief 判断模块是否存在
 *
 * @note 通过读取芯片版本号来实现的
 */
uint8e_t E19x_IsExist( void )
{
    uint8e_t result = 1;
    uint8e_t version;
    /* 读取芯片版本号 正常应返回0x12 寄存器地址:0x42   */
    version = E19x_GetRegister( 0x42 );
    if( version == 0x12 )
    {
        /* 正常 */
        result = 0;
    }
    return result;
}

/*!
 * @brief 允许中断
 *
 * @param irq 中断类型 见宏定义 按位或
 *      @arg IRQ_RXTIMEOUT
 *      @arg IRQ_RXDONE
 *      @arg IRQ_PAYLOADCRCERROR
 *      @arg IRQ_VALIDHEADER
 *      @arg IRQ_TXDONE
 *      @arg IRQ_CADDONE
 *      @arg IRQ_FHSSCHANGEDCHANNEL
 *      @arg IRQ_CADDETECTED
 */
void E19x_SetIRQ( uint8e_t irq )
{
    uint8e_t reg_value;
    /* 开启中断的机制为对应为置 0 */
    reg_value = ~irq;
    E19x_SetRegister( 0x11, reg_value );
}

/*!
 * @brief 获取中断标识
 *
 * @return 一般都是发送完成/接收完成
 *       0x80:IRQ_RXTIMEOUT
 *       0x40:IRQ_RXDONE
 *       0x20:IRQ_PAYLOADCRCERROR
 *       0x10:IRQ_VALIDHEADER
 *       0x08:IRQ_TXDONE
 *       0x04:IRQ_CADDONE
 *       0x02:IRQ_FHSSCHANGEDCHANNEL
 *       0x01:IRQ_CADDETECTED
 */
uint8e_t E19x_GetIRQ( void )
{
    /* 读取中断状态 寄存器地址:0x12 */
    return E19x_GetRegister( 0x12 );
}

/*!
 * @brief 清除所有中断标识
 */
void E19x_ClearIRQ( void )
{
    uint8e_t reg_value = 0;
    /* 读取中断状态 寄存器地址:0x12 */
    reg_value = E19x_GetRegister( 0x12 );
    /* 中断清除机制为回写对应位 1 */
    E19x_SetRegister( 0x12, reg_value );
}

/*!
 * @brief 进入休眠模式
 */
void E19x_SetSleep( void )
{
    uint8e_t result;
    /* 读取模式控制寄存器 寄存器地址:0x01   */
    result = E19x_GetRegister( 0x01 );
    /* bit2至bit0 3位表示模式 0代表SLEEP模式  */
    result &= 0xF8;//bit清零
    /* 回写 */
    E19x_SetRegister( 0x01, result );
    /* 状态记录 */
    E19x_Status = GO_SLEEP;
}

/*!
 * @brief 进入待机配置模式
 */
void E19x_SetStby( void )
{
    uint8e_t result;
    /* 读取模式控制寄存器 寄存器地址:0x01   */
    result = E19x_GetRegister( 0x01 );
    /* bit2至bit0 3位表示模式 0x01代表STDBY模式  */
    result &= 0xF8;//bit清零
    result |= 0x01;//bit置位
    /* 回写 */
    E19x_SetRegister( 0x01, result );
    /* 状态记录 */
    E19x_Status = GO_STBY;
}

/*!
 * @brief 进入发送模式
 */
void E19x_SetTransmit( void )
{
    uint8e_t result;
    /* 读取模式控制寄存器 寄存器地址:0x01   */
    result = E19x_GetRegister( 0x01 );
    /* bit2至bit0 3位表示模式 0x03代表TX模式  */
    result &= 0xF8;//bit清零
    result |= 0x03;//bit置位
    /* 回写 */
    E19x_SetRegister( 0x01, result );
    /* 状态记录  已经开始发送，所以这里状态为等待发送完成 */
    E19x_Status = GO_WAIT_TRANSMIT;
}

/*!
 * @brief 进入接收模式
 */
void E19x_SetReceive( void )
{
    uint8e_t result;
    /* 读取模式控制寄存器 寄存器地址:0x01   */
    result = E19x_GetRegister( 0x01 );
    /* bit2至bit0 3位表示模式 0x05代表连续RX模式  */
    result &= 0xF8;//bit清零
    result |= 0x05;//bit置位
    /* 回写 */
    E19x_SetRegister( 0x01, result );
    /* 状态记录  已经开始接收，所以这里状态为等待接收完成状态 */
    E19x_Status = GO_WAIT_RECEIVE;
}

/*!
 * @brief 进入信道检查模式 LBT？(Listen Before Talk)
 */
void E19x_SetCAD( void )
{
    uint8e_t result;
    /* 读取模式控制寄存器 寄存器地址:0x01   */
    result = E19x_GetRegister( 0x01 );
    /* bit2至bit0 3位表示模式 0x07代表连续CAD模式  */
    result &= 0xF8;//bit清零
    result |= 0x07;//bit置位
    /* 回写 */
    E19x_SetRegister( 0x01, result );
}

/*!
 * @brief LoRa模式初始化
 *
 * @param frequency         单位Hz  例如输入 433000000 表示433.0MHz
 * @param bandwidth         预定义频宽
 * @param spreading_factor  预定义扩频因子
 * @param coding_rate       预定义编码率
 * @param crc               CRC校验开关
 * @param preamble_length   前导码长度
 * @param sync_word         同步字
 * @param output_power      输出功率 (步进:1dBm)
 * @param low_datarate      低空速优化开关
 *
 * @return 0:正常  1:配置参数错误
 *
 * @note 预定义参数见ebyte_e19x.h中的宏定义 例如 E19X_LORA_BANDWIDTH
 */
uint8e_t E19x_SetLoRaInit( uint32e_t frequency, uint8e_t bandwidth, uint8e_t spreading_factor, uint8e_t coding_rate,
                           uint8e_t crc, uint16e_t preamble_length, uint8e_t sync_word, int8e_t output_power, uint8e_t low_datarate )
{
    uint8e_t config[8];
    uint8e_t reg_value;
    uint32e_t freq;
    /* 只能在Sleep模式下 切换调制方式 */
    E19x_SetSleep();
    /* 切换为LoRa模式 寄存器地址:0x01 bit7=1 */
    E19x_SetRegister( 0x01, 0x80 );
    /* 待机配置 */
    E19x_SetStby();
    /* 设置频率 */
    freq = ( uint32e_t )( ( double )frequency / ( double )FREQ_STEP );
    config[0] = ( uint8e_t )( ( freq >> 16 ) & 0xFF );
    config[1] = ( uint8e_t )( ( freq >> 8 ) & 0xFF );
    config[2] = ( uint8e_t )( freq & 0xFF );
    E19x_SetRegisters( 0x06, config, 3 );  //寄存器起始地址:0x06  共计3字节
    /* 设置扩频因子 SF
       SF = 6 是 LoRa 调制解调器可能实现的最高数据速率传输的特殊用例，需要单独配置 */
    if( spreading_factor < 6 || spreading_factor > 12 )
    {
        return 1;    //参数检查
    }
    if( spreading_factor == 6 )
    {
        reg_value = E19x_GetRegister( 0x31 );   //配置DetectionOptimize 寄存器地址:0x31 bit2至bit0
        reg_value = ( reg_value & 0xF8 ) | 0x05;//0x05专用于SF6
        E19x_SetRegister( 0x31, reg_value );
    }
    else
    {
        reg_value = E19x_GetRegister( 0x31 );   //配置DetectionOptimize 寄存器地址:0x31 bit2至bit0
        reg_value = ( reg_value & 0xF8 ) | 0x03;//0x03专用于SF7到SF12
        E19x_SetRegister( 0x31, reg_value );
    }
    reg_value = E19x_GetRegister( 0x1E );       //配置SpreadingFactor 寄存器地址:0x1E bit7至bit4
    reg_value = ( reg_value & 0x0F ) | ( spreading_factor << 4 );
    E19x_SetRegister( 0x1E, reg_value );
    /* 设置编码率 CR */
    if( coding_rate < 1 || coding_rate > 4 )
    {
        return 1;    //参数检查
    }
    reg_value = E19x_GetRegister( 0x1D );       //配置CodingRate 寄存器地址:0x1D bit3至bit1
    reg_value = ( reg_value & 0xF1 ) | ( coding_rate << 1 );
    E19x_SetRegister( 0x1D, reg_value );
    /* 设置数据包校验 CRC */
    if( crc > 1 )
    {
        return 1;    //参数检查
    }
    reg_value = E19x_GetRegister( 0x1E );       //配置RxPayloadCrcOn 寄存器地址:0x1E bit7至bit4
    reg_value = ( reg_value & 0xFB ) | ( crc << 2 );
    E19x_SetRegister( 0x1E, reg_value );
    /* 设置频宽  */
    if( bandwidth > 9 )
    {
        return 1;    //参数检查
    }
    reg_value = E19x_GetRegister( 0x1D );       //配置Bw  寄存器地址:0x1D bit7至bit4
    reg_value = ( reg_value & 0x0F ) | ( bandwidth << 4 );
    E19x_SetRegister( 0x1D, reg_value );
    /* 设置前导码长度 */
    config[0] = ( preamble_length >> 8 ) & 0x00FF;
    config[1] = preamble_length & 0xFF;
    E19x_SetRegisters( 0x20, config, 2 ); //寄存器起始地址:0x20  共计2字节
    /* 设置同步字 */
    E19x_SetRegister( 0x39, sync_word ); //寄存器起始地址:0x39
    /* 设置低空速优化 */
    if( low_datarate > 1 )
    {
        return 1;    //参数检查
    }
    reg_value = E19x_GetRegister( 0x26 );       //配置LowDataRateOptimize 寄存器地址:0x26 bit3
    reg_value = ( reg_value & 0xF7 ) | ( low_datarate << 3 );
    E19x_SetRegister( 0x26, reg_value );
    /* 设置输出功率 低功耗优先选择2-17为主 超出后切换为17-20 */
    if( output_power < 2 || output_power > 20 )
    {
        return 1;    //参数检查
    }
    if( output_power >= 17 ) // 17-20 dBm
    {
        E19x_SetRegister( 0x4D, 0x87 );          //配置RegPaDac 寄存器地址:0x4D  模块可控输出功率范围切换为 5dBm至20dBm
        reg_value  = E19x_GetRegister( 0x09 );   //配置PaSelect 寄存器地址:0x09
        reg_value |= 0x80 ;                      //bit7  PA_BOOST引脚输出
        reg_value |= 0x70 ;                      //配置MaxPower
        reg_value = ( reg_value & 0xF0 ) | ( ( uint8e_t )( ( uint16e_t )( output_power - 5 ) & 0x0F ) ); //配置OutputPower
        E19x_SetRegister( 0x09, reg_value );
        E19x_SetRegister( 0x0B, 0x3B );          //配置RegOcp   寄存器地址:0x0B  扩大电流 过流保护开启
    }
    else                  // 2-17 dBm
    {
        E19x_SetRegister( 0x4D, 0x84 );          //配置RegPaDac 寄存器地址:0x4D  模块可控输出功率范围切换为 2dBm至17dBm
        reg_value  = E19x_GetRegister( 0x09 );   //配置PaSelect 寄存器地址:0x09
        reg_value &= 0x7F ;                      //bit7  PA_BOOST引脚输出
        reg_value |= 0x70 ;                      //配置MaxPower
        reg_value = ( reg_value & 0xF0 ) | ( ( uint8e_t )( ( uint16e_t )( output_power - 2 ) & 0x0F ) ); //配置OutputPower
        E19x_SetRegister( 0x09, reg_value );
    }
    return 0;
}

/*!
 * @brief 模块开始监听，准备接收数据
 *
 * @return 0
 * @note
 */
uint8e_t E19x_GoReceive( void )
{
    /* 模式切换:待机 */
    E19x_SetStby();
    /* 关闭跳频 寄存器地址:0x24 */
    E19x_SetRegister( 0x24, 0 );
    /* 设置GPIO映射 这里配置DIO0映射到RxDone 即触发接收完成中断
       本例程采用轮询读取模块内部标志位的方式来判断是否接收完成。所以，IO配置了也没用到。
       如需考虑中断，仅需要在IO中断函数中读取数据，清除中断即可 */
    E19x_SetGPIO( 0, 0 );
    /* FIFO: 设置接收数据存放起始地址 从0开始  寄存器地址:0x0F */
    E19x_SetRegister( 0x0F, 0 );
    /* IO: 开启接收电路 */
    E19X_SetAntenna( ANTENNA_RXEN );
    /* 允许接收中断 */
    E19x_SetIRQ( IRQ_RXDONE );
    /* 清除中断标识 */
    E19x_ClearIRQ();
    /* 切换为接收模式 */
    E19x_SetReceive();
    return 0;
}

/*!
 * @brief 向模块写入数据，开始无线传输
 *
 * @param data 指向待发送数据包
 * @param size 数据包长度
 *
 * @return 0
 *
 * @note 数据包最大长度255
 */
uint8e_t E19x_GoTransmit( uint8e_t* data, uint8e_t size )
{
    uint8e_t irqStatus = 0;
    /* 模式切换:待机 */
    E19x_SetStby();
    /* 关闭跳频 寄存器地址:0x24 */
    E19x_SetRegister( 0x24, 0 );
    /* 设置数据包长度 寄存器地址:0x22 */
    E19x_SetRegister( 0x22, size );
    /* FIFO:发送区 发送地址从128开始  寄存器地址:0x0E */
    E19x_SetRegister( 0x0E, 0x80 );
    /* FIFO:发送区 SPI写入地址从128开始  寄存器地址:0x0D */
    E19x_SetRegister( 0x0D, 0x80 );
    /* FIFO:写入发送数据 */
    E19x_SetFIFO( data, size );
    /* 设置GPIO映射 这里配置DIO0映射到TxDone 即触发发送完成中断
       本例程采用轮询读取模块内部标志位的方式来判断是否发送完成。所以，IO配置了也没用到。
       如需考虑中断，需要配置宏定义 EBYTE_TRANSMIT_MODE_BLOCKED 并自行做IO中断处理 */
    E19x_SetGPIO( 0x40, 0x40 );
    /* IO: 开启发送电路 */
    E19X_SetAntenna( ANTENNA_TXEN );
    /* 允许发送中断 */
    E19x_SetIRQ( IRQ_TXDONE );
    /* 清除中断标识 */
    E19x_ClearIRQ();
    /* 切换为发送模式 */
    E19x_SetTransmit();
#if (EBYTE_TRANSMIT_MODE_BLOCKED)
    /* 阻塞 等待发送完成 */
    do
    {
        Ebyte_Port_DelayMs( 1 );
        irqStatus = E19x_GetIRQ();
    }
    while( irqStatus == 0xFF || ( !( irqStatus & IRQ_TXDONE ) ) ); //存在干扰时,有概率读回0xFF,这里认为是错误数据
    /* 到此发送完成 清除中断 */
    E19x_ClearIRQ();
    /* 待机 */
    E19x_SetStby();
    /* 回调用户处理函数 */
    Ebyte_Port_TransmitCallback( 0x0001 );
#endif
    return 0;
}

/*!
 * @brief 模块进入休眠(低功耗)
 *
 * @return 0
 */
uint8e_t E19x_GoSleep( void )
{
    E19x_SetSleep();
    return 0;
}


/*!
 * @brief BBYTE 无线模块初始化
 */
uint8e_t E19x_Init( void )
{
    uint8e_t result;
    /* 复位 */
    E19x_Reset();
    /* 检查：模块是否存在 */
    result = E19x_IsExist();
    if( result != 0 )
    {
        return 1;
    }
    /* 初始化：LoRa 或 FSK */
    switch( E19X_MODULATION_TYPE )
    {
        case 0:
            /* 执行LoRa模式初始化 配置见宏定义 */
            E19x_SetLoRaInit( E19X_FREQUENCY_START,        //载波频率
                              E19X_LORA_BANDWIDTH,         //频宽
                              E19X_LORA_SPREADING_FACTOR,  //扩频因子
                              E19X_LORA_CORING_RATE,       //编码率
                              E19X_LORA_CRC,               //CRC开关
                              E19X_LORA_PREAMBLE_LENGTH,   //前导码长度
                              E19X_LORA_SYNC_WORD,         //同步字
                              E19X_OUTPUT_POWER,           //输出功率
                              E19X_LORA_LOW_DATARATE );    //低空速优化
            break;
        case 1:
            //FSK
            break;
        default:
            break;
    }
    E19x_GoReceive();
    return 0;
}


/*!
 * @brief 保留
 */
void E19x_TaskForIRQ( void )
{
}

/*!
 * @brief 轮询函数 需要主函数循环调用
 *
 * @return 0
 * @note 读取内部标识位，从而完成数据接收
 */
uint8e_t E19x_TaskForPoll( void )
{
    uint8e_t irqStatus;
    /* 获取模块内部中断标识状态 */
    irqStatus = E19x_GetIRQ();
    /* 非0表示某一位中断置位 */
    if( irqStatus != 0 )
    {
        /* 如果是接收完成 */
        if( irqStatus & IRQ_RXDONE )
        {
            /* 模式切换:待机 */
            E19x_SetStby();
            /* 获取最新接收数据存放在FIFO中的位置  寄存器地址:0x10 */
            uint8e_t rxAddress = E19x_GetRegister( 0x10 );
            /* 获取最新接收数据的长度  寄存器地址:0x13 */
            uint8e_t rxLength = E19x_GetRegister( 0x13 );
            /* 设置SPI读取FIFO 起始地址  寄存器地址:0x0D */
            E19x_SetRegister( 0x0D, rxAddress );
            /* 读取FIFO数据 */
            E19x_GetFIFO( E19x_ReceiveBuffer, 100 );
            /* 回调用户函数 */
            Ebyte_Port_ReceiveCallback( 0x0001, E19x_ReceiveBuffer, rxLength );
        }
        /* 清除中断 */
        E19x_ClearIRQ();
    }
    return 0;
}

/*!
 * @brief 获取模块名字
 *
 * @return 指向名字字符串的指针
 * @note 标准字符串 末尾含有结束符 '\0'
 */
uint8e_t* E19x_GetName( void )
{
    return E19x_NameString;
}

/*!
 * @brief 获取模块程序版本
 *
 * @return 8位的编码
 * @note 例如0x10 代表V1.0
 */
uint8e_t E19x_GetDriverVersion( void )
{
    return  EBYTE_E19_PROGRAM_TYPE;
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
uint8e_t E19x_GetStatus( void )
{
    return ( uint8e_t )E19x_Status;;
}
