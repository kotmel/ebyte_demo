#include "board.h"
#include "ebyte_e07x.h"

extern volatile int received;

volatile int irq_counter = 0;

// / Auxiliary identification module
#if defined(EBYTE_E07_400M10S)
static uint8e_t E07x_NameString[] = "E07-400M10S";
#elif defined(EBYTE_E07_900M10S)
static uint8e_t E07x_NameString[] = "E07-900M10S";
#endif 

// / Auxiliary identification driver version number
#define EBYTE_E07_TYPE_PROGRAM   0x10 

#define EBYTE_E07X_XOSC         26000000  // 26MHz crystal oscillator
#define EBYTE_E07X_POW_2_28     268435456 // 2 to the 28th power
#define EBYTE_E07X_POW_2_17     131072
#define EBYTE_E07X_CAL_BASE     ((double)EBYTE_E07X_POW_2_28) / ((double)EBYTE_E07X_XOSC)

/// Specify the internal FIFO
#define  FIFO_READ           0x00
#define  FIFO_WRITE          0x01   

// / Specify the interrupt type
#define IRQ_RX_SYNC_RECV 0x06  
#define IRQ_TX_SYNC_SEND 0x06  
#define IRQ_RX_CRC_OK    0x07

/// Output power meter auxiliary calculation
static uint8e_t E07x_PaTabel[] = { 0xC0, 0, 0, 0, 0, 0, 0, 0};

/// Receive buffer
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

// / status flag
static E07x_Status_t E07x_Status = GO_INIT;

/* Basic configuration parameters
* from configuration software TI SmartRF Studio 7
* Version 2.21.0 */
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


/* !
 * @brief set internal register value
 *
 * @param address register address
 * @param data write value
 */
static void E07x_SetRegister( uint8e_t address, uint8e_t data )
{
    /* SPI CS */
    Ebyte_Port_SpiCsIoControl(0);
    
    /* write address */
    Ebyte_Port_SpiTransmitAndReceivce( address );
    
    /* write value */
    Ebyte_Port_SpiTransmitAndReceivce( data );
    
    /* SPI CS */
    Ebyte_Port_SpiCsIoControl(1);    
}

/* !
 * @brief read internal register value
 *
 * @param address register address
 * @return register value
 */
static uint8e_t E07x_GetRegister( uint8e_t address )
{
    uint8e_t result;
    
 
    
    /* SPI CS */
    Ebyte_Port_SpiCsIoControl(0);
    
    /* Register read instructions after address 0x30 need to be changed */
    if( address < 0x30 )
    {
        /* Write address bit7 is set to 1, which means that it needs to be read later, that is |0x80 */
        Ebyte_Port_SpiTransmitAndReceivce( address | 0x80);    
    }
    else
    {
         /* write address */
        Ebyte_Port_SpiTransmitAndReceivce( address | 0xC0);         
    }

    /* read 1Byte */
    result = Ebyte_Port_SpiTransmitAndReceivce( 0xFF );
    
    /* SPI CS */
    Ebyte_Port_SpiCsIoControl(1);  
    
    return result;
}

/* !
 * @brief Set internal register values in batches
 *
 * @param address register address
 * @param data points to the first address of writing data
 * @param size write length
 */
static void E07x_SetRegisters( uint8e_t address , uint8e_t *data , uint8e_t size )
{
    uint8e_t i = size;
    
    /* SPI CS */
    Ebyte_Port_SpiCsIoControl(0);
    
    /* Write start address address bit6 is set to represent continuous write instructions, ie |0x40 */
    Ebyte_Port_SpiTransmitAndReceivce( address | 0x40 );
    
    /* write data */
    while( i-- )
    {
        Ebyte_Port_SpiTransmitAndReceivce( *data++ );
    }
    
    /* SPI CS */
    Ebyte_Port_SpiCsIoControl(1);
}

/* !
 * @brief write command
 *
 * @param address register address
 */
static void E07x_SendCommand( uint8e_t command )
{
    /* SPI CS */
    Ebyte_Port_SpiCsIoControl(0);
    
    /* write command */
    Ebyte_Port_SpiTransmitAndReceivce( command );
    
    /* SPI CS */
    Ebyte_Port_SpiCsIoControl(1);
}

/* !
 * @brief Write data to be sent to FIFO
 *
 * @param data points to send data
 * @param size write length
 */
static void E07x_SetFIFO( uint8e_t *data, uint8e_t size)
{
    uint8e_t i = size;
    
    /* SPI CS */
    Ebyte_Port_SpiCsIoControl(0);
    
    /* FIFO address: 0x3F Address bit6 is set to represent continuous write instructions, ie |0x40 */
    Ebyte_Port_SpiTransmitAndReceivce( 0x3F | 0x40 );
    
    /* write data */
    while( i-- )
    {
        Ebyte_Port_SpiTransmitAndReceivce( *data++ );
    }
    
    /* SPI CS */
    Ebyte_Port_SpiCsIoControl(1);
}

/* !
 * @brief read data to FIFO
 *
 * @param data points to cache
 * @param size read length
 */
static void E07x_GetFIFO( uint8e_t *data, uint8e_t size)
{
    uint8e_t i = size;
    
    /* SPI CS */
    Ebyte_Port_SpiCsIoControl(0);
    
    /* FIFO address: 0x3F Address bit6 is set to represent continuous read instructions, namely |0x40 */
    Ebyte_Port_SpiTransmitAndReceivce( 0x3F | 0xC0 );
    
    /* write data */
    while( i-- )
    {
        *data++ = Ebyte_Port_SpiTransmitAndReceivce( 0xFF );
    }
    
    /* SPI CS */
    Ebyte_Port_SpiCsIoControl(1);
}


/* !
 * @brief clear FIFO content
 * @param mode mode
 * @arg FIFO_WRITE: specifies the sending FIFO
 * @arg FIFO_READ: Specifies the receive FIFO
 */
static void E07x_ClearFIFO( uint8e_t mode )
{
  
    if( mode == FIFO_WRITE )
    {    
        /* Command: clear send FIFO(0x3B) */
        E07x_SendCommand(0x3B);
    }
    else
    {
        /* Command: clear receive FIFO(0x3A) */
        E07x_SendCommand(0x3A);
    } 
    
}

/*!
 * @brief soft reset
 */
void E07x_Reset(void)
{
    /* SPI CS */
    Ebyte_Port_SpiCsIoControl(1);
    Ebyte_Port_SpiCsIoControl(0);
    Ebyte_Port_DelayMs(1);
    Ebyte_Port_SpiCsIoControl(1);
    Ebyte_Port_DelayMs(1);
    
    /* Command: chip reset (0x30) */
    E07x_SendCommand( 0x30 );
    
    Ebyte_Port_DelayMs(20);
}

/*!
 * @brief toggle into standby mode
 */
static void E07x_SetStby(void)
{
    /* Command: 0x36 Exit TX/RX mode and enter idle state */
    E07x_SendCommand(0x36);
    
/* Status record */
    E07x_Status = GO_STBY;
}

/* !
 * @brief switch to sleep mode
 *
 * @note Please refer to the manual, the parameters of some registers will be lost!
 */
static void E07x_SetSleep(void)
{
    /* Command: 0x39 enter sleep mode */
    E07x_SendCommand(0x39);
    
    /* Status record */
    E07x_Status = GO_SLEEP;
}

static void E07x_SetGPIO( uint8e_t gpio , uint8e_t config )
{
  
  /* Used to specify the polarity of the IO when there is no interrupt, it is high level when triggering an interrupt, and it is low level */
  uint8e_t mask = 0x40;
  
  /* merge interrupt type */
  mask |= (config & 0x3F);
  
  switch ( gpio )
  {
    /* IOCFG0 register address: 0x02 */
    case 0:  E07x_SetRegister(0x02,mask) ;break;
    /* IOCFG1 register address: 0x01 */
    case 1:  E07x_SetRegister(0x01,mask) ;break;
    /* IOCFG2 register address: 0x00 */
    case 2:  E07x_SetRegister(0x00,mask) ;break;
    default: break;
  }
}

/* !
 * @brief Determine whether the module exists
 *
 * @return 0: normal 1: module exception
 */
static uint8e_t E07x_IsExist(void)
{
    uint8e_t result = 0;
    uint8e_t reg_value;
    
    /* Read temperature sensor configuration parameters should be 0x7F after reset. Register address: 0x2A */
    reg_value = E07x_GetRegister(0x2A);
    
    /* The default value must be 0x7F */
    if( reg_value != 0x7F ) 
    {
       /* can not read the correct data, please check the hardware */
       result = 1;
    }
    
    return result;
}

/* !
 * @brief modulation method
 *
 * @param mod desired modulation
 * @arg 0:2-FSK
 * @arg 1:GFSK
 * @arg 3:ASK/OOK
 * @arg 4:4-FSK
 * @arg 7:MSK
 */
static void E07x_SetModulation(uint8e_t mod)
{
/* convert bit */
    uint8e_t tmp = (mod<<4) & 0x70;
  
    /* MDMCFG2 register address 0x12 clears bit[6:4] */
    E07x_InitSetting.mdmcfg2  &= 0x8F;
    
    /* Set bit[6:4] */
    E07x_InitSetting.mdmcfg2 |= tmp;
}
/* !
 * @brief Free carrier frequency reference value calculation
 *
 * @param frequency expected carrier frequency Hz
 * @return 0: normal 1: parameter error
 * @note must be between the following frequency bands ( Hz ):
 * 300000000 - 348000000
 * 387000000 - 464000000
 * 779000000 - 928000000
 *! ! ! Note: The decimal part of the calculation results of processors with different architectures may be inconsistent, and there may be deviations
 *
 * Spectrum analyzer test frequency point OK. The actual measurement can communicate with the module of the official calculator parameter configuration frequency.
 */
static uint8e_t E07x_SetFrequency( uint32e_t frequency )
{ 
   uint32e_t freq ;
   uint8e_t  freq2,freq1,freq0;
   
   /* parameter check */
   if(  frequency > 928000000 ) return 1;
   if( (frequency < 779000000) && ( frequency>464000000) )return 1;
   if( (frequency < 387000000) && ( frequency>348000000) )return 1;
   if( (frequency < 300000000)) return 1;
   
   /* Frequency controller word (FREQ)=Carrier frequency*(2^16)/Crystal oscillator frequency */
   freq = (uint32e_t)(((double)frequency * 65535 )/ ( (double)EBYTE_E07X_XOSC ));
   freq2 = ( uint8e_t )( ( freq >> 16 ) & 0xFF );
   freq1 = ( uint8e_t )( ( freq >> 8 ) & 0xFF );
   freq0 = ( uint8e_t )(  freq  & 0xFF );
   
   /* Fill the frequency part in the initialization parameter array and wait for subsequent unified writing */
   E07x_InitSetting.freq2 = freq2;
   E07x_InitSetting.freq1 = freq1;
   E07x_InitSetting.freq0 = freq0;
   
   return 0;
}

/* !
* @brief free data rate baseline calculation
*
* @param datarate expected data rate
* @note recommends between the following rates:
* 300 (0.3K) - 250000000 (250K)
 */
static void E07x_SetDataRate( uint32e_t datarate )
{
    uint8e_t  date_m ;
    uint16e_t date_e ;
    uint32e_t date_e_sum = 1,temp;
    
    /* Intermediate formula conversion (256+DRATE_M)*2^DRATE_E=datarate*2^28/crystal oscillator frequency */
    uint32e_t calculation = (uint32e_t)( datarate * EBYTE_E07X_CAL_BASE);
    
    /* Successive approximation matches DRATE_E */
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
       
       temp = calculation/date_e_sum; // The result must be between [256,256+DRATE_M]
       if(  temp>=256 && temp<=511 )
       {
          date_m = temp - 256;// get DRATE_M
          break;
       }
    }
    
    /* Fill the data rate part in the initialization parameter array and wait for subsequent unified writing */
    if ( date_e<256 )
    {
       E07x_InitSetting.mdmcfg4 &= 0xF0;
       E07x_InitSetting.mdmcfg4 |= date_e;
       E07x_InitSetting.mdmcfg3 = date_m;    
    }
    
}

/* !
 * @brief free bandwidth benchmark calculation
 *
 * @param bandwidth Expected receiving bandwidth (Hz)
 * @note The measured result is consistent with the official calculator, the default minimum is 58.035714
 * Note: The chip can actually only be configured to fix a few bandwidths, and the input value will be converted upwards to close to the following fixed bandwidths:
 * 58.035714 KHz
 * 67.708333 KHz
 * 81.250000 KHz
 * 101.562500 KHz
 * 116.071429 KHz
 * 135.416667 KHz
 * 162.500000 KHz
 * 203.125000 KHz
 * 232.142857 KHz
 * 270.833333 KHz
 * 325.000000 KHz
 * 406.250000 KHz
 * 464.285714 KHz
 * 541.666667 KHz
 * 650.000000 KHz
 * 812.500000 KHz
 */
static void E07x_SetChannelBandwidth( uint32e_t bandwidth )
{
    uint8e_t  chanbw_m ,chanbw_e,mask;

    uint32e_t chanbw_e_sum = 1,temp;
    
    /* Intermediate formula conversion (4+CHANBW_M)*2^CHANBW_E=crystal frequency/(8*bandwidth) */
    uint32e_t calculation = (uint32e_t)( EBYTE_E07X_XOSC/(8 * bandwidth));    
    
    /* Successive approximation match CHANBW_E */
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
          chanbw_m = temp - 4;// get CHANBW_M
          break;
       }
    }    
    
    /* Fill the bandwidth part in the initialization parameter array and wait for subsequent unified writing */
    if( chanbw_e<=3 )
    {
        mask = ((uint8e_t)((chanbw_e<<6)|(chanbw_m<<4))) & 0xF0;
        E07x_InitSetting.mdmcfg4 &= 0x0F;
        E07x_InitSetting.mdmcfg4 |= mask;
    }
    
}

/* !
 * @brief Calculate the reference value of frequency offset
 *
 * @param frequency_dev unit: Hz
 *
 * @note The measured results are consistent with the official calculation results
 */
static void E07x_SetFrequencyDeviation( uint32e_t frequency_dev )
{
    uint8e_t  deviation_m,deviation_e,mask;
    uint32e_t deviation_e_sum = 1,temp;
    
    /* Intermediate formula conversion (8+DEVIATION_M)*2^DEVIATION_E=frequency_dev * 2^17 / crystal oscillator frequency */
    uint32e_t calculation = (uint32e_t)( frequency_dev * EBYTE_E07X_POW_2_17 / EBYTE_E07X_XOSC );   
    
    /* Successive approximation matches DEVIATION_E */
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
        
       temp = calculation/deviation_e_sum; //get  8+DEVIATION_M
       
       if(  temp>=8 && temp<=15 )
       {
          deviation_m = temp - 8;// get CHANBW_M
          break;
       }
    }      
    
    /* Fill the frequency offset part in the initialization parameter array and wait for subsequent unified writing */
    if( deviation_e<=7 )
    {
        mask = ((uint8e_t)((deviation_e<<4)|deviation_m)) & 0x77;
        E07x_InitSetting.deviatn &= 0x88;
        E07x_InitSetting.deviatn |= mask;
    }    
}

/* !
 * @brief set the output power
 *
 * @param power unit dBm Note: there is a limit to the input value
 *
 * @return 0: Normal 1: The parameter is incorrect
 * @note dropped PA Ramping, perhaps losing some linearity and integrity? Increased spurious signals?
 * The power output is not linear. The PA value set in the function comes from TI SmartRF Studio and is for reference only
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
    
    if( E07_FREQUENCY_START > 900000000 ) //900MHz to 928MHz
    {
        switch ( power ) 
        {
            case 11:  E07x_PaTabel[0] = 0xC0;break; // The power of this frequency range can be increased to 11dBm
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
    else //779MHz to 900MHz
    {
        switch ( power ) 
        {
            case 12:  E07x_PaTabel[0] = 0xC0;break; // The power of this frequency range can be increased to 12dBm
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

/* !
 * @brief module basic parameter configuration
 *
 * @param frequency carrier frequency
 * @param data_rate symbol rate (air rate)
 * @param frequency_dev modulation frequency deviation
 * @param bandwidth Receive bandwidth
 * @param output_power output power
 * @param preamble_size send preamble length
 * @param sync_word sync word
 * @param crc CRC switch
 * @param device_address device address
 *
 * @return 0: normal 1: parameter error
 * @note (optional) Address checking is turned off by default
 */
static uint8e_t E07x_Config( uint32e_t frequency , uint32e_t data_rate , uint32e_t frequency_dev ,uint32e_t bandwidth, 
                              int8e_t output_power , uint16e_t preamble_size, uint16e_t sync_word , uint8e_t crc )
{
    uint8e_t result ;  
    uint8e_t reg_value;
    
    /* Calculation: carrier frequency
    * Register start address 0x0D */
    result = E07x_SetFrequency( frequency );
    if( result != 0 ) return 1;
    
    /* Calculation: Airspeed
    * MDMCFG4 MDMCFG3 register address: 0x10 0x11 */
    E07x_SetDataRate( data_rate );
    
    /* Calculation: frequency offset
    * DEVIATN register address: 0x15 */
    E07x_SetFrequencyDeviation( frequency_dev );    
    
    /* Calculation: receive bandwidth
    * MDMCFG4 register address: 0x10 */
    E07x_SetChannelBandwidth( bandwidth );
    
    /* Calculation: output power
    * Register address: 0x3E */
    E07x_SetOutputPower(output_power);
    
    /* modulation mode
    * MDMCFG2 register address: 0x12 */
    E07x_SetModulation(1);//GFSK
    
     /* preamble length
    * MDMCFG1 register address: 0x13 */
    if( preamble_size >7 ) return 1;// parameter check
    E07x_InitSetting.mdmcfg1 &= 0x8F;// clear
    E07x_InitSetting.mdmcfg1 |= (preamble_size<<4); // set
    
    /* sync word
    * SYNC1 SYNC0 register address: 0x04 0x05 */
    E07x_InitSetting.sync1 =  (uint8e_t)((sync_word>>8)&0xFF);//High Byte
    E07x_InitSetting.sync0 =  (uint8e_t)(sync_word & 0xFF);   //Low Byte
    
    /* CRC switch
    * PKTCTRL0 register address: 0x08 bit2 */

    if( crc > 1 ) return 1;// parameter check can only be 0 or 1
    if( crc )
    {
        E07x_InitSetting.pktctrl0 |= 0x04; // Open
    }else
    {
        E07x_InitSetting.pktctrl0 &= 0xFB;
    }
    
    /* The length of the data packet defaults to variable length mode and the length is placed in the first byte of the data
    * PKTCTRL0 register address: 0x08 bit[1:0] */
    E07x_InitSetting.pktctrl0 &= 0xFC;// clear
    E07x_InitSetting.pktctrl0 |= 0x01;// 0x01 mode (variable length)
    
    /* Precautions: If you want to fully use the configuration parameters of the official SmartRF Studio, then just comment out the modification function above */
     /* Write the calculated E07x_InitSetting parameter table to the module */
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
//    E07x_SetRegister( 0x2E, E07x_InitSetting.test0); // Found: Setting TEST0 leads to custom calculation frequency offset
                                                        // Phenomenon: When the frequency point is set to less than 411MHz, the frequency point is locked at 411MHz. When the frequency point is set to be greater than 411MHz, the frequency point switching is normal
                                                        // Suggestion: Uncomment TEST0 when the official SmartRF Studio parameters are fully enabled. When evaluating with free parameters, keep annotations

   /* Supplement: channel code (can be used for frequency hopping) register address: 0x0A */
    E07x_SetRegister( 0x0A ,0 );
    
    /* Supplement: Disable address filter register address: 0x07 */
    reg_value = E07x_GetRegister(0x07);
    reg_value &= 0xFC;// clear
    E07x_SetRegister(0x07,reg_value);
    
    /* Supplement: configure output power register address: 0x3E */
    E07x_SetRegisters( 0x3E ,E07x_PaTabel,8 );
    
    return 0;
}

/* !
 * @brief module initialization
 *
 * @return 0: normal 1: initialization failed
 */
uint8e_t E07x_Init( void )
{
    uint8e_t result = 0;
    
    /* soft reset */
    E07x_Reset();
    
    /* Check if the module exists */
    result = E07x_IsExist();
    if( result != 0 ) return 1;
    
    /* Basic parameter setting */
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
    
    /* Enter receiving mode by default */
    E07x_GoReceive();
    return 0;
}

/*!
 * @brief kkk - IRQ handling implementation - use with caution !
 */
void E07x_TaskForIRQ(void)
{

    received = 1;
    irq_counter++;
#if HANDLE_RECEIVE_IN_IRQ
    E07x_TaskForPoll();
#endif
}

/* !
 * @brief polling function assists in completing data reception
 *
 * @param data points to send data
 * @param size data length
 * @return 0
 */
uint8e_t E07x_GoTransmit( uint8e_t *data, uint8e_t size )
{
    uint8e_t irqStatus = 0;
    // disable receive IRQ
    BSP_GPIO_PORT_E07_GDO0->CR2 &= (uint8_t)(~(BSP_GPIO_PIN_E07_GDO0));

    /* Mode switching: standby */
    E07x_SetStby();
    
    /* Clear send data queue */
    E07x_ClearFIFO( FIFO_WRITE );
   
    /* Send data is variable length modulus, the first byte first writes the length */
    E07x_SetFIFO( &size , 1 );
    
    /* Subsequent write complete data packet */
    E07x_SetFIFO( data , size);
    
    /* Set pin GPIO0 to map to interrupt
    * Trigger an interrupt when the sync word is sent! ! ! Note: Automatically when the data in the FIFO is completely sent! clear interrupt */
     E07x_SetGPIO(0 ,IRQ_TX_SYNC_SEND );

    /* Send command: 0x35 module enters receiving mode */
    E07x_SendCommand( 0x35 );      
    
    /* Status record */
    E07x_Status = GO_WAIT_TRANSMIT;    
    
    /* �������ͷ�ʽ �ڴ˵ȴ�������� */  
    do
    {
      Ebyte_Port_DelayMs(1);
      
      /* Read status register MARCSTATE address: 0x35 */
      irqStatus = E07x_GetRegister(0x35);

    }
    while( (irqStatus != 0x01) );// Automatically return to the idle state after sending is completed, which is 0x01
                                  // If the register MCSM1 address is set: 0x17 and TXOFF_MODE is turned on, then the status after sending is not necessarily idle and needs to be processed separately
    
    /* Status record */
    E07x_Status = GO_STBY;  
    
    /* callback user function */
    Ebyte_Port_TransmitCallback( 0x0001 );
    
    return 0;
}

/* !
 * @brief start listening data
 *
 * @return 0
 */
 uint8e_t E07x_GoReceive( void )
{
  
    /* Mode switching: standby */
    E07x_SetStby();
    
    /* Clear the receive data queue */
    E07x_ClearFIFO( FIFO_READ );  
  
    /* Set pin GPIO0 to map to interrupt */
//    E07x_SetGPIO(0 ,IRQ_RX_CRC_OK );
     E07x_SetGPIO(0 ,IRQ_RX_SYNC_RECV );
  
    /* Send command: 0x34 module enters receiving mode */
    E07x_SendCommand( 0x34 );  
    
    /* Status record */
    E07x_Status = GO_WAIT_RECEIVE;
    // kkk enable receive IRQ
    BSP_GPIO_PORT_E07_GDO0->CR2 |= (uint8_t)(BSP_GPIO_PIN_E07_GDO0);
    return 0;
}

/* !
 * @brief module enters sleep mode (low power consumption)
 *
 * @return 0;
 */
uint8e_t E07x_GoSleep(void)
{
    E07x_SetSleep();
    
    return 0;
}

/* !
 * @brief get module program version
 *
 * @return 8-bit encoding
 * @note For example, 0x10 represents V1.0
 */
uint8e_t E07x_GetDriverVersion(void)
{
  return  EBYTE_E07_TYPE_PROGRAM;
}

/* !
 * @brief Get the module name
 *
 * @return pointer to name string
 * @note The end of the standard string contains the terminator '\0'
 */
uint8e_t* E07x_GetName(void)
{
    return E07x_NameString;
}

/* !
 * @brief get module status
 *
 * @return status code
 * Uninitialized GO_INIT =0x00
 * Switching state GO_BUSY =0x01
 * Standby/idle GO_STBY =0x02
 * Ready to receive environment GO_RECEIVE =0x03
 * Listening to data GO_WAIT_RECEIVE =0x04
 * Ready to send environment GO_TRANSMIT =0x05
 * Wait for the sending to complete GO_WAIT_TRANSMIT =0x06
 * Sleep GO_SLEEP =0x07
 * Internal error GO_ERROR =0x08
 */
uint8e_t E07x_GetStatus(void)
{
  return (uint8e_t)E07x_Status;;
}

/* !
 * @brief polling function assists in completing data reception
 *
 * @return 0
 * @note needs to be called periodically by the main function
 */
uint8e_t E07x_TaskForPoll(void)
{
    uint8e_t recvSize;
    //if (0 == Ebyte_Port_Gdo0IoRead())             // Detect whether the wireless module generates a receiving interrupt
    if (received) // flag filled in IRQ
    {
        received = 0;
    //    while (Ebyte_Port_Gdo0IoRead() == 0);

        // Read the received data length
        recvSize = 0;
        E07x_GetFIFO(&recvSize , 1);

        // If the received length is not 0
        if( recvSize != 0 )
        {
            E07x_GetFIFO( E07x_RxBuffer, recvSize );
            // callback user function
            Ebyte_Port_ReceiveCallback( 0x0002 , E07x_RxBuffer , recvSize );
            mprintf("irq %d\n", irq_counter);
            irq_counter = 0;
        }
        else
        {
            // To-Do please handle other situations by yourself such as preamble only? Interrupt type interrupt?
            E07x_GoReceive();// Example continues to receive
        }
    }

    return 0;
}