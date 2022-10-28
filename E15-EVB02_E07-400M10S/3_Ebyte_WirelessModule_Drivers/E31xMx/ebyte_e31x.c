#include "ebyte_e31x.h"
#include "ebyte_e31x_register_def.h"

///辅助识别模块频段
#if defined(EBYTE_E31_400M17S)
static uint8e_t E31x_NameString[] = "E31-400M17S";
#elif defined(EBYTE_E31_900M17S)
static uint8e_t E31x_NameString[] = "E31-900M17S";
#endif

///辅助识别驱动程序版本号
#define EBYTE_E31_PROGRAM_TYPE 0x10

///晶振频率
#define  XTAL_FREQ      26000000

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
}E31x_Status_t;

/// 状态标识
static E31x_Status_t E31x_Status = GO_INIT;

///接收缓存
static uint8e_t E31x_RxBuffer[255] ;


static uint8e_t  E31x_PreambleSize;
static uint32e_t E31x_SyncWord;
static uint8e_t E31x_CrcOnOff;
static uint8e_t E31x_PllRxRange;
static uint8e_t E31x_PllTxRange;

const uint16e_t E31x_RegisterVauleInit[][2] =
{
    { REG_AX5043_MODULATION,   0x08 },
    { REG_AX5043_ENCODING,   0x00   },
    { REG_AX5043_FRAMING,   0x26   }, // 前导码模式：Raw, Pattern Match  CRC模式：CRC-16
    { REG_AX5043_PINFUNCSYSCLK,  0x01},
    { REG_AX5043_PINFUNCDCLK, 0x01  },
    { REG_AX5043_PINFUNCDATA, 0x01  },
    { REG_AX5043_PINFUNCANTSEL, 0x00},
    { REG_AX5043_PINFUNCPWRAMP, 0x01},
    { REG_AX5043_WAKEUPXOEARLY, 0x01},
    { REG_AX5043_IFFREQ1, 0x00   },
    { REG_AX5043_IFFREQ0, 0xFC   },
    { REG_AX5043_DECIMATION, 0x5A  },
    { REG_AX5043_RXDATARATE2, 0x00  },
    { REG_AX5043_RXDATARATE1, 0x3c  },
    { REG_AX5043_RXDATARATE0, 0x2f  },
    { REG_AX5043_MAXDROFFSET2, 0x00 },
    { REG_AX5043_MAXDROFFSET1, 0x00 },
    { REG_AX5043_MAXDROFFSET0, 0x00 },
    { REG_AX5043_MAXRFOFFSET2, 0x80 },
    { REG_AX5043_MAXRFOFFSET1, 0x01 },
    { REG_AX5043_MAXRFOFFSET0, 0x17 },
    { REG_AX5043_FSKDMAX1, 0x00   },
    { REG_AX5043_FSKDMAX0, 0xA6   },
    { REG_AX5043_FSKDMIN1, 0xFF   },
    { REG_AX5043_FSKDMIN0, 0x5a   },
    { REG_AX5043_AMPLFILTER, 0x00   },
    { REG_AX5043_RXPARAMSETS, 0xF4  },
    { REG_AX5043_AGCGAIN0, 0x97   },
    { REG_AX5043_AGCTARGET0, 0x76   },
    { REG_AX5043_TIMEGAIN0, 0xf8   },
    { REG_AX5043_DRGAIN0, 0xf2   },
    { REG_AX5043_PHASEGAIN0,  0xc3  },
    { REG_AX5043_FREQUENCYGAINA0, 0x0f},
    { REG_AX5043_FREQUENCYGAINB0, 0x1f},
    { REG_AX5043_FREQUENCYGAINC0, 0x0a},
    { REG_AX5043_FREQUENCYGAIND0, 0x0a},
    { REG_AX5043_AMPLITUDEGAIN0, 0x46 },
    { REG_AX5043_FREQDEV10, 0x00   },
    { REG_AX5043_FREQDEV00, 0x00   },
    { REG_AX5043_FOURFSK0, 0x00   },
    { REG_AX5043_BBOFFSRES0, 0x00  },
    { REG_AX5043_AGCGAIN1, 0x97   },
    { REG_AX5043_AGCTARGET1, 0x76  },
    { REG_AX5043_AGCAHYST1, 0x00   },
    { REG_AX5043_AGCMINMAX1, 0x00  },
    { REG_AX5043_TIMEGAIN1, 0xf6   },
    { REG_AX5043_DRGAIN1, 0xf1   },
    { REG_AX5043_PHASEGAIN1, 0xc3  },
    { REG_AX5043_FREQUENCYGAINA1, 0x0f},
    { REG_AX5043_FREQUENCYGAINB1, 0x1f},
    { REG_AX5043_FREQUENCYGAINC1, 0x0a},
    { REG_AX5043_FREQUENCYGAIND1, 0x0a},
    { REG_AX5043_AMPLITUDEGAIN1, 0x46 },
    { REG_AX5043_FREQDEV11, 0x00   },
    { REG_AX5043_FREQDEV01, 0x38   },
    { REG_AX5043_FOURFSK1, 0x00   },
    { REG_AX5043_BBOFFSRES1, 0x00  },
    { REG_AX5043_AGCGAIN3, 0xff   },
    { REG_AX5043_AGCTARGET3, 0x76  },
    { REG_AX5043_AGCAHYST3, 0x00   },
    { REG_AX5043_AGCMINMAX3, 0x00  },
    { REG_AX5043_TIMEGAIN3, 0xf5   },
    { REG_AX5043_DRGAIN3, 0xf0   },
    { REG_AX5043_PHASEGAIN3, 0xc3  },
    { REG_AX5043_FREQUENCYGAINA3, 0x0f},
    { REG_AX5043_FREQUENCYGAINB3, 0x1f},
    { REG_AX5043_FREQUENCYGAINC3, 0x0d},
    { REG_AX5043_FREQUENCYGAIND3, 0x0d},
    { REG_AX5043_AMPLITUDEGAIN3, 0x46 },
    { REG_AX5043_FREQDEV13, 0x00  },
    { REG_AX5043_FREQDEV03, 0x38  },
    { REG_AX5043_FOURFSK3, 0x00  },
    { REG_AX5043_BBOFFSRES3, 0x00 },
    { REG_AX5043_MODCFGF, 0x03  },
    { REG_AX5043_FSKDEV2, 0x00  },
    { REG_AX5043_FSKDEV1, 0x01  },
    { REG_AX5043_FSKDEV0, 0x43  },
    { REG_AX5043_MODCFGA, 0x05  },
    { REG_AX5043_TXRATE2, 0x00  },
    { REG_AX5043_TXRATE1, 0x03  },
    { REG_AX5043_TXRATE0, 0x06  },
    { REG_AX5043_TXPWRCOEFFB1, 0x0f },
    { REG_AX5043_TXPWRCOEFFB0, 0xff },
    { REG_AX5043_PLLRNGCLK, 0x04  },
    { REG_AX5043_BBTUNE, 0x0f  },
    { REG_AX5043_BBOFFSCAP, 0x77  },
    { REG_AX5043_PKTADDRCFG, 0x00 },
    { REG_AX5043_PKTLENCFG, 0x80  },
    { REG_AX5043_PKTLENOFFSET, 0x00 },
    { REG_AX5043_PKTMAXLEN, 0xc8  },
    { REG_AX5043_MATCH0PAT3, 0xaa },
    { REG_AX5043_MATCH0PAT2, 0xcc },
    { REG_AX5043_MATCH0PAT1, 0xaa },
    { REG_AX5043_MATCH0PAT0, 0xcc },
    { REG_AX5043_MATCH0LEN, 0x1f  },
    { REG_AX5043_MATCH0MAX, 0x1f  },
    { REG_AX5043_MATCH1PAT1, 0x55 },
    { REG_AX5043_MATCH1PAT0, 0x55 },
    { REG_AX5043_MATCH1LEN, 0x8a  },
    { REG_AX5043_MATCH1MAX, 0x0a  },
    { REG_AX5043_TMGRXBOOST, 0x3e   },
    { REG_AX5043_TMGRXSETTLE, 0x31 },
    { REG_AX5043_TMGRXOFFSACQ, 0x7f },
    { REG_AX5043_TMGRXCOARSEAGC, 0x14},
    { REG_AX5043_TMGRXAGC, 0x00  },
    { REG_AX5043_TMGRXRSSI, 0x08  },
    { REG_AX5043_TMGRXPREAMBLE2, 0x35},
    { REG_AX5043_RSSIREFERENCE, 0x19 },
    { REG_AX5043_RSSIABSTHR, 0xc6 },
    { REG_AX5043_BGNDRSSIGAIN, 0x02 },
    { REG_AX5043_BGNDRSSITHR, 0x00 },
    { REG_AX5043_PKTCHUNKSIZE, 0x0d },
    { REG_AX5043_PKTMISCFLAGS, 0x03 },
    { REG_AX5043_PKTACCEPTFLAGS, 0x20},
    { REG_AX5043_DACVALUE1, 0x00  },
    { REG_AX5043_DACVALUE0, 0x00  },
    { REG_AX5043_DACCONFIG, 0x00  },
    { REG_AX5043_0xF10, 0x04   },
    { REG_AX5043_0xF11, 0x00   },
    { REG_AX5043_0xF1C, 0x07   },
    { REG_AX5043_0xF21, 0x5c   },
    { REG_AX5043_0xF22, 0x53   },
    { REG_AX5043_0xF23, 0x76   },
    { REG_AX5043_0xF26, 0x92   },
#if defined(EBYTE_E31_400M17S)
    { REG_AX5043_0xF34, 0x28   },
#elif defined(EBYTE_E31_900M17S)
    { REG_AX5043_0xF34, 0x08   },
#endif
    { REG_AX5043_0xF35, 0x11 },

    { REG_AX5043_AGCGAIN0, 0x83   },
    { REG_AX5043_AGCGAIN1, 0x83   },
    { REG_AX5043_TMGRXOFFSACQ, 0x00   },
    { REG_AX5043_TMGRXCOARSEAGC, 0x73   },
    { REG_AX5043_AGCTARGET0, 0x84  },
    { REG_AX5043_AGCTARGET1, 0x84  },
    { REG_AX5043_AGCTARGET3, 0x84  },
    { REG_AX5043_0xF21, 0x68  },
    { REG_AX5043_0xF22, 0xFF  },
    { REG_AX5043_0xF26, 0x98  },
    { REG_AX5043_0xF23, 0x84  },
    { REG_AX5043_0xF44, 0x25  },
    { 0xFFFF, 0xDDDD  }
};

const uint16e_t E31x_DatarateAddress[42] =
{
    REG_AX5043_IFFREQ1,
    REG_AX5043_IFFREQ0,
    REG_AX5043_DECIMATION,
    REG_AX5043_RXDATARATE2,
    REG_AX5043_RXDATARATE1,
    REG_AX5043_RXDATARATE0,

    REG_AX5043_MAXRFOFFSET2,
    REG_AX5043_MAXRFOFFSET1,
    REG_AX5043_MAXRFOFFSET0,

    REG_AX5043_FSKDMAX1,
    REG_AX5043_FSKDMAX0,

    REG_AX5043_FSKDMIN1,
    REG_AX5043_FSKDMIN0,

    REG_AX5043_AGCGAIN0,
    REG_AX5043_AGCTARGET0,

    REG_AX5043_TIMEGAIN0,
    REG_AX5043_DRGAIN0,

    REG_AX5043_FREQUENCYGAINA0,
    REG_AX5043_FREQUENCYGAINB0,
    REG_AX5043_FREQUENCYGAINC0,
    REG_AX5043_FREQUENCYGAIND0,
    REG_AX5043_AGCGAIN1,
    REG_AX5043_TIMEGAIN1,
    REG_AX5043_DRGAIN1,

    REG_AX5043_FREQUENCYGAINA1,
    REG_AX5043_FREQUENCYGAINB1,
    REG_AX5043_FREQUENCYGAINC1,
    REG_AX5043_FREQUENCYGAIND1,

    REG_AX5043_TIMEGAIN3,
    REG_AX5043_DRGAIN3,

    REG_AX5043_FREQUENCYGAINA3,
    REG_AX5043_FREQUENCYGAINB3,
    REG_AX5043_FREQUENCYGAINC3,
    REG_AX5043_FREQUENCYGAIND3,

    REG_AX5043_FSKDEV2,
    REG_AX5043_FSKDEV1,
    REG_AX5043_FSKDEV0,
    REG_AX5043_MODCFGA,
    REG_AX5043_TXRATE2,
    REG_AX5043_TXRATE1,
    REG_AX5043_TXRATE0,
    REG_AX5043_BBTUNE,
};

const uint16e_t E31x_DatarateValue[42] =
{
    0x00, 0xFC, 0x20, 0x00, 0xa9, 0x45, 0x80, 0x04, 0x49,
    0x03, 0x95, 0xFc, 0x6b, 0xe7, 0x84, 0xaa, 0xa4, 0x0f,
    0x1f, 0x0a, 0x0a, 0xe7, 0xa8, 0xa3, 0x0f, 0x1f, 0x0a,
    0x0a, 0xa7, 0xa2, 0x0f, 0x1f, 0x0d, 0x0d, 0x00, 0x05,
    0x0b, 0x05, 0x00, 0x03, 0x06, 0x0f
};

const uint16e_t E31x_TxRegister[][2] =
{
    {REG_AX5043_PLLLOOP, 0x0b},
    { REG_AX5043_PLLCPI, 0x10    },
#if defined(EBYTE_E31_400M17S)
    { REG_AX5043_PLLVCODIV, 0x24   },
#elif defined(EBYTE_E31_900M17S)
    { REG_AX5043_PLLVCODIV, 0x20   },
#endif
    { REG_AX5043_PLLVCOI, 0x99   },
    { REG_AX5043_XTALCAP, 0x02   },
    { REG_AX5043_0xF00, 0x0f    },
    { REG_AX5043_REF, 0x03    },
    { REG_AX5043_0xF18, 0x06    }
};

const uint16e_t E31x_RxRegister[][2] =
{
    { REG_AX5043_PLLLOOP, 0x0b},
    { REG_AX5043_PLLCPI, 0x10  },
#if defined(EBYTE_E31_400M17S)
    { REG_AX5043_PLLVCODIV, 0x24   },
#elif defined(EBYTE_E31_900M17S)
    { REG_AX5043_PLLVCODIV, 0x20   },
#endif
    { REG_AX5043_PLLVCOI, 0x99  },
    { REG_AX5043_XTALCAP, 0x02  },
    { REG_AX5043_0xF00, 0x0f  },
    { REG_AX5043_REF, 0x03   },
    { REG_AX5043_0xF18, 0x02  }
};

/*!
 * @brief 写单个寄存器
 *
 * @param address 寄存器地址
 * @param data   寄存器值
 * @note 存在两字节/一字节的地址，需要区分进行写入，参考 AX5243手册 SPI Timing
 */
static void E31x_SetRegister( uint16e_t address, uint8e_t data )
{
    /* SPI CS */
    Ebyte_Port_SpiCsIoControl( 0 );
    /* 区分2字节/1字节地址 */
    if( address & 0xFF00 )
    {
        /* 写2字节地址 */
        address |= 0xF000;//第15bit 为R/W位，置1表示后续为写
        Ebyte_Port_SpiTransmitAndReceivce( ( address >> 8 ) );
        Ebyte_Port_SpiTransmitAndReceivce( address );
        /* 写数据 */
        Ebyte_Port_SpiTransmitAndReceivce( data );
    }
    else
    {
        /* 写1字节地址 */
        Ebyte_Port_SpiTransmitAndReceivce( ( ( uint8e_t )( address | 0x0080 ) ) ); //第7bit 为R/W位
        /* 写数据 */
        Ebyte_Port_SpiTransmitAndReceivce( data );
    }
    /* SPI CS */
    Ebyte_Port_SpiCsIoControl( 1 );
}

/*!
 * @brief 读单个寄存器
 *
 * @param address 寄存器地址
 * @return    寄存器值
 * @note 存在2byte/1byte的地址，需要区分，寄存器值都是1byte，参考 AX5243手册 SPI Timing
 */
static uint8e_t E31x_GetRegister( uint16e_t address )
{
    uint8e_t result = 0;
    /* SPI CS */
    Ebyte_Port_SpiCsIoControl( 0 );
    /* 区分2字节/1字节地址 */
    if( address & 0xFF00 )
    {
        /* 写2字节地址 */
        address |= 0x7000;//第15bit 为R/W位，置0表示后续为读 即|0x7000
        Ebyte_Port_SpiTransmitAndReceivce( ( address >> 8 ) );
        Ebyte_Port_SpiTransmitAndReceivce( address );
        /* 读数据 */
        result = Ebyte_Port_SpiTransmitAndReceivce( 0xFF );
    }
    else
    {
        /* 写1字节地址 */
        Ebyte_Port_SpiTransmitAndReceivce( address & 0x7F );//第7bit 为R/W位 即&0x7F
        /* 读数据 */
        result = Ebyte_Port_SpiTransmitAndReceivce( 0xFF );
    }
    /* SPI CS */
    Ebyte_Port_SpiCsIoControl( 1 );
    return result;
}

/*!
 * @brief 将待发送数据写入FIFO
 *
 * @param data 指向发送数据
 * @param size 数据长度
 */
static void E31x_SetFIFO( uint8e_t* data, uint8e_t size )
{
    uint8e_t  i;
    /* SPI CS */
    Ebyte_Port_SpiCsIoControl( 0 );
    /* 指令 FIFODATA 寄存器地址:0x29 bit7为R/W位*/
    Ebyte_Port_SpiTransmitAndReceivce( 0x29 | 0x80 );
    /* 写数据 */
    for( i = 0; i < size; i++ )
    {
        Ebyte_Port_SpiTransmitAndReceivce( *data++ );
    }
    /* SPI CS */
    Ebyte_Port_SpiCsIoControl( 1 );
}

/*!
 * @brief 读取FIFO
 *
 * @param data 指向发送数据
 * @param size 读取数量
 */
static void E31x_GetFIFO( uint8e_t* data, uint8e_t size )
{
    uint8e_t  i;
    /* SPI CS */
    Ebyte_Port_SpiCsIoControl( 0 );
    /* 指令 FIFODATA 寄存器地址:0x29 bit7为R/W位 */
    Ebyte_Port_SpiTransmitAndReceivce( REG_AX5043_FIFODATA & 0x7F );
    /* 读数据 */
    for( i = 0; i < size; i++ )
    {
        *data++ = Ebyte_Port_SpiTransmitAndReceivce( 0xFF );
    }
    /* SPI CS */
    Ebyte_Port_SpiCsIoControl( 1 );
}

/*!
 * @brief 清除FIFO
 */
static void E31x_ClearFIFO( void )
{
    /* FIFOSTAT 寄存器地址:0x28 */
    E31x_SetRegister( 0x28, 0x83 );
}

/*!
 * @brief 判断模块是否存在
 *
 * @return 0：正常  1:不存在
 */
static uint8e_t E31x_IsExist( void )
{
    uint8e_t result;
    /* REVISION ID寄存器 地址:0x00  */
    result = E31x_GetRegister( 0 );
    /* 读取该寄存器必为0x51 */
    if( result != 0x51 )
    {
        return 1;
    }
    return 0;
}

/*!
 * @brief 开启外部晶振
 */
static void E31x_SetXTAL( void )
{
    uint8e_t result;
    /* PWRMODE 寄存器地址:0x02 */
    result = E31x_GetRegister( 0x02 );
    /* 合并数据 */
    result &= 0xF0;
    result |= 0x05;//模式0x05 STANDBY  将会启动晶振
    /* 回写 */
    E31x_SetRegister( 0x02, result );
    /* 获取晶振状态 XTALSTATUS 寄存器地址:0x1D bit0置位表示晶振启动 */
    do
    {
        result = E31x_GetRegister( 0x1D );
    }
    while( ( result & 0x01 ) != 1 );
}

/*!
 * @brief 软复位
 *
 * @return 0:正常 1:异常
 */
uint8e_t  E31x_Reset( void )
{
    /* 复位 RST  PWRMODE 寄存器地址:0x02 bit7*/
    Ebyte_Port_SpiCsIoControl( 1 );
    Ebyte_Port_DelayMs( 1 ); //延时至少100ns 参考手册Power-on-Reset (POR)
    E31x_SetRegister( 0x02, 0x80 );
    E31x_SetRegister( 0x02, 0x00 );
    /* 通信检查 等待通信稳定 SCRATCH 寄存器地址:0x01 */
    do
    {
        E31x_SetRegister( 0x01, 0x55 );
    }
    while( E31x_GetRegister( 0x01 ) != 0x55 );
    /* 引脚检查 PINFUNCIRQ 寄存器地址:0x24 */
    E31x_SetRegister( 0x24, 0x00 ); //IRQ 输出0
    Ebyte_Port_DelayMs( 2 );
    if( Ebyte_Port_IrqIoRead() != 0 )
    {
        return 1;
    }
    E31x_SetRegister( 0x24, 0x01 ); //IRQ 输出1
    Ebyte_Port_DelayMs( 2 );
    if( Ebyte_Port_IrqIoRead() == 0 )
    {
        return 1;
    }
    return 0;
}

static void E31x_SetTxRegisters( void )
{
    uint8e_t i;
    for( i = 0; i < 8 ; i++ )
    {
        E31x_SetRegister( E31x_TxRegister[i][0], E31x_TxRegister[i][1] );
    }
    E31x_SetRegister( REG_AX5043_PLLRANGINGB, E31x_PllTxRange & 0x0F );
    E31x_SetRegister( REG_AX5043_PLLRANGINGA, E31x_PllTxRange & 0x0F );
}

static void E31x_SetRxRegisters( void )
{
    uint8e_t i;
    for( i = 0; i < 8 ; i++ )
    {
        E31x_SetRegister( E31x_RxRegister[i][0], E31x_RxRegister[i][1] );
    }
    E31x_SetRegister( REG_AX5043_PLLRANGINGB, E31x_PllRxRange & 0x0F );
    E31x_SetRegister( REG_AX5043_PLLRANGINGA, E31x_PllRxRange & 0x0F );
}

/*!
 * @brief 设置频率
 */
static void E31x_SetFrequency( uint32e_t freq_hz )
{
    uint32e_t freq = ( uint32e_t )( ( double )( freq_hz / ( double )( XTAL_FREQ ) ) * 1024 * 1024 * 16 );
    E31x_SetRegister( REG_AX5043_FREQB0,  freq & 0xFF );
    E31x_SetRegister( REG_AX5043_FREQB1, ( freq >> 8 ) & 0xFF );
    E31x_SetRegister( REG_AX5043_FREQB2, ( freq >> 16 ) & 0xFF );
    E31x_SetRegister( REG_AX5043_FREQB3, ( freq >> 24 ) & 0xFF );
    E31x_SetRegister( REG_AX5043_FREQA0,  freq & 0xFF );
    E31x_SetRegister( REG_AX5043_FREQA1, ( freq >> 8 ) & 0xFF );
    E31x_SetRegister( REG_AX5043_FREQA2, ( freq >> 16 ) & 0xFF );
    E31x_SetRegister( REG_AX5043_FREQA3, ( freq >> 24 ) & 0xFF );
}

/*!
 * @brief 设置频率并自动修正频率范围
 *
 * @return 0:正常 1:失败
 */
static uint8e_t E31x_SetFrequencyAutoRange( uint32e_t frequency )
{
    uint8e_t pllloop_save, pllcpi_save;
    /* 基准频率 */
    E31x_SetFrequency( frequency );
    /* 切换为RX参数 */
    E31x_SetRxRegisters();
    /* 保护PLL参数 */
    pllloop_save = E31x_GetRegister( REG_AX5043_PLLLOOP );
    pllcpi_save = E31x_GetRegister( REG_AX5043_PLLCPI );
    E31x_SetRegister( REG_AX5043_PLLLOOP, 0x09 );
    E31x_SetRegister( REG_AX5043_PLLCPI, 0x08 );
    /* 自动修正基准值 起始为0x18  这里填充1A加速自动修正过程 */
    E31x_SetRegister( REG_AX5043_PLLRANGINGA, 0x1A );
    /* 等待RNGSTART置位 即自动修正结束  */
    while( E31x_GetRegister( REG_AX5043_PLLRANGINGA ) & 0x10 );
    /* 结果判断 如果RNGERR置位 表示修正失败 */
    E31x_PllRxRange = E31x_GetRegister( REG_AX5043_PLLRANGINGA );
    if( E31x_PllRxRange & 0x20 )
    {
        return 1;
    }
    /* 切换为TX参数 */
    E31x_SetTxRegisters();
    /* 过程和上面一样 */
    E31x_SetRegister( REG_AX5043_PLLLOOP, 0x09 );
    E31x_SetRegister( REG_AX5043_PLLCPI, 0x08 );
    E31x_SetRegister( REG_AX5043_PLLRANGINGA, 0x1A );
    while( E31x_GetRegister( REG_AX5043_PLLRANGINGA ) & 0x10 );
    E31x_PllTxRange = E31x_GetRegister( REG_AX5043_PLLRANGINGA );
    if( E31x_PllTxRange & 0x20 )
    {
        return 1;
    }
    /* 成功 开始回写 */
    E31x_SetRegister( REG_AX5043_PLLLOOP, pllloop_save );
    E31x_SetRegister( REG_AX5043_PLLCPI, pllcpi_save );
    E31x_SetRegister( REG_AX5043_PLLRANGINGB, E31x_PllTxRange & 0x0F );
    E31x_SetRegister( REG_AX5043_PLLRANGINGA, E31x_PllTxRange & 0x0F );
    return 0;
}

/*!
 * @brief 配置模块参数
 *
 * @param frequency     基准载波频率(通信频点)
 * @param output_power  发射功率 0~0x0FFF  默认:最大值0x0FFF即17dBm
 * @param preamble_size 前导码长度
 * @param sync_word     同步字 4byte
 * @param crc           CRC开关
 * @return 0:正常 1:参数不正确
 */
static uint8e_t  E31x_Config( uint32e_t frequency, uint16e_t output_power, uint8e_t preamble_size,  uint32e_t sync_word, uint8e_t crc )
{
    uint16e_t i;
    uint8e_t result = 0;
    /* 写入初始化参数 请参考原厂配置软件 AX-RadioLAB  */
    for( i = 0 ; i < 256; i++ )
    {
        /* 结束符检查 0xFFFF 0xDDDD */
        if( ( E31x_RegisterVauleInit[i][0] == 0xFFFF ) && ( E31x_RegisterVauleInit[i][1] == 0xDDDD ) )
        {
            break;
        }
        /* 写入寄存器 */
        E31x_SetRegister( E31x_RegisterVauleInit[i][0], E31x_RegisterVauleInit[i][1] );
    }
    /* 开启外部晶振 */
    E31x_SetXTAL();
    /* 配置空速 Data Rate  默认1.2Kbps */
    for( i = 0 ; i < 42; i++ )
    {
        E31x_SetRegister( E31x_DatarateAddress[i],  E31x_DatarateValue[i] );
    }
    /* 配置发射功率 TXPWRCOEFFB1 寄存器地址:0x16A  TXPWRCOEFFB0 寄存器地址:0x16B */
    if( output_power > 0x0FFF )
    {
        return 1;    //默认最大值 0x0FFF, 17dBm
    }
    E31x_SetRegister( 0x16A, ( ( uint8e_t )( output_power >> 8 ) ) );
    E31x_SetRegister( 0x16B, ( ( uint8e_t ) output_power ) );
    /* 配置同步字 */
    E31x_SyncWord = sync_word;
    E31x_SetRegister( REG_AX5043_MATCH0PAT3, ( ( uint8e_t )( sync_word >> 24 ) ) );//S3
    E31x_SetRegister( REG_AX5043_MATCH0PAT2, ( ( uint8e_t )( sync_word >> 16 ) ) );//S2
    E31x_SetRegister( REG_AX5043_MATCH0PAT1, ( ( uint8e_t )( sync_word >> 8 ) ) ); //S1
    E31x_SetRegister( REG_AX5043_MATCH0PAT0, ( ( uint8e_t )  sync_word ) );        //S0
    //    E31x_SetRegister( REG_AX5043_MATCH0LEN, 0x1F );//修改同步字长度 默认4字节0x1F  如果2字节可以设置为0x0F
    /* 记录前导码长度 发送时会用到 */
    E31x_PreambleSize = preamble_size;
    /* 记录CRC 发送时会用到
       注意:芯片存在特性,开或没开CRC 接收方都能收到数据  */
    E31x_CrcOnOff = crc;
    /* 配置频率并进行自动修正 可能会失败！ */
    result = E31x_SetFrequencyAutoRange( frequency );
    if( result != 0 )
    {
        return 1;
    }
    return result;
}

/*!
 * @brief 开始监听数据
 *
 * @return 0:正常 1:异常
 */
uint8e_t E31x_GoReceive( void )
{
    uint8e_t result = 0;
    /* 模式切换: POWERDOWN */
    E31x_SetRegister( 0x02, 0x00 );
    /* 切换接收寄存器参数 */
    E31x_SetRxRegisters();
    /* 开启外部晶振 */
    E31x_SetXTAL();
    /* 开启FIFO */
    E31x_SetRegister( REG_AX5043_PWRMODE, AX5043_PWRSTATE_FIFO_ON );
    /* 添加RSSI偏移计算常量 */
    E31x_SetRegister( REG_AX5043_RSSIREFERENCE, 0x19 );
    /* 关闭前导码超时计算 */
    E31x_SetRegister( REG_AX5043_TMGRXPREAMBLE1, 0x00 );
    /* CRC检查完毕后丢弃CRC所占字节 */
    result = E31x_GetRegister( REG_AX5043_PKTSTOREFLAGS ) & ( ~0x40 );
    E31x_SetRegister( REG_AX5043_PKTSTOREFLAGS, result );
    /* FIFO: 打开中断 */
    uint16e_t fifoIRQ = 0x0001;//FIFO非空中断
    E31x_SetRegister( REG_AX5043_IRQMASK0, ( ( uint8e_t )fifoIRQ ) );
    E31x_SetRegister( REG_AX5043_IRQMASK1, ( ( uint8e_t )( fifoIRQ >> 8 ) ) );
    /* FIFO: 清除数据 */
    E31x_ClearFIFO();
    /* 模式切换: FULLRX  即开始接收 */
    E31x_SetRegister( REG_AX5043_PWRMODE, 0x09 );
    /* 状态记录 */
    E31x_Status = GO_WAIT_RECEIVE;    
    return result;
}

/*!
 * @brief 开始发送数据
 *
 * @param data 指向发送数据
 * @param size 发送数据长度
 */
uint8e_t E31x_GoTransmit( uint8e_t* data, uint8e_t size )
{
    uint8e_t result;
    /* 模式切换: POWERDOWN */
    E31x_SetRegister( 0x02, 0x00 );
    /* 切换TX参数 */
    E31x_SetTxRegisters();
    /* 打开外部晶振 */
    E31x_SetXTAL();
    /* 关闭中断 */
    E31x_SetRegister( REG_AX5043_IRQMASK0, 0x00 );
    E31x_SetRegister( REG_AX5043_0xF08, 0x07 ); //莫得说明 暂时保留
    /* 打开FIFO */
    E31x_SetRegister( REG_AX5043_PWRMODE, AX5043_PWRSTATE_FIFO_ON );
    /* FIFO:设置 阈值/临界点  */
    E31x_SetRegister( REG_AX5043_FIFOTHRESH1, 0x00 );
    E31x_SetRegister( REG_AX5043_FIFOTHRESH0, 0x80 );
    /* FIFO: 清除数据 */
    E31x_ClearFIFO();
    /* FIFO 数据包上限 240字节 */
    E31x_SetRegister( REG_AX5043_PKTCHUNKSIZE, 0x0D );
    E31x_SetRegister( REG_AX5043_IRQMASK1, 0x00 );
    /* 数据帧:前导码  参考AN9347 章节REPEATDATA COMMAND */
    E31x_SetRegister( REG_AX5043_FIFODATA, AX5043_FIFOCMD_REPEATDATA | ( 3 << 5 ) ); //帧头 类型REPEATDATA 0x62
    E31x_SetRegister( REG_AX5043_FIFODATA, 0x38 ); //标识符 0x38 单帧无CRC
    E31x_SetRegister( REG_AX5043_FIFODATA, E31x_PreambleSize ); //重复次数 即前导码长度
    E31x_SetRegister( REG_AX5043_FIFODATA, 0x55 ); //前导码内容 实际上是0xAA 先发低bit(LSB)
    /* 数据帧:同步字  参考 TRANSMIT DATA FORMAT 同步字匹配后会被丢弃 */
    E31x_SetRegister( REG_AX5043_FIFODATA, 0xA1 ); //帧头 A代表长度为5 类型为1 普通数据
    E31x_SetRegister( REG_AX5043_FIFODATA, 0x18 ); //标识符 无CRC
    E31x_SetRegister( REG_AX5043_FIFODATA, ( ( uint8e_t )  E31x_SyncWord ) );         //S0
    E31x_SetRegister( REG_AX5043_FIFODATA, ( ( uint8e_t )( E31x_SyncWord >> 8 ) ) );  //S1
    E31x_SetRegister( REG_AX5043_FIFODATA, ( ( uint8e_t )( E31x_SyncWord >> 16 ) ) ); //S2
    E31x_SetRegister( REG_AX5043_FIFODATA, ( ( uint8e_t )( E31x_SyncWord >> 24 ) ) ); //S3
    /* 数据帧:描述部分 参考 TRANSMIT DATA FORMAT */
    E31x_SetRegister( REG_AX5043_FIFODATA, 0xE1 ); //帧头  E代表可变数据包 1代表普通数据
    E31x_SetRegister( REG_AX5043_FIFODATA, size + 2 ); //本次数据包长度 后续加了标识符和一位长度位 导致长度+2
    if( E31x_CrcOnOff )//CRC控制
    {
        E31x_SetRegister( REG_AX5043_FIFODATA, 0x03 ); //标识符 0x03 单帧 有CRC
    }
    else
    {
        E31x_SetRegister( REG_AX5043_FIFODATA, 0x0B ); //标识符 0x0B 单帧 无CRC
    }
    E31x_SetRegister( REG_AX5043_FIFODATA, size + 1 ); //DATA  我觉得可以叫做块长度位
    /* 数据帧:真实数据 */
    E31x_SetFIFO( data, size );
    /* 允许发送/接收完成事件 REVMDONE bit0 */
    E31x_SetRegister( REG_AX5043_RADIOEVENTMASK0, 0x01 );
    /* 允许由IC自动硬件使能中断 IRQMRADIOCTRL */
    E31x_SetRegister( REG_AX5043_IRQMASK0, 0x40 );
    /* FIFO: Commit 提交 数据刷新 */
    E31x_SetRegister( REG_AX5043_FIFOSTAT, 0x04 );
    /* 模式切换: FULLTX模式 即开始发送 */
    E31x_SetRegister( 0x02, 0x0D );
    /* 状态记录 */
    E31x_Status = GO_WAIT_TRANSMIT;   
    /* 阻塞等待发送完成 */
    do
    {
        result = E31x_GetRegister( REG_AX5043_RADIOSTATE );
    }
    while( result != 0 );//正常发送完成后 状态为idle 即为0
    /* 状态记录 */
    E31x_Status = GO_STBY;     
    /* 回调用户函数 */
    Ebyte_Port_TransmitCallback( 0x0001 );
    E31x_SetRegister( REG_AX5043_0xF08, 0x04 ); //莫得说明 暂时保留
    return 0 ;
}

/*!
 * @brief 进入休眠模式
 */
uint8e_t E31x_GoSleep( void )
{
    /* (可选)关闭外部晶振 如果是由外部单片机控制的TXCO引脚 那么可以去掉这个 */
    E31x_SetRegister( REG_AX5043_PINFUNCPWRAMP, 0x00 );
    /* 进入POWERDOWN模式 除了寄存器其他所有模块都关闭 控制寄存器的内容不会丢失
       芯片还提供深度休眠模式 0x01 但所有数据将丢失 请酌情考虑 */
    E31x_SetRegister( 0x02, 0x00 ) ;
    /* 状态记录 */
    E31x_Status = GO_SLEEP;    
    return 0;
}


/*!
 * @brief 模块初始化
 *
 * @return 0:正常 1:初始化失败
 */
uint8e_t E31x_Init( void )
{
    uint8e_t result = 0;
    /* 判断模块是否存在 */
    result = E31x_IsExist();
    if( result != 0 )
    {
        return 1;
    }
    /* 复位 */
    result = E31x_Reset();
    if( result != 0 )
    {
        return 1;
    }
    /* 参数配置 */
    result = E31x_Config( E31X_FREQUENCY_START,
                          E31X_OUTPUT_POWER,   
                          E31X_PREAMBLE_SIZE,  
                          E31X_SYNC_WORD,
                          E31X_IS_CRC );
    if( result != 0 )
    {
        return 1;
    }
    /* 进入接收模式 */
    E31x_GoReceive();
    return result;
}

/*!
 * @brief 周期调用函数 辅助完成无线数据接收
 *
 * @return 0
 * @note 需要被主函数循环调用，因为通过轮询内部标识位来判断是否有数据到达
 *       如若考虑中断，在中断中调用本函数即可
 */
uint8e_t E31x_TaskForPoll( void )
{
    uint8e_t statusIRQ ;
    /* 获取状态信息 */
    statusIRQ = E31x_GetRegister( REG_AX5043_IRQREQUEST0 );
    /* 0x01：表示FIFO中接收到数据 */
    if( statusIRQ & 0x01 )
    {
        /* 正常来说，FIFO数据第一字节表示类型与长度
           数据包格式可以参考手册AND9347章节 FIFO Chunk Encoding  */
        uint8e_t chunkHeader = 0;
        E31x_GetFIFO( &chunkHeader, 1 );
        /* 包头中的bit7:bit5表示长度  bit4:bit0表示类型  */
        uint8e_t chunkSize = ( chunkHeader >> 5 );
        uint8e_t chunkType = ( chunkHeader & 0x1F );
        /* 当长度为最大值7时 表示本次为可变长数据包 需要自己处理长度问题 */
        if( chunkSize == 7 )
        {
            E31x_GetFIFO( &chunkSize, 1 );//读取长度
        }
        /* 根据类型进行解包 */
        switch( chunkType )
        {
            /* 0x01:普通数据包 DATA */
            case 0x01 :
                if( chunkSize != 0 )
                {
                    uint8e_t chunkFlag = 0;
                    E31x_GetFIFO( &chunkFlag, 1 );//读取标识符 请自行处理
                    uint8e_t dataSize = 0;
                    E31x_GetFIFO( &dataSize, 1 ); //读取数据块长度
                    dataSize -= 1;                //去掉长度位占的1位 即-1
                    E31x_GetFIFO( E31x_RxBuffer, dataSize );//读取有效内容
                    E31x_Status = GO_STBY;//状态记录                    
                    Ebyte_Port_ReceiveCallback( 0x0001, E31x_RxBuffer, dataSize ); //回调用户函数
                }
                break;
            /* 0x11:RSSI */
            case 0x11 :
                break;
            default:
                break;
        }
    }
    return 0;
}

/*!
 * @brief 保留
 */
void E31x_TaskForIRQ( void )
{
}

/*!
 * @brief 获取模块名字
 *
 * @return 指向名字字符串的指针
 * @note 标准字符串 末尾含有结束符 '\0'
 */
uint8e_t* E31x_GetName( void )
{
    return E31x_NameString;
}

/*!
 * @brief 获取模块程序版本
 *
 * @return 8位的编码
 * @note 例如0x10 代表V1.0
 */
uint8e_t E31x_GetDriverVersion( void )
{
    return  EBYTE_E31_PROGRAM_TYPE;
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
uint8e_t E31x_GetStatus( void )
{
    return (uint8e_t)E31x_Status;
}