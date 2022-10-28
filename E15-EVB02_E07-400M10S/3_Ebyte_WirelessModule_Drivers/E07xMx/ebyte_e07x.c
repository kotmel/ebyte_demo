#include "ebyte_e07x.h"

/// 辅助识别模块
#if defined(EBYTE_E07_400M10S)
static uint8e_t E07x_NameString[] = "E07-400M10S";
#elif defined(EBYTE_E07_900M10S)
static uint8e_t E07x_NameString[] = "E07-900M10S";
#endif 

/// 辅助识别驱动程序版本号 
#define EBYTE_E07_TYPE_PROGRAM   0x10 

#define EBYTE_E07X_XOSC         26000000  //26MHz晶振
#define EBYTE_E07X_POW_2_28     268435456 //2的28次方
#define EBYTE_E07X_POW_2_17     131072
#define EBYTE_E07X_CAL_BASE     ((double)EBYTE_E07X_POW_2_28) / ((double)EBYTE_E07X_XOSC)

///指定内部FIFO
#define  FIFO_READ           0x00
#define  FIFO_WRITE          0x01   

///指定中断类型
#define IRQ_RX_SYNC_RECV 0x06  
#define IRQ_TX_SYNC_SEND 0x06  
#define IRQ_RX_CRC_OK    0x07

///输出功率表 辅助计算
static uint8e_t E07x_PaTabel[] = { 0xC0, 0, 0, 0, 0, 0, 0, 0};

///接收缓存
static uint8e_t E07x_RxBuffer[64];

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
}E07x_Status_t;

/// 状态标识
static E07x_Status_t E07x_Status = GO_INIT;

/* 基础配置参数 
 * 来自配置软件TI SmartRF Studio 7 
 * 版本 2.21.0 */
typedef struct {
    uint8e_t iocfg0;     // GDO0 Output Pin Configuration
    uint8e_t fifothr;    // RX FIFO and TX FIFO Thresholds
    uint8e_t sync1;      // Sync Word, High Byte
    uint8e_t sync0;      // Sync Word, Low Byte
    uint8e_t pktlen;     // Packet Length
    uint8e_t pktctrl0;   // Packet Automation Control
    uint8e_t fsctrl1;    // Frequency Synthesizer Control
    uint8e_t freq2;      // Frequency Control Word, High Byte
    uint8e_t freq1;      // Frequency Control Word, Middle Byte
    uint8e_t freq0;      // Frequency Control Word, Low Byte
    uint8e_t mdmcfg4;    // Modem Configuration
    uint8e_t mdmcfg3;    // Modem Configuration
    uint8e_t mdmcfg2;    // Modem Configuration
    uint8e_t mdmcfg1;    // Modem Configuration
    uint8e_t deviatn;    // Modem Deviation Setting
    uint8e_t mcsm0;      // Main Radio Control State Machine Configuration
    uint8e_t foccfg;     // Frequency Offset Compensation Configuration
    uint8e_t agcctrl2;   // AGC Control
    uint8e_t agcctrl1;   // AGC Control
    uint8e_t worctrl;    // Wake On Radio Control
    uint8e_t fscal3;     // Frequency Synthesizer Calibration
    uint8e_t fscal2;     // Frequency Synthesizer Calibration
    uint8e_t fscal1;     // Frequency Synthesizer Calibration
    uint8e_t fscal0;     // Frequency Synthesizer Calibration
    uint8e_t test2;      // Various Test Settings
    uint8e_t test1;      // Various Test Settings
    uint8e_t test0;      // Various Test Settings
} RF_SETTINGS;

static RF_SETTINGS E07x_InitSetting = {
    0x06,  // IOCFG0        GDO0 Output Pin Configuration
    0x47,  // FIFOTHR       RX FIFO and TX FIFO Thresholds
    0x7A,  // SYNC1         Sync Word, High Byte
    0x0E,  // SYNC0         Sync Word, Low Byte
    0x14,  // PKTLEN        Packet Length
    0x05,  // PKTCTRL0      Packet Automation Control
    0x06,  // FSCTRL1       Frequency Synthesizer Control
    0x10,  // FREQ2         Frequency Control Word, High Byte
    0xB1,  // FREQ1         Frequency Control Word, Middle Byte
    0x3B,  // FREQ0         Frequency Control Word, Low Byte
    0xF5,  // MDMCFG4       Modem Configuration
    0x83,  // MDMCFG3       Modem Configuration
    0x13,  // MDMCFG2       Modem Configuration
    0x02,  // MDMCFG1       Modem Configuration
    0x31,  // DEVIATN       Modem Deviation Setting
    0x18,  // MCSM0         Main Radio Control State Machine Configuration
    0x16,  // FOCCFG        Frequency Offset Compensation Configuration
    0x43,  // AGCCTRL2      AGC Control
    0x49,  // AGCCTRL1      AGC Control
    0xFB,  // WORCTRL       Wake On Radio Control
    0xE9,  // FSCAL3        Frequency Synthesizer Calibration
    0x2A,  // FSCAL2        Frequency Synthesizer Calibration
    0x00,  // FSCAL1        Frequency Synthesizer Calibration
    0x1F,  // FSCAL0        Frequency Synthesizer Calibration
    0x81,  // TEST2         Various Test Settings
    0x35,  // TEST1         Various Test Settings
    0x09,  // TEST0         Various Test Settings
};


/*!
 * @brief 设置内部寄存器值
 * 
 * @param address 寄存器地址
 * @param data    写入值
 */
static void E07x_SetRegister( uint8e_t address, uint8e_t data )
{
    /* SPI CS */
    Ebyte_Port_SpiCsIoControl(0);
    
    /* 写入地址 */
    Ebyte_Port_SpiTransmitAndReceivce( address );
    
    /* 写入值 */
    Ebyte_Port_SpiTransmitAndReceivce( data );
    
    /* SPI CS */
    Ebyte_Port_SpiCsIoControl(1);    
}

/*!
 * @brief 读取内部寄存器值
 * 
 * @param address 寄存器地址
 * @return 寄存器值
 */
static uint8e_t E07x_GetRegister( uint8e_t address )
{
    uint8e_t result;
    
 
    
    /* SPI CS */
    Ebyte_Port_SpiCsIoControl(0);
    
    /* 0x30地址以后的寄存器 读取指令需要改变 */
    if( address < 0x30 )
    {
        /* 写地址  bit7置1 表示后续需要读取 即|0x80 */
        Ebyte_Port_SpiTransmitAndReceivce( address | 0x80);    
    }
    else
    {
         /* 写地址  */
        Ebyte_Port_SpiTransmitAndReceivce( address | 0xC0);         
    }

    /* 读取 1Byte */
    result = Ebyte_Port_SpiTransmitAndReceivce( 0xFF );
    
    /* SPI CS */
    Ebyte_Port_SpiCsIoControl(1);  
    
    return result;
}

/*!
 * @brief 批量设置内部寄存器值
 * 
 * @param address 寄存器地址
 * @param data    指向写入数据首地址
 * @param size    写入长度
 */
static void E07x_SetRegisters( uint8e_t address , uint8e_t *data , uint8e_t size )
{
    uint8e_t i = size;
    
    /* SPI CS */
    Ebyte_Port_SpiCsIoControl(0);
    
    /* 写入起始地址 地址bit6置位代表连续写指令 即|0x40 */
    Ebyte_Port_SpiTransmitAndReceivce( address | 0x40 );
    
    /* 写入数据 */
    while( i-- )
    {
        Ebyte_Port_SpiTransmitAndReceivce( *data++ );
    }
    
    /* SPI CS */
    Ebyte_Port_SpiCsIoControl(1);
}

/*!
 * @brief 写命令
 * 
 * @param address 寄存器地址
 */
static void E07x_SendCommand( uint8e_t command )
{
    /* SPI CS */
    Ebyte_Port_SpiCsIoControl(0);
    
    /* 写入命令 */
    Ebyte_Port_SpiTransmitAndReceivce( command );
    
    /* SPI CS */
    Ebyte_Port_SpiCsIoControl(1);
}

/*!
 * @brief 向FIFO写入待发送数据
 * 
 * @param data 指向发送数据
 * @param size 写入长度
 */
static void E07x_SetFIFO( uint8e_t *data, uint8e_t size)
{
    uint8e_t i = size;
    
    /* SPI CS */
    Ebyte_Port_SpiCsIoControl(0);
    
    /* FIFO地址:0x3F 地址bit6置位代表连续写指令 即|0x40 */
    Ebyte_Port_SpiTransmitAndReceivce( 0x3F | 0x40 );
    
    /* 写入数据 */
    while( i-- )
    {
        Ebyte_Port_SpiTransmitAndReceivce( *data++ );
    }
    
    /* SPI CS */
    Ebyte_Port_SpiCsIoControl(1);
}

/*!
 * @brief 向FIFO读取数据
 * 
 * @param data 指向缓存
 * @param size 读取长度
 */
static void E07x_GetFIFO( uint8e_t *data, uint8e_t size)
{
    uint8e_t i = size;
    
    /* SPI CS */
    Ebyte_Port_SpiCsIoControl(0);
    
    /* FIFO地址:0x3F 地址bit6置位代表连续读指令 即|0x40 */
    Ebyte_Port_SpiTransmitAndReceivce( 0x3F | 0xC0 );
    
    /* 写入数据 */
    while( i-- )
    {
        *data++ = Ebyte_Port_SpiTransmitAndReceivce( 0xFF );
    }
    
    /* SPI CS */
    Ebyte_Port_SpiCsIoControl(1);
}

/*!
 * @brief 清除FIFO内容
 * @param mode 模式
 *           @arg FIFO_WRITE ：指定发送FIFO
 *           @arg FIFO_READ  ：指定接收FIFO
 */
static void E07x_ClearFIFO( uint8e_t mode )
{
  
    if( mode == FIFO_WRITE )
    {    
        /* 指令:清除发送FIFO(0x3B) */
        E07x_SendCommand(0x3B);
    }
    else
    {
        /* 指令:清除接收FIFO(0x3A) */
        E07x_SendCommand(0x3A);
    } 
    
}

/*!
 * @brief 软复位
 */
void E07x_Reset(void)
{
    /* SPI CS */
    Ebyte_Port_SpiCsIoControl(1);
    Ebyte_Port_SpiCsIoControl(0);
    Ebyte_Port_DelayMs(1);
    Ebyte_Port_SpiCsIoControl(1);
    Ebyte_Port_DelayMs(1);
    
    /* 命令:芯片复位(0x30) */
    E07x_SendCommand( 0x30 );
    
    Ebyte_Port_DelayMs(20);
}

/*!
 * @brief 切换进入待机模式
 */
static void E07x_SetStby(void)
{
    /* 指令:0x36 退出TX/RX 模式 进入空闲状态 */
    E07x_SendCommand(0x36);
    
    /* 状态记录 */
    E07x_Status = GO_STBY;
}

/*!
 * @brief 切换进入休眠模式
 * 
 * @note 请查阅手册，部分寄存器的参数会丢失！
 */
static void E07x_SetSleep(void)
{
    /* 指令:0x39 进入休眠模式 */
    E07x_SendCommand(0x39);
    
    /* 状态记录 */
    E07x_Status = GO_SLEEP;
}

static void E07x_SetGPIO( uint8e_t gpio , uint8e_t config )
{
  
  /* 用于指定IO的极性 无中断时为高电平 触发中断时为低电平 */
  uint8e_t mask = 0x40;
  
  /* 合并中断类型 */ 
  mask |= (config & 0x3F);
  
  switch ( gpio )
  {
    /* IOCFG0 寄存器地址:0x02 */
    case 0:  E07x_SetRegister(0x02,mask) ;break;
    /* IOCFG1 寄存器地址:0x01 */
    case 1:  E07x_SetRegister(0x01,mask) ;break;
    /* IOCFG2 寄存器地址:0x00 */
    case 2:  E07x_SetRegister(0x00,mask) ;break;
    default: break;
  }
}

/*!
 * @brief 判断模块是否存在
 *
 * @return 0:正常 1:模块异常
 */
static uint8e_t E07x_IsExist(void)
{
    uint8e_t result = 0;
    uint8e_t reg_value;
    
    /* 读温度传感器配置参数 复位后应该为0x7F 寄存器地址:0x2A  */
    reg_value = E07x_GetRegister(0x2A);
    
    /* 默认值必为0x7F */
    if( reg_value != 0x7F ) 
    {
       /* 读不到正确数据 请检查硬件 */
       result = 1;
    }
    
    return result;
}

/*!
 * @brief 调制方式
 *
 * @param mod 期望的调制方式 
 *        @arg 0:2-FSK
 *        @arg 1:GFSK
 *        @arg 3:ASK/OOK
 *        @arg 4:4-FSK
 *        @arg 7:MSK
 */
static void E07x_SetModulation(uint8e_t mod)
{
    /* 转换位 */
    uint8e_t tmp = (mod<<4) & 0x70;
  
    /* MDMCFG2 寄存器地址 0x12  清除bit[6:4]*/
    E07x_InitSetting.mdmcfg2  &= 0x8F;
    
    /* 设置bit[6:4] */
    E07x_InitSetting.mdmcfg2 |= tmp;
}

/*!
 * @brief 自由载波频率基准值计算
 *
 * @param frequency 期望的载波频率 Hz
 * @return 0:正常 1:参数错误
 * @note 必须在以下频段之间( Hz ):
 *        300000000 - 348000000
 *        387000000 - 464000000
 *        779000000 - 928000000
 *       ！！！注意:不同架构处理器计算结果小数部分可能不一致，有可能产生偏差
 *
 *       频谱仪测试频点OK。实测可与官方计算器参数配置频率的模块进行通信。
 */
static uint8e_t E07x_SetFrequency( uint32e_t frequency )
{ 
   uint32e_t freq ;
   uint8e_t  freq2,freq1,freq0;
   
   /* 参数检查 */
   if(  frequency > 928000000 ) return 1;
   if( (frequency < 779000000) && ( frequency>464000000) )return 1;
   if( (frequency < 387000000) && ( frequency>348000000) )return 1;
   if( (frequency < 300000000)) return 1;
   
   /* 频率控制器字(FREQ)=载波频率 *（2^16)/ 晶振频率   */
   freq = (uint32e_t)(((double)frequency * 65535 )/ ( (double)EBYTE_E07X_XOSC ));
   freq2 = ( uint8e_t )( ( freq >> 16 ) & 0xFF );
   freq1 = ( uint8e_t )( ( freq >> 8 ) & 0xFF );
   freq0 = ( uint8e_t )(  freq  & 0xFF );
   
   /* 填充初始化参数数组中的频率部分 等待后续统一写入  */
   E07x_InitSetting.freq2 = freq2;
   E07x_InitSetting.freq1 = freq1;
   E07x_InitSetting.freq0 = freq0;
   
   return 0;
}

/*!
 * @brief 自由数据速率基准值计算
 *
 * @param datarate 期望的数据速率
 * @note  建议在以下速率之间:
 *        300 (0.3K) - 250000000 (250K)
 */
static void E07x_SetDataRate( uint32e_t datarate )
{
    uint8e_t  date_m ;
    uint16e_t date_e ;
    uint32e_t date_e_sum = 1,temp;
    
    /* 中间公式换算 (256+DRATE_M)*2^DRATE_E=datarate*2^28/晶振频率  */
    uint32e_t calculation = (uint32e_t)( datarate * EBYTE_E07X_CAL_BASE);
    
    /* 逐次逼近匹配 DRATE_E */
    for( date_e=0 ; date_e<=0xFF;date_e++ )
    {
       if(date_e==0)
       {
          date_e_sum =1;//2^0
       }
       else
       {
          date_e_sum *= 2;//2^DRATE_E 
       }
       
       temp = calculation/date_e_sum; // 结果必须在 [256 ,256+DRATE_M]之间
       if(  temp>=256 && temp<=511 )
       {
          date_m = temp - 256;//得到DRATE_M
          break;
       }
    }
    
    /* 填充初始化参数数组中的数据速率部分 等待后续统一写入  */
    if ( date_e<256 )
    {
       E07x_InitSetting.mdmcfg4 &= 0xF0;
       E07x_InitSetting.mdmcfg4 |= date_e;
       E07x_InitSetting.mdmcfg3 = date_m;    
    }
    
}

/*!
 * @brief 自由频宽基准值计算
 *
 * @param bandwidth 期望的接收频宽 (Hz)
 * @note 实测和官方计算器结果一致 默认最低58.035714
 *       注意：芯片实际只能配置为固定几个频宽，输入的数值会被向上转化接近到如下固定频宽：
 *        58.035714 KHz 
 *        67.708333 KHz 
 *        81.250000 KHz
 *       101.562500 KHz 
 *       116.071429 KHz
 *       135.416667 KHz
 *       162.500000 KHz
 *       203.125000 KHz
 *       232.142857 KHz
 *       270.833333 KHz
 *       325.000000 KHz
 *       406.250000 KHz
 *       464.285714 KHz
 *       541.666667 KHz
 *       650.000000 KHz
 *       812.500000 KHz
 */
static void E07x_SetChannelBandwidth( uint32e_t bandwidth )
{
    uint8e_t  chanbw_m ,chanbw_e,mask;

    uint32e_t chanbw_e_sum = 1,temp;
    
    /* 中间公式换算 (4+CHANBW_M)*2^CHANBW_E=晶振频率/(8*bandwidth)  */
    uint32e_t calculation = (uint32e_t)( EBYTE_E07X_XOSC/(8 * bandwidth));    
    
    /* 逐次逼近匹配 CHANBW_E */
    for( chanbw_e=0 ; chanbw_e<=3;chanbw_e++ )
    {
       if(chanbw_e==0)
       {
          chanbw_e_sum = 1;//2^0
       }
       else
       {
          chanbw_e_sum *= 2;//2^CHANBW_E
       }
        
       temp = calculation/chanbw_e_sum; 
       if(  temp>=4 && temp<=7 )
       {
          chanbw_m = temp - 4;//得到CHANBW_M
          break;
       }
    }    
    
    /* 填充初始化参数数组中的频宽部分 等待后续统一写入  */
    if( chanbw_e<=3 )
    {
        mask = ((uint8e_t)((chanbw_e<<6)|(chanbw_m<<4))) & 0xF0;
        E07x_InitSetting.mdmcfg4 &= 0x0F;
        E07x_InitSetting.mdmcfg4 |= mask;
    }
    
}

/*!
 * @brief 计算频偏基准值
 * 
 * @param frequency_dev 单位:Hz 
 * 
 * @note 实测和官方计算结果一致
 */
static void E07x_SetFrequencyDeviation( uint32e_t frequency_dev )
{
    uint8e_t  deviation_m,deviation_e,mask;
    uint32e_t deviation_e_sum = 1,temp;
    
    /* 中间公式换算 (8+DEVIATION_M)*2^DEVIATION_E=frequency_dev * 2^17 / 晶振频率  */
    uint32e_t calculation = (uint32e_t)( frequency_dev * EBYTE_E07X_POW_2_17 / EBYTE_E07X_XOSC );   
    
    /* 逐次逼近匹配 DEVIATION_E */
    for( deviation_e=0 ; deviation_e<=7;deviation_e++ )
    {
       if(deviation_e==0)
       {
          deviation_e_sum = 1;//2^0
       }
       else
       {
          deviation_e_sum *= 2;//2^DEVIATION_E
       }
        
       temp = calculation/deviation_e_sum; //得到 8+DEVIATION_M
       
       if(  temp>=8 && temp<=15 )
       {
          deviation_m = temp - 8;//得到CHANBW_M
          break;
       }
    }      
    
    /* 填充初始化参数数组中的频偏部分 等待后续统一写入  */
    if( deviation_e<=7 )
    {
        mask = ((uint8e_t)((deviation_e<<4)|deviation_m)) & 0x77;
        E07x_InitSetting.deviatn &= 0x88;
        E07x_InitSetting.deviatn |= mask;
    }    
}

/*!
 * @brief 设置输出功率
 * 
 * @param power 单位 dBm 注意:输入值存在限制  
 * 
 * @return 0：正常 1:参数不正确
 * @note 放弃了PA Ramping，也许失去了一定的线性度和完整性？杂散信号增加？ 
 *       功率的输出并不是线性的，函数中设置的PA值来自TI SmartRF Studio，仅为参考
 */
static uint8e_t E07x_SetOutputPower( int8e_t power )
{
    uint8e_t result = 0;
#if defined(EBYTE_E07_400M10S)
    switch ( power ) 
    {
        case 10:  E07x_PaTabel[0] = 0xC0;break;
        case 7:   E07x_PaTabel[0] = 0xC8;break;
        case 5:   E07x_PaTabel[0] = 0x84;break;
        case 0:   E07x_PaTabel[0] = 0x60;break;
        case -10: E07x_PaTabel[0] = 0x34;break;
        case -15: E07x_PaTabel[0] = 0x1D;break;
        case -20: E07x_PaTabel[0] = 0x0E;break;
        case -30: E07x_PaTabel[0] = 0x12;break;        
        default: result=1; break;
    }
#elif defined(EBYTE_E07_900M10S)  
    
    if( E07_FREQUENCY_START > 900000000 ) //900MHz 到 928MHz
    {
        switch ( power ) 
        {
            case 11:  E07x_PaTabel[0] = 0xC0;break; //此频率区间 功率可以提升到11dBm    
            case 10:  E07x_PaTabel[0] = 0xC3;break;
            case 7:   E07x_PaTabel[0] = 0xCC;break;
            case 5:   E07x_PaTabel[0] = 0x84;break;
            case 0:   E07x_PaTabel[0] = 0x8E;break;
            case -10: E07x_PaTabel[0] = 0x27;break;
            case -15: E07x_PaTabel[0] = 0x1E;break;
            case -20: E07x_PaTabel[0] = 0x0E;break;
            case -30: E07x_PaTabel[0] = 0x03;break;        
            default: result=1; break;
        }    
    }
    else //779MHz 到 900MHz
    {
        switch ( power ) 
        {
            case 12:  E07x_PaTabel[0] = 0xC0;break; //此频率区间 功率可以提升到12dBm    
            case 10:  E07x_PaTabel[0] = 0xC5;break;
            case 7:   E07x_PaTabel[0] = 0xCD;break;
            case 5:   E07x_PaTabel[0] = 0x86;break;
            case 0:   E07x_PaTabel[0] = 0x50;break;
            case -10: E07x_PaTabel[0] = 0x26;break;
            case -15: E07x_PaTabel[0] = 0x1D;break;
            case -20: E07x_PaTabel[0] = 0x17;break;
            case -30: E07x_PaTabel[0] = 0x03;break;        
            default: result=1; break;
        }            
    }

#endif    

    
    return result;
}

/*!
 * @brief 模块基础参数配置
 * 
 * @param frequency      载波频率
 * @param data_rate      符号速率(空中速率)
 * @param frequency_dev  调制频偏
 * @param bandwidth      接收频宽
 * @param output_power   输出功率
 * @param preamble_size  发送前导码长度 
 * @param sync_word      同步字
 * @param crc            CRC开关
 * @param device_address 设备地址
 *
 * @return 0:正常 1:参数错误
 * @note (可选)默认关闭了地址检查
 */
static uint8e_t E07x_Config( uint32e_t frequency , uint32e_t data_rate , uint32e_t frequency_dev ,uint32e_t bandwidth, 
                              int8e_t output_power , uint16e_t preamble_size, uint16e_t sync_word , uint8e_t crc )
{
    uint8e_t result ;  
    uint8e_t reg_value;
    
    /* 计算:载波频率 
     * 寄存器起始地址 0x0D */
    result = E07x_SetFrequency( frequency );
    if( result != 0 ) return 1;
    
    /* 计算:空速 
     * MDMCFG4 MDMCFG3寄存器地址:0x10 0x11 */
    E07x_SetDataRate( data_rate );
    
    /* 计算:频偏 
     * DEVIATN寄存器地址:0x15 */
    E07x_SetFrequencyDeviation( frequency_dev );    
    
    /* 计算:接收频宽 
     * MDMCFG4寄存器地址:0x10 */
    E07x_SetChannelBandwidth( bandwidth );
    
    /* 计算:输出功率 
     * 寄存器地址:0x3E */
    E07x_SetOutputPower(output_power);
    
    /* 调制模式
     * MDMCFG2 寄存器地址:0x12 */
    E07x_SetModulation(1);//GFSK
    
    /* 前导码长度 
     * MDMCFG1 寄存器地址:0x13 */  
    if( preamble_size >7 ) return 1;//参数检查
    E07x_InitSetting.mdmcfg1 &= 0x8F;//清零
    E07x_InitSetting.mdmcfg1 |= (preamble_size<<4);//置位
    
    /* 同步字 
     * SYNC1 SYNC0 寄存器地址:0x04 0x05 */
    E07x_InitSetting.sync1 =  (uint8e_t)((sync_word>>8)&0xFF);//高Byte
    E07x_InitSetting.sync0 =  (uint8e_t)(sync_word & 0xFF);   //低Byte
    
    /* CRC开关 
     * PKTCTRL0寄存器地址:0x08 bit2 */
    if( crc > 1 ) return 1;//参数检查 只能为0或1
    if( crc )
    {
        E07x_InitSetting.pktctrl0 |= 0x04; //开启 
    }else
    {
        E07x_InitSetting.pktctrl0 &= 0xFB;
    }
    
    /* 数据包长度 默认为可变长模式且长度放在数据第一字节
     * PKTCTRL0寄存器地址:0x08 bit[1:0] */
    E07x_InitSetting.pktctrl0 &= 0xFC;//清零
    E07x_InitSetting.pktctrl0 |= 0x01;//0x01模式 (可变长)
    
    /* 注意事项:如果希望完全使用官方SmartRF Studio的配置参数，那么直接注释掉上方的修改函数即可 */
    /* 向模块写入计算后的 E07x_InitSetting 参数表 */
    E07x_SetRegister( 0x02, E07x_InitSetting.iocfg0 );
    E07x_SetRegister( 0x03, E07x_InitSetting.fifothr);
    E07x_SetRegister( 0x04, E07x_InitSetting.sync1);
    E07x_SetRegister( 0x05, E07x_InitSetting.sync0);    
    E07x_SetRegister( 0x08, E07x_InitSetting.pktctrl0);
    E07x_SetRegister( 0x0B, E07x_InitSetting.fsctrl1);
    E07x_SetRegister( 0x0D, E07x_InitSetting.freq2);
    E07x_SetRegister( 0x0E, E07x_InitSetting.freq1);
    E07x_SetRegister( 0x0F, E07x_InitSetting.freq0);
    E07x_SetRegister( 0x10, E07x_InitSetting.mdmcfg4);
    E07x_SetRegister( 0x11, E07x_InitSetting.mdmcfg3);
    E07x_SetRegister( 0x12, E07x_InitSetting.mdmcfg2);
    E07x_SetRegister( 0x13, E07x_InitSetting.mdmcfg1);
    E07x_SetRegister( 0x15, E07x_InitSetting.deviatn);
    E07x_SetRegister( 0x18, E07x_InitSetting.mcsm0);
    E07x_SetRegister( 0x19, E07x_InitSetting.foccfg);
    E07x_SetRegister( 0x1B, E07x_InitSetting.agcctrl2);
    E07x_SetRegister( 0x1C, E07x_InitSetting.agcctrl1);
    E07x_SetRegister( 0x20, E07x_InitSetting.worctrl);
    E07x_SetRegister( 0x23, E07x_InitSetting.fscal3);
    E07x_SetRegister( 0x24, E07x_InitSetting.fscal2);
    E07x_SetRegister( 0x25, E07x_InitSetting.fscal1);
    E07x_SetRegister( 0x26, E07x_InitSetting.fscal0);
    E07x_SetRegister( 0x2C, E07x_InitSetting.test2);
    E07x_SetRegister( 0x2D, E07x_InitSetting.test1);
//    E07x_SetRegister( 0x2E, E07x_InitSetting.test0); //发现: 设置TEST0导致自定义计算频率偏移  
                                                       //现象: 当频点设置为小于411MHz时，频点锁死在了411MHz。当频点设置为大于411MHz时，频点切换正常
                                                       //建议: 当完全启用官方SmartRF Studio参数时，解除TEST0的注释。当使用自由参数计算时，保持注释
    
    /* 补充：通道编码(可用于跳频)  寄存器地址: 0x0A */
    E07x_SetRegister( 0x0A ,0 );
    
    /* 补充：关闭地址过滤  寄存器地址: 0x07 */
    reg_value = E07x_GetRegister(0x07);
    reg_value &= 0xFC;//清零
    E07x_SetRegister(0x07,reg_value);
    
    /* 补充: 配置输出功率 寄存器地址: 0x3E */
    E07x_SetRegisters( 0x3E ,E07x_PaTabel,8 );
    
    return 0;
}

/*!
 * @brief 模块初始化
 * 
 * @return 0:正常 1:初始化失败
 */
uint8e_t E07x_Init( void )
{
    uint8e_t result = 0;
    
    /* 软复位 */
    E07x_Reset();
    
    /* 判断模块是否存在 */
    result = E07x_IsExist();
    if( result != 0 ) return 1;
    
    /* 基础参数设置 */
    result = E07x_Config(  E07_FREQUENCY_START,
                           E07_DATA_RATE, 
                           E07_FREQUENCY_DEVIATION,
                           E07_BANDWIDTH,
                           E07_OUTPUT_POWER,
                           E07_PREAMBLE_SIZE,
                           E07_SYNC_WORD,
                           E07_IS_CRC
                           );
    if( result != 0 ) return 1;
    
    /* 默认进入接收模式 */
    E07x_GoReceive();
    return 0;
}

/*!
 * @brief 保留 
 */
void E07x_TaskForIRQ(void)
{

}

/*!
 * @brief 轮询函数 辅助完成数据接收
 *  
 * @param data 指向发送数据
 * @param size 数据长度
 * @return 0
 */
uint8e_t E07x_GoTransmit( uint8e_t *data, uint8e_t size )
{
    uint8e_t irqStatus = 0;
    
    /* 模式切换: 待机 */
    E07x_SetStby();
    
    /* 清除发送数据队列 */
    E07x_ClearFIFO( FIFO_WRITE );
   
    /* 发送数据为可变长模 第一字节先写入长度 */
    E07x_SetFIFO( &size , 1 );
    
    /* 后续写入完整数据包 */
    E07x_SetFIFO( data , size);
    
    /* 设置引脚 GPIO0 映射到中断
     * 当发送进行同步字时 触发中断 ！！！注意:当FIFO中的数据完全发送后 自动！清除中断 */
    E07x_SetGPIO(0 ,IRQ_TX_SYNC_SEND );

    /* 发送指令:0x35 模块进入接收模式 */   
    E07x_SendCommand( 0x35 );      
    
    /* 状态记录 */
    E07x_Status = GO_WAIT_TRANSMIT;    
    
    /* 阻塞发送方式 在此等待发送完成 */  
    do
    {
      Ebyte_Port_DelayMs(1);
      
      /* 读取状态寄存器 MARCSTATE 地址:0x35 */
      irqStatus = E07x_GetRegister(0x35);

    }
    while( (irqStatus != 0x01) );//发送完成后自动退回到空闲状态 即为0x01  
                                  //如果设置了寄存器MCSM1 地址:0x17 开启了TXOFF_MODE 那么发送完成后状态并不一定是空闲 需要另行处理 
    
    /* 状态记录 */
    E07x_Status = GO_STBY;  
    
    /* 回调用户函数 */
    Ebyte_Port_TransmitCallback( 0x0001 );
    
    return 0;
}

/*!
 * @brief 开始监听数据
 *  
 * @return 0
 */
uint8e_t E07x_GoReceive( void )
{
  
    /* 模式切换: 待机 */
    E07x_SetStby();
    
    /* 清除接收数据队列 */
    E07x_ClearFIFO( FIFO_READ );  
  
    /* 设置引脚 GPIO0 映射到中断 */
//    E07x_SetGPIO(0 ,IRQ_RX_CRC_OK );
     E07x_SetGPIO(0 ,IRQ_RX_SYNC_RECV );
  
    /* 发送指令:0x34 模块进入接收模式 */   
    E07x_SendCommand( 0x34 );  
    
    /* 状态记录 */
    E07x_Status = GO_WAIT_RECEIVE;
    return 0;
}

/*!
 * @brief 模块进入休眠模式(低功耗)
 * 
 * @return 0;
 */
uint8e_t E07x_GoSleep(void)
{
    E07x_SetSleep();
    
    return 0;
}

/*!
 * @brief 获取模块程序版本
 * 
 * @return 8位的编码 
 * @note 例如0x10 代表V1.0
 */
uint8e_t E07x_GetDriverVersion(void)
{
  return  EBYTE_E07_TYPE_PROGRAM;
}

/*!
 * @brief 获取模块名字
 * 
 * @return 指向名字字符串的指针 
 * @note 标准字符串 末尾含有结束符 '\0'
 */
uint8e_t* E07x_GetName(void)
{
    return E07x_NameString;
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
uint8e_t E07x_GetStatus(void)
{
  return (uint8e_t)E07x_Status;;
}

/*!
 * @brief 轮询函数 辅助完成数据接收
 *  
 * @return 0
 * @note 需要被主函数周期性调用
 */
uint8e_t E07x_TaskForPoll(void)
{
    uint8e_t recvSize;
    if (0 == Ebyte_Port_Gdo0IoRead())             // 检测无线模块是否产生接收中断 
    {
        while (Ebyte_Port_Gdo0IoRead() == 0);

        // 读取接收到的数据长度 
        recvSize = 0;
        E07x_GetFIFO(&recvSize , 1);

        // 如果接收长度不为0
        if( recvSize != 0 )
        {
            E07x_GetFIFO( E07x_RxBuffer, recvSize );
            //回调用户函数
            Ebyte_Port_ReceiveCallback( 0x0002 , E07x_RxBuffer , recvSize );            
        }
        else
        {
            //To-Do 请自行处理其他情况 例如只有前导码?干扰型的中断？
            E07x_GoReceive();//示例 继续接收
        }
    }

    return 0;
}