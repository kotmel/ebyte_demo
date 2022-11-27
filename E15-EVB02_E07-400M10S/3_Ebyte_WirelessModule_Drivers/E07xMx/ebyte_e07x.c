#include "board.h"
#include "ebyte_e07x.h"

extern volatile int received;

volatile int irq_counter = 0;

/// ����ʶ��ģ��
#if defined(EBYTE_E07_400M10S)
static uint8e_t E07x_NameString[] = "E07-400M10S";
#elif defined(EBYTE_E07_900M10S)
static uint8e_t E07x_NameString[] = "E07-900M10S";
#endif 

/// ����ʶ����������汾�� 
#define EBYTE_E07_TYPE_PROGRAM   0x10 

#define EBYTE_E07X_XOSC         26000000  //26MHz����
#define EBYTE_E07X_POW_2_28     268435456 //2��28�η�
#define EBYTE_E07X_POW_2_17     131072
#define EBYTE_E07X_CAL_BASE     ((double)EBYTE_E07X_POW_2_28) / ((double)EBYTE_E07X_XOSC)

///ָ���ڲ�FIFO
#define  FIFO_READ           0x00
#define  FIFO_WRITE          0x01   

///ָ���ж�����
#define IRQ_RX_SYNC_RECV 0x06  
#define IRQ_TX_SYNC_SEND 0x06  
#define IRQ_RX_CRC_OK    0x07

///������ʱ� ��������
static uint8e_t E07x_PaTabel[] = { 0xC0, 0, 0, 0, 0, 0, 0, 0};

///���ջ���
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

/// ״̬��ʶ
static E07x_Status_t E07x_Status = GO_INIT;

/* �������ò��� 
 * �����������TI SmartRF Studio 7 
 * �汾 2.21.0 */
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
 * @brief �����ڲ��Ĵ���ֵ
 * 
 * @param address �Ĵ�����ַ
 * @param data    д��ֵ
 */
static void E07x_SetRegister( uint8e_t address, uint8e_t data )
{
    /* SPI CS */
    Ebyte_Port_SpiCsIoControl(0);
    
    /* д���ַ */
    Ebyte_Port_SpiTransmitAndReceivce( address );
    
    /* д��ֵ */
    Ebyte_Port_SpiTransmitAndReceivce( data );
    
    /* SPI CS */
    Ebyte_Port_SpiCsIoControl(1);    
}

/*!
 * @brief ��ȡ�ڲ��Ĵ���ֵ
 * 
 * @param address �Ĵ�����ַ
 * @return �Ĵ���ֵ
 */
static uint8e_t E07x_GetRegister( uint8e_t address )
{
    uint8e_t result;
    
 
    
    /* SPI CS */
    Ebyte_Port_SpiCsIoControl(0);
    
    /* 0x30��ַ�Ժ�ļĴ��� ��ȡָ����Ҫ�ı� */
    if( address < 0x30 )
    {
        /* д��ַ  bit7��1 ��ʾ������Ҫ��ȡ ��|0x80 */
        Ebyte_Port_SpiTransmitAndReceivce( address | 0x80);    
    }
    else
    {
         /* д��ַ  */
        Ebyte_Port_SpiTransmitAndReceivce( address | 0xC0);         
    }

    /* ��ȡ 1Byte */
    result = Ebyte_Port_SpiTransmitAndReceivce( 0xFF );
    
    /* SPI CS */
    Ebyte_Port_SpiCsIoControl(1);  
    
    return result;
}

/*!
 * @brief ���������ڲ��Ĵ���ֵ
 * 
 * @param address �Ĵ�����ַ
 * @param data    ָ��д�������׵�ַ
 * @param size    д�볤��
 */
static void E07x_SetRegisters( uint8e_t address , uint8e_t *data , uint8e_t size )
{
    uint8e_t i = size;
    
    /* SPI CS */
    Ebyte_Port_SpiCsIoControl(0);
    
    /* д����ʼ��ַ ��ַbit6��λ��������дָ�� ��|0x40 */
    Ebyte_Port_SpiTransmitAndReceivce( address | 0x40 );
    
    /* д������ */
    while( i-- )
    {
        Ebyte_Port_SpiTransmitAndReceivce( *data++ );
    }
    
    /* SPI CS */
    Ebyte_Port_SpiCsIoControl(1);
}

/*!
 * @brief д����
 * 
 * @param address �Ĵ�����ַ
 */
static void E07x_SendCommand( uint8e_t command )
{
    /* SPI CS */
    Ebyte_Port_SpiCsIoControl(0);
    
    /* д������ */
    Ebyte_Port_SpiTransmitAndReceivce( command );
    
    /* SPI CS */
    Ebyte_Port_SpiCsIoControl(1);
}

/*!
 * @brief ��FIFOд�����������
 * 
 * @param data ָ��������
 * @param size д�볤��
 */
static void E07x_SetFIFO( uint8e_t *data, uint8e_t size)
{
    uint8e_t i = size;
    
    /* SPI CS */
    Ebyte_Port_SpiCsIoControl(0);
    
    /* FIFO��ַ:0x3F ��ַbit6��λ��������дָ�� ��|0x40 */
    Ebyte_Port_SpiTransmitAndReceivce( 0x3F | 0x40 );
    
    /* д������ */
    while( i-- )
    {
        Ebyte_Port_SpiTransmitAndReceivce( *data++ );
    }
    
    /* SPI CS */
    Ebyte_Port_SpiCsIoControl(1);
}

/*!
 * @brief ��FIFO��ȡ����
 * 
 * @param data ָ�򻺴�
 * @param size ��ȡ����
 */
static void E07x_GetFIFO( uint8e_t *data, uint8e_t size)
{
    uint8e_t i = size;
    
    /* SPI CS */
    Ebyte_Port_SpiCsIoControl(0);
    
    /* FIFO��ַ:0x3F ��ַbit6��λ����������ָ�� ��|0x40 */
    Ebyte_Port_SpiTransmitAndReceivce( 0x3F | 0xC0 );
    
    /* д������ */
    while( i-- )
    {
        *data++ = Ebyte_Port_SpiTransmitAndReceivce( 0xFF );
    }
    
    /* SPI CS */
    Ebyte_Port_SpiCsIoControl(1);
}

/*!
 * @brief ���FIFO����
 * @param mode ģʽ
 *           @arg FIFO_WRITE ��ָ������FIFO
 *           @arg FIFO_READ  ��ָ������FIFO
 */
static void E07x_ClearFIFO( uint8e_t mode )
{
  
    if( mode == FIFO_WRITE )
    {    
        /* ָ��:�������FIFO(0x3B) */
        E07x_SendCommand(0x3B);
    }
    else
    {
        /* ָ��:�������FIFO(0x3A) */
        E07x_SendCommand(0x3A);
    } 
    
}

/*!
 * @brief ��λ
 */
void E07x_Reset(void)
{
    /* SPI CS */
    Ebyte_Port_SpiCsIoControl(1);
    Ebyte_Port_SpiCsIoControl(0);
    Ebyte_Port_DelayMs(1);
    Ebyte_Port_SpiCsIoControl(1);
    Ebyte_Port_DelayMs(1);
    
    /* ����:оƬ��λ(0x30) */
    E07x_SendCommand( 0x30 );
    
    Ebyte_Port_DelayMs(20);
}

/*!
 * @brief �л��������ģʽ
 */
static void E07x_SetStby(void)
{
    /* ָ��:0x36 �˳�TX/RX ģʽ �������״̬ */
    E07x_SendCommand(0x36);
    
    /* ״̬��¼ */
    E07x_Status = GO_STBY;
}

/*!
 * @brief �л���������ģʽ
 * 
 * @note ������ֲᣬ���ּĴ����Ĳ����ᶪʧ��
 */
static void E07x_SetSleep(void)
{
    /* ָ��:0x39 ��������ģʽ */
    E07x_SendCommand(0x39);
    
    /* ״̬��¼ */
    E07x_Status = GO_SLEEP;
}

static void E07x_SetGPIO( uint8e_t gpio , uint8e_t config )
{
  
  /* ����ָ��IO�ļ��� ���ж�ʱΪ�ߵ�ƽ �����ж�ʱΪ�͵�ƽ */
  uint8e_t mask = 0x40;
  
  /* �ϲ��ж����� */ 
  mask |= (config & 0x3F);
  
  switch ( gpio )
  {
    /* IOCFG0 �Ĵ�����ַ:0x02 */
    case 0:  E07x_SetRegister(0x02,mask) ;break;
    /* IOCFG1 �Ĵ�����ַ:0x01 */
    case 1:  E07x_SetRegister(0x01,mask) ;break;
    /* IOCFG2 �Ĵ�����ַ:0x00 */
    case 2:  E07x_SetRegister(0x00,mask) ;break;
    default: break;
  }
}

/*!
 * @brief �ж�ģ���Ƿ����
 *
 * @return 0:���� 1:ģ���쳣
 */
static uint8e_t E07x_IsExist(void)
{
    uint8e_t result = 0;
    uint8e_t reg_value;
    
    /* ���¶ȴ��������ò��� ��λ��Ӧ��Ϊ0x7F �Ĵ�����ַ:0x2A  */
    reg_value = E07x_GetRegister(0x2A);
    
    /* Ĭ��ֵ��Ϊ0x7F */
    if( reg_value != 0x7F ) 
    {
       /* ��������ȷ���� ����Ӳ�� */
       result = 1;
    }
    
    return result;
}

/*!
 * @brief ���Ʒ�ʽ
 *
 * @param mod �����ĵ��Ʒ�ʽ 
 *        @arg 0:2-FSK
 *        @arg 1:GFSK
 *        @arg 3:ASK/OOK
 *        @arg 4:4-FSK
 *        @arg 7:MSK
 */
static void E07x_SetModulation(uint8e_t mod)
{
    /* ת��λ */
    uint8e_t tmp = (mod<<4) & 0x70;
  
    /* MDMCFG2 �Ĵ�����ַ 0x12  ���bit[6:4]*/
    E07x_InitSetting.mdmcfg2  &= 0x8F;
    
    /* ����bit[6:4] */
    E07x_InitSetting.mdmcfg2 |= tmp;
}

/*!
 * @brief �����ز�Ƶ�ʻ�׼ֵ����
 *
 * @param frequency �������ز�Ƶ�� Hz
 * @return 0:���� 1:��������
 * @note ����������Ƶ��֮��( Hz ):
 *        300000000 - 348000000
 *        387000000 - 464000000
 *        779000000 - 928000000
 *       ������ע��:��ͬ�ܹ�������������С�����ֿ��ܲ�һ�£��п��ܲ���ƫ��
 *
 *       Ƶ���ǲ���Ƶ��OK��ʵ�����ٷ���������������Ƶ�ʵ�ģ�����ͨ�š�
 */
static uint8e_t E07x_SetFrequency( uint32e_t frequency )
{ 
   uint32e_t freq ;
   uint8e_t  freq2,freq1,freq0;
   
   /* ������� */
   if(  frequency > 928000000 ) return 1;
   if( (frequency < 779000000) && ( frequency>464000000) )return 1;
   if( (frequency < 387000000) && ( frequency>348000000) )return 1;
   if( (frequency < 300000000)) return 1;
   
   /* Ƶ�ʿ�������(FREQ)=�ز�Ƶ�� *��2^16)/ ����Ƶ��   */
   freq = (uint32e_t)(((double)frequency * 65535 )/ ( (double)EBYTE_E07X_XOSC ));
   freq2 = ( uint8e_t )( ( freq >> 16 ) & 0xFF );
   freq1 = ( uint8e_t )( ( freq >> 8 ) & 0xFF );
   freq0 = ( uint8e_t )(  freq  & 0xFF );
   
   /* ����ʼ�����������е�Ƶ�ʲ��� �ȴ�����ͳһд��  */
   E07x_InitSetting.freq2 = freq2;
   E07x_InitSetting.freq1 = freq1;
   E07x_InitSetting.freq0 = freq0;
   
   return 0;
}

/*!
 * @brief �����������ʻ�׼ֵ����
 *
 * @param datarate ��������������
 * @note  ��������������֮��:
 *        300 (0.3K) - 250000000 (250K)
 */
static void E07x_SetDataRate( uint32e_t datarate )
{
    uint8e_t  date_m ;
    uint16e_t date_e ;
    uint32e_t date_e_sum = 1,temp;
    
    /* �м乫ʽ���� (256+DRATE_M)*2^DRATE_E=datarate*2^28/����Ƶ��  */
    uint32e_t calculation = (uint32e_t)( datarate * EBYTE_E07X_CAL_BASE);
    
    /* ��αƽ�ƥ�� DRATE_E */
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
       
       temp = calculation/date_e_sum; // ��������� [256 ,256+DRATE_M]֮��
       if(  temp>=256 && temp<=511 )
       {
          date_m = temp - 256;//�õ�DRATE_M
          break;
       }
    }
    
    /* ����ʼ�����������е��������ʲ��� �ȴ�����ͳһд��  */
    if ( date_e<256 )
    {
       E07x_InitSetting.mdmcfg4 &= 0xF0;
       E07x_InitSetting.mdmcfg4 |= date_e;
       E07x_InitSetting.mdmcfg3 = date_m;    
    }
    
}

/*!
 * @brief ����Ƶ���׼ֵ����
 *
 * @param bandwidth �����Ľ���Ƶ�� (Hz)
 * @note ʵ��͹ٷ����������һ�� Ĭ�����58.035714
 *       ע�⣺оƬʵ��ֻ������Ϊ�̶�����Ƶ���������ֵ�ᱻ����ת���ӽ������¹̶�Ƶ��
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
    
    /* �м乫ʽ���� (4+CHANBW_M)*2^CHANBW_E=����Ƶ��/(8*bandwidth)  */
    uint32e_t calculation = (uint32e_t)( EBYTE_E07X_XOSC/(8 * bandwidth));    
    
    /* ��αƽ�ƥ�� CHANBW_E */
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
          chanbw_m = temp - 4;//�õ�CHANBW_M
          break;
       }
    }    
    
    /* ����ʼ�����������е�Ƶ���� �ȴ�����ͳһд��  */
    if( chanbw_e<=3 )
    {
        mask = ((uint8e_t)((chanbw_e<<6)|(chanbw_m<<4))) & 0xF0;
        E07x_InitSetting.mdmcfg4 &= 0x0F;
        E07x_InitSetting.mdmcfg4 |= mask;
    }
    
}

/*!
 * @brief ����Ƶƫ��׼ֵ
 * 
 * @param frequency_dev ��λ:Hz 
 * 
 * @note ʵ��͹ٷ�������һ��
 */
static void E07x_SetFrequencyDeviation( uint32e_t frequency_dev )
{
    uint8e_t  deviation_m,deviation_e,mask;
    uint32e_t deviation_e_sum = 1,temp;
    
    /* �м乫ʽ���� (8+DEVIATION_M)*2^DEVIATION_E=frequency_dev * 2^17 / ����Ƶ��  */
    uint32e_t calculation = (uint32e_t)( frequency_dev * EBYTE_E07X_POW_2_17 / EBYTE_E07X_XOSC );   
    
    /* ��αƽ�ƥ�� DEVIATION_E */
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
        
       temp = calculation/deviation_e_sum; //�õ� 8+DEVIATION_M
       
       if(  temp>=8 && temp<=15 )
       {
          deviation_m = temp - 8;//�õ�CHANBW_M
          break;
       }
    }      
    
    /* ����ʼ�����������е�Ƶƫ���� �ȴ�����ͳһд��  */
    if( deviation_e<=7 )
    {
        mask = ((uint8e_t)((deviation_e<<4)|deviation_m)) & 0x77;
        E07x_InitSetting.deviatn &= 0x88;
        E07x_InitSetting.deviatn |= mask;
    }    
}

/*!
 * @brief �����������
 * 
 * @param power ��λ dBm ע��:����ֵ��������  
 * 
 * @return 0������ 1:��������ȷ
 * @note ������PA Ramping��Ҳ��ʧȥ��һ�������ԶȺ������ԣ���ɢ�ź����ӣ� 
 *       ���ʵ�������������Եģ����������õ�PAֵ����TI SmartRF Studio����Ϊ�ο�
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
    
    if( E07_FREQUENCY_START > 900000000 ) //900MHz �� 928MHz
    {
        switch ( power ) 
        {
            case 11:  E07x_PaTabel[0] = 0xC0;break; //��Ƶ������ ���ʿ���������11dBm    
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
    else //779MHz �� 900MHz
    {
        switch ( power ) 
        {
            case 12:  E07x_PaTabel[0] = 0xC0;break; //��Ƶ������ ���ʿ���������12dBm    
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
 * @brief ģ�������������
 * 
 * @param frequency      �ز�Ƶ��
 * @param data_rate      ��������(��������)
 * @param frequency_dev  ����Ƶƫ
 * @param bandwidth      ����Ƶ��
 * @param output_power   �������
 * @param preamble_size  ����ǰ���볤�� 
 * @param sync_word      ͬ����
 * @param crc            CRC����
 * @param device_address �豸��ַ
 *
 * @return 0:���� 1:��������
 * @note (��ѡ)Ĭ�Ϲر��˵�ַ���
 */
static uint8e_t E07x_Config( uint32e_t frequency , uint32e_t data_rate , uint32e_t frequency_dev ,uint32e_t bandwidth, 
                              int8e_t output_power , uint16e_t preamble_size, uint16e_t sync_word , uint8e_t crc )
{
    uint8e_t result ;  
    uint8e_t reg_value;
    
    /* ����:�ز�Ƶ�� 
     * �Ĵ�����ʼ��ַ 0x0D */
    result = E07x_SetFrequency( frequency );
    if( result != 0 ) return 1;
    
    /* ����:���� 
     * MDMCFG4 MDMCFG3�Ĵ�����ַ:0x10 0x11 */
    E07x_SetDataRate( data_rate );
    
    /* ����:Ƶƫ 
     * DEVIATN�Ĵ�����ַ:0x15 */
    E07x_SetFrequencyDeviation( frequency_dev );    
    
    /* ����:����Ƶ�� 
     * MDMCFG4�Ĵ�����ַ:0x10 */
    E07x_SetChannelBandwidth( bandwidth );
    
    /* ����:������� 
     * �Ĵ�����ַ:0x3E */
    E07x_SetOutputPower(output_power);
    
    /* ����ģʽ
     * MDMCFG2 �Ĵ�����ַ:0x12 */
    E07x_SetModulation(1);//GFSK
    
    /* ǰ���볤�� 
     * MDMCFG1 �Ĵ�����ַ:0x13 */  
    if( preamble_size >7 ) return 1;//�������
    E07x_InitSetting.mdmcfg1 &= 0x8F;//����
    E07x_InitSetting.mdmcfg1 |= (preamble_size<<4);//��λ
    
    /* ͬ���� 
     * SYNC1 SYNC0 �Ĵ�����ַ:0x04 0x05 */
    E07x_InitSetting.sync1 =  (uint8e_t)((sync_word>>8)&0xFF);//��Byte
    E07x_InitSetting.sync0 =  (uint8e_t)(sync_word & 0xFF);   //��Byte
    
    /* CRC���� 
     * PKTCTRL0�Ĵ�����ַ:0x08 bit2 */
    if( crc > 1 ) return 1;//������� ֻ��Ϊ0��1
    if( crc )
    {
        E07x_InitSetting.pktctrl0 |= 0x04; //���� 
    }else
    {
        E07x_InitSetting.pktctrl0 &= 0xFB;
    }
    
    /* ���ݰ����� Ĭ��Ϊ�ɱ䳤ģʽ�ҳ��ȷ������ݵ�һ�ֽ�
     * PKTCTRL0�Ĵ�����ַ:0x08 bit[1:0] */
    E07x_InitSetting.pktctrl0 &= 0xFC;//����
    E07x_InitSetting.pktctrl0 |= 0x01;//0x01ģʽ (�ɱ䳤)
    
    /* ע������:���ϣ����ȫʹ�ùٷ�SmartRF Studio�����ò�������ôֱ��ע�͵��Ϸ����޸ĺ������� */
    /* ��ģ��д������� E07x_InitSetting ������ */
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
//    E07x_SetRegister( 0x2E, E07x_InitSetting.test0); //����: ����TEST0�����Զ������Ƶ��ƫ��  
                                                       //����: ��Ƶ������ΪС��411MHzʱ��Ƶ����������411MHz����Ƶ������Ϊ����411MHzʱ��Ƶ���л�����
                                                       //����: ����ȫ���ùٷ�SmartRF Studio����ʱ�����TEST0��ע�͡���ʹ�����ɲ�������ʱ������ע��
    
    /* ���䣺ͨ������(��������Ƶ)  �Ĵ�����ַ: 0x0A */
    E07x_SetRegister( 0x0A ,0 );
    
    /* ���䣺�رյ�ַ����  �Ĵ�����ַ: 0x07 */
    reg_value = E07x_GetRegister(0x07);
    reg_value &= 0xFC;//����
    E07x_SetRegister(0x07,reg_value);
    
    /* ����: ����������� �Ĵ�����ַ: 0x3E */
    E07x_SetRegisters( 0x3E ,E07x_PaTabel,8 );
    
    return 0;
}

/*!
 * @brief ģ���ʼ��
 * 
 * @return 0:���� 1:��ʼ��ʧ��
 */
uint8e_t E07x_Init( void )
{
    uint8e_t result = 0;
    
    /* ��λ */
    E07x_Reset();
    
    /* �ж�ģ���Ƿ���� */
    result = E07x_IsExist();
    if( result != 0 ) return 1;
    
    /* ������������ */
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
    
    /* Ĭ�Ͻ������ģʽ */
    E07x_GoReceive();
    return 0;
}

/*!
 * @brief ���� 
 */
void E07x_TaskForIRQ(void)
{
    //received = 1;
    //irq_counter++;

    uint8e_t recvSize;
    recvSize = 0;
    E07x_GetFIFO(&recvSize , 1);

    if( recvSize != 0 )
    {
        E07x_GetFIFO( E07x_RxBuffer, recvSize );
        Ebyte_Port_ReceiveCallback( 0x0002 , E07x_RxBuffer , recvSize );
    } else {
        E07x_GoReceive();
    }
}

/*!
 * @brief ��ѯ���� ����������ݽ���
 *  
 * @param data ָ��������
 * @param size ���ݳ���
 * @return 0
 */
uint8e_t E07x_GoTransmit( uint8e_t *data, uint8e_t size )
{
    uint8e_t irqStatus = 0;
    // disable receive IRQ
    BSP_GPIO_PORT_E07_GDO0->CR2 &= (uint8_t)(~(BSP_GPIO_PIN_E07_GDO0));

    /* ģʽ�л�: ���� */
    E07x_SetStby();
    
    /* ����������ݶ��� */
    E07x_ClearFIFO( FIFO_WRITE );
   
    /* ��������Ϊ�ɱ䳤ģ ��һ�ֽ���д�볤�� */
    E07x_SetFIFO( &size , 1 );
    
    /* ����д���������ݰ� */
    E07x_SetFIFO( data , size);
    
    /* �������� GPIO0 ӳ�䵽�ж�
     * �����ͽ���ͬ����ʱ �����ж� ������ע��:��FIFO�е�������ȫ���ͺ� �Զ�������ж� */
    E07x_SetGPIO(0 ,IRQ_TX_SYNC_SEND );

    /* ����ָ��:0x35 ģ��������ģʽ */   
    E07x_SendCommand( 0x35 );      
    
    /* ״̬��¼ */
    E07x_Status = GO_WAIT_TRANSMIT;    
    
    /* �������ͷ�ʽ �ڴ˵ȴ�������� */  
    do
    {
      Ebyte_Port_DelayMs(1);
      
      /* ��ȡ״̬�Ĵ��� MARCSTATE ��ַ:0x35 */
      irqStatus = E07x_GetRegister(0x35);

    }
    while( (irqStatus != 0x01) );//������ɺ��Զ��˻ص�����״̬ ��Ϊ0x01  
                                  //��������˼Ĵ���MCSM1 ��ַ:0x17 ������TXOFF_MODE ��ô������ɺ�״̬����һ���ǿ��� ��Ҫ���д��� 
    
    /* ״̬��¼ */
    E07x_Status = GO_STBY;  
    
    /* �ص��û����� */
    Ebyte_Port_TransmitCallback( 0x0001 );
    
    return 0;
}

/*!
 * @brief ��ʼ��������
 *  
 * @return 0
 */
uint8e_t E07x_GoReceive( void )
{
  
    /* ģʽ�л�: ���� */
    E07x_SetStby();
    
    /* ����������ݶ��� */
    E07x_ClearFIFO( FIFO_READ );  
  
    /* �������� GPIO0 ӳ�䵽�ж� */
//    E07x_SetGPIO(0 ,IRQ_RX_CRC_OK );
     E07x_SetGPIO(0 ,IRQ_RX_SYNC_RECV );
  
    /* ����ָ��:0x34 ģ��������ģʽ */   
    E07x_SendCommand( 0x34 );  
    
    /* ״̬��¼ */
    E07x_Status = GO_WAIT_RECEIVE;
    // enable receive IRQ
    BSP_GPIO_PORT_E07_GDO0->CR2 |= (uint8_t)(BSP_GPIO_PIN_E07_GDO0);
    return 0;
}

/*!
 * @brief ģ���������ģʽ(�͹���)
 * 
 * @return 0;
 */
uint8e_t E07x_GoSleep(void)
{
    E07x_SetSleep();
    
    return 0;
}

/*!
 * @brief ��ȡģ�����汾
 * 
 * @return 8λ�ı��� 
 * @note ����0x10 ����V1.0
 */
uint8e_t E07x_GetDriverVersion(void)
{
  return  EBYTE_E07_TYPE_PROGRAM;
}

/*!
 * @brief ��ȡģ������
 * 
 * @return ָ�������ַ�����ָ�� 
 * @note ��׼�ַ��� ĩβ���н����� '\0'
 */
uint8e_t* E07x_GetName(void)
{
    return E07x_NameString;
}

/*!
 * @brief ��ȡģ��״̬
 * 
 * @return ״̬����
 *        δ��ʼ��     GO_INIT          =0x00        
 *        �����л�״̬ GO_BUSY          =0x01   
 *        ����/����    GO_STBY          =0x02   
 *        ׼�����ջ��� GO_RECEIVE       =0x03   
 *        ���ڼ������� GO_WAIT_RECEIVE  =0x04          
 *        ׼�����ͻ��� GO_TRANSMIT      =0x05   
 *        �ȴ�������� GO_WAIT_TRANSMIT =0x06   
 *        ����         GO_SLEEP         =0x07   
 *        �ڲ�����     GO_ERROR         =0x08 
 */
uint8e_t E07x_GetStatus(void)
{
  return (uint8e_t)E07x_Status;;
}

/*!
 * @brief ��ѯ���� ����������ݽ���
 *  
 * @return 0
 * @note ��Ҫ�������������Ե���
 */
uint8e_t E07x_TaskForPoll(void)
{
    uint8e_t recvSize;
    //if (0 == Ebyte_Port_Gdo0IoRead())             // �������ģ���Ƿ���������ж�
    if (received)
    {
        received = 0;
    //    while (Ebyte_Port_Gdo0IoRead() == 0);

        // ��ȡ���յ������ݳ��� 
        recvSize = 0;
        E07x_GetFIFO(&recvSize , 1);

        // ������ճ��Ȳ�Ϊ0
        if( recvSize != 0 )
        {
            E07x_GetFIFO( E07x_RxBuffer, recvSize );
            //�ص��û�����
            Ebyte_Port_ReceiveCallback( 0x0002 , E07x_RxBuffer , recvSize );
            mprintf("irq %d\n", irq_counter)          ;
            irq_counter = 0;
        }
        else
        {
            //To-Do �����д���������� ����ֻ��ǰ����?�����͵��жϣ�
            E07x_GoReceive();//ʾ�� ��������
        }
    }

    return 0;
}