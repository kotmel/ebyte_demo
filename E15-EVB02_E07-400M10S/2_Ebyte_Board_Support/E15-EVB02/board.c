/**
  **********************************************************************************
  * @file      board.c
  * @brief     E15-EVB02 �弶����������
  * @details   ������μ� https://www.ebyte.com/
  * @author    JiangHeng
  * @date      2021-05-06
  * @version   1.0.0
  **********************************************************************************
  * @copyright BSD License
  *            �ɶ��ڰ��ص��ӿƼ����޹�˾
  *   ______   ____   __     __  _______   ______
  *  |  ____| |  _ \  \ \   / / |__   __| |  ____|
  *  | |__    | |_) |  \ \_/ /     | |    | |__
  *  |  __|   |  _ <    \   /      | |    |  __|
  *  | |____  | |_) |    | |       | |    | |____
  *  |______| |____/     |_|       |_|    |______|
  *
  **********************************************************************************
  */

#include "board.h"

/// �����¼�����
BSP_BTN_FIFO_t BSP_BTN_FIFO;


/// ������ʱ���� ���ڶ�ʱ���ж� �ݼ� 
volatile uint32_t Ebyte_TimerDelayCounter = 0;


/*!
 * @brief �ڲ�ʱ�ӳ�ʼ��
 */
void Ebyte_BSP_HSI_Init( void )
{
    /* �ڲ� 16M HSI ʱ�� */
    //CLK_SYSCLKSourceConfig( CLK_SYSCLKSource_HSI );
     CLK->SWR = (uint8_t)CLK_SYSCLKSource_HSI;
    /* 1��Ƶ  16M/1 */
    //CLK_SYSCLKDivConfig( CLK_SYSCLKDiv_1 );
    CLK->CKDIVR = (uint8_t)(CLK_SYSCLKDiv_1);
}

/*!
 * @brief ���E07 ��ͨ��������ʼ��
 */
void Ebyte_BSP_E07xGPIO_Init( void )
{
    GPIO_Init( BSP_GPIO_PORT_E07_GDO0, BSP_GPIO_PIN_E07_GDO0, GPIO_Mode_In_PU_No_IT );
    GPIO_Init( BSP_GPIO_PORT_E07_GDO1, BSP_GPIO_PIN_E07_GDO1, GPIO_Mode_In_PU_No_IT );
}

/*!
 * @brief ��ʼ������IO
 *
 * @note  Ŀ��Ӳ��: EBYTE E15-EVB02
 */
void Ebyte_BSP_GPIO_Init( void )
{
    /* ����ģ�����������ų�ʼ�� */
    Ebyte_BSP_E07xGPIO_Init();
    /* LED  ���� */
    GPIO_Init( BSP_GPIO_PORT_LED_1, BSP_GPIO_PIN_LED_1, GPIO_Mode_Out_PP_Low_Slow );
    GPIO_Init( BSP_GPIO_PORT_LED_2, BSP_GPIO_PIN_LED_2, GPIO_Mode_Out_PP_Low_Slow );
    /* ���� ���� */
    GPIO_Init( BSP_GPIO_PORT_BUTTON_1, BSP_GPIO_PIN_BUTTON_1, GPIO_Mode_In_PU_No_IT );
    GPIO_Init( BSP_GPIO_PORT_BUTTON_2, BSP_GPIO_PIN_BUTTON_2, GPIO_Mode_In_PU_No_IT );
}

/*!
 * @brief ͨ�Ŵ��ڳ�ʼ��
 *
 * @note  ��ע�⣬��ͬ��MCU��������˿�ӳ��
 */
void Ebyte_BSP_UART_Init( void )
{
    /* ʱ�� */
    CLK_PeripheralClockConfig( BSP_USER_UART_CLOCK, ENABLE );
    /* GPIO */
    GPIO_ExternalPullUpConfig( BSP_GPIO_PORT_UART_TX, BSP_GPIO_PIN_UART_TX, ENABLE );
    GPIO_ExternalPullUpConfig( BSP_GPIO_PORT_UART_RX, BSP_GPIO_PIN_UART_RX, ENABLE );
    /* �˿���ӳ��  */
    SYSCFG_REMAPPinConfig( REMAP_Pin_USART1TxRxPortA, ENABLE );
    /* ������������ E15-EVB02Ĭ�ϲ�����9600 8N1 */
    USART_Init( BSP_USER_UART, BSP_USER_UART_BAUDRATE, USART_WordLength_8b, USART_StopBits_1, BSP_USER_UART_PARITY, ( USART_Mode_TypeDef )( USART_Mode_Rx | USART_Mode_Tx ) );  //�������պͷ���
    /* �򿪽����ж� */;
    USART_ITConfig( BSP_USER_UART, USART_IT_RXNE, ENABLE );
    /* ���� ʹ�� */
    USART_Cmd( BSP_USER_UART, ENABLE );
}

/*!
 * @brief ����ģ��ͨ��SPI�ӿڳ�ʼ������
 */
void Ebyte_BSP_SPI_Init( void )
{
    /* ʱ�� */
    CLK_PeripheralClockConfig( CLK_Peripheral_SPI1, ENABLE );
    /* GPIO */
    GPIO_Init( BSP_GPIO_PORT_SPI_NSS,  BSP_GPIO_PIN_SPI_NSS,  GPIO_Mode_Out_PP_High_Fast ); //Ƭѡ CS
    GPIO_ExternalPullUpConfig( BSP_GPIO_PORT_SPI_SCK, BSP_GPIO_PIN_SPI_MOSI | BSP_GPIO_PIN_SPI_MISO | BSP_GPIO_PIN_SPI_SCK, ENABLE ); // MOSI MISO SCK
    /* �������� */
    SPI_Init( BSP_RF_SPI,
              SPI_FirstBit_MSB,                 //�Ӹ�λ��ʼ����
              SPI_BaudRatePrescaler_2,          //16M/2 SCK����
              SPI_Mode_Master,                  //����ģʽ
              SPI_CPOL_Low,                     //���� CPOL=0
              SPI_CPHA_1Edge,                   //���� CPHA=0  ��һ��ʱ�ӱ��ز�������
              SPI_Direction_2Lines_FullDuplex,  //ȫ˫��
              SPI_NSS_Soft,                     //�������ƴӻ�CSƬѡ
              0x07 );                           //CRC����
    /* ʹ�� */
    SPI_Cmd( BSP_RF_SPI, ENABLE );
}

/*!
 * @brief RFģ��SPIͨ����/������
 *
 * @param data ��������
 * @return ��������
 * @note stm8l SPI�⺯���е�SPI_SendData()/SPI_ReceiveData() ����ֱ��ʹ��
 */
uint8_t Ebyte_BSP_SpiTransAndRecv( uint8_t data )
{
    BSP_RF_SPI->DR = data;
    while( ( BSP_RF_SPI->SR & SPI_FLAG_TXE ) == RESET );
    while( ( BSP_RF_SPI->SR & SPI_FLAG_RXNE ) == RESET );
    return BSP_RF_SPI->DR;
}
/*!
 * @brief ��ʱ����ʼ��
 *
 * @note  ʹ����TIM3����1ms�������ж�
 *        TIM3����ʱ��ΪHSI 16MHz, 128��Ƶ��Ϊ 16 MHz / 128 = 125 000 Hz
 *        Ŀ�궨ʱ1ms �������ڼ�Ϊ ( 0.001 x 125000 - 1) = 124
 */
void Ebyte_BSP_TIMER_Init( void )
{
    /* ʱ�� */
    CLK_PeripheralClockConfig( CLK_Peripheral_TIM3, ENABLE );
    /* ���� */
    //TIM3_TimeBaseInit( TIM3_Prescaler_128, TIM3_CounterMode_Up, 124 );
    /* Set the Autoreload value */
    TIM3->ARRH = (uint8_t)(124 >> 8) ;
    TIM3->ARRL = (uint8_t)(124);

    /* Set the Prescaler value */
    TIM3->PSCR = (uint8_t)(TIM3_Prescaler_128);

    /* Select the Counter Mode */
    TIM3->CR1 &= (uint8_t)((uint8_t)(~TIM_CR1_CMS)) & ((uint8_t)(~TIM_CR1_DIR));
    TIM3->CR1 |= (uint8_t)(TIM3_CounterMode_Up);

    /* Generate an update event to reload the Prescaler value immediately */
    TIM3->EGR = TIM3_EventSource_Update;
    /* �����ж� */
    //TIM3_ClearFlag( TIM3_FLAG_Update );
    TIM3->SR1 = (uint8_t)(~(uint8_t)(TIM3_FLAG_Update));
    TIM3->SR2 = (uint8_t)(~(uint8_t)((uint16_t)TIM3_FLAG_Update >> 8));
    //TIM3_ITConfig( TIM3_IT_Update, ENABLE );
    TIM3->IER |= (uint8_t)TIM3_IT_Update;
    /* ʹ�� */
    //TIM3_Cmd( ENABLE );
    TIM3->CR1 |= TIM_CR1_CEN;
}

// 16 MHz / 128 = 125 000 Hz
// 125000 / 65535 = 1.907
void Ebyte_BSP_TIMER2_Init( void )
{
    /* ʱ�� */
    CLK_PeripheralClockConfig( CLK_Peripheral_TIM2, ENABLE );
    /* ���� */
    //TIM2_TimeBaseInit( TIM2_Prescaler_128, TIM2_CounterMode_Up, 124 );
    
     /* Set the Autoreload value */
    TIM2->ARRH = (uint8_t)(255) ;
    TIM2->ARRL = (uint8_t)(255);

    /* Set the Prescaler value */
    TIM2->PSCR = (uint8_t)(TIM2_Prescaler_128);

    /* Select the Counter Mode */
    TIM2->CR1 &= (uint8_t)((uint8_t)(~TIM_CR1_CMS)) & ((uint8_t)(~TIM_CR1_DIR));
    TIM2->CR1 |= (uint8_t)(TIM2_CounterMode_Up);

    /* Generate an update event to reload the Prescaler value immediately */
    TIM2->EGR = TIM2_EventSource_Update;

    /* �����ж� */
    //TIM2_ClearFlag( TIM2_FLAG_Update );
    TIM2->SR1 = (uint8_t)(~(uint8_t)(TIM2_FLAG_Update));
    TIM2->SR2 = (uint8_t)(~(uint8_t)((uint16_t)TIM2_FLAG_Update >> 8));

    //TIM2_ITConfig( TIM2_IT_Update, ENABLE );
    TIM2->IER |= (uint8_t)TIM2_IT_Update;
    
     /* ʹ�� */
    //TIM2_Cmd( ENABLE );
    TIM2->CR1 |= TIM_CR1_CEN;
}

/*!
 * @brief E15-EVB02 ������Դ��ʼ��
 *
 * @note  �ڲ�ʱ��HSI  x 16MHz
 *        �û�ͨ�Ŵ��� x USART1
 *        ����ģ��ͨ�Žӿ� x SPI1
 *        ��ʱ��  x TIM3
 *        ����    x 2
 *        ָʾ��  x 2
 */
void Ebyte_BSP_Init( void )
{
    /* ʱ��     ��ʼ�� */
    Ebyte_BSP_HSI_Init();
    /* IO       ��ʼ�� */
    Ebyte_BSP_GPIO_Init();
    /* ����     ��ʼ�� */
    Ebyte_BSP_UART_Init();
    /* SPI�ӿ�  ��ʼ��  (E49�Ƚ�����) */
    Ebyte_BSP_SPI_Init();
    /* ��ʱ��   ��ʼ�� */
    Ebyte_BSP_TIMER_Init();
    //Ebyte_BSP_TIMER2_Init(); // kk 2Hz blink LED 1 just for test
    /* �����¼����� ��ʼ�� */
    Ebyte_BTN_FIFO_Init( &BSP_BTN_FIFO );
}

/*!
 * @brief ����LED ��/��/��ת
 *
 * @param LEDx  �������ŷ��������
 *              @arg BSP_LED_1 : LED1
 *              @arg BSP_LED_2 : LED2
 *
 * @param ctl   �� / ��
 *              @arg OFF     : ��
 *              @arg ON      : ��
 *              @arg TOGGLE  : ��ת
 */
void Ebyte_BSP_LedControl( BSP_LED_t LEDx, BSP_LED_Ctl_t ctl )
{
    if( TOGGLE == ctl )
    {
        switch( LEDx )
        {
            case BSP_LED_1 :
                GPIO_ToggleBits( BSP_GPIO_PORT_LED_1, BSP_GPIO_PIN_LED_1 );
                break;
            case BSP_LED_2 :
                GPIO_ToggleBits( BSP_GPIO_PORT_LED_2, BSP_GPIO_PIN_LED_2 );
                break;
            default:
                break;
        }
    }
    else
    {
        switch( LEDx )
        {
            case BSP_LED_1 :
                GPIO_WriteBit( BSP_GPIO_PORT_LED_1, BSP_GPIO_PIN_LED_1, ( BitAction )ctl );
                break;
            case BSP_LED_2 :
                GPIO_WriteBit( BSP_GPIO_PORT_LED_2, BSP_GPIO_PIN_LED_2, ( BitAction )ctl );
                break;
            default:
                break;
        }
    }
}


/*!
 * @brief ���ڶ�ʱ���ĺ�����ʱ����
 *
 * @param nTime ��λ:����
 */
void Ebyte_BSP_DelayMs( volatile uint32_t nTime )
{
    Ebyte_TimerDelayCounter = nTime;
    while( Ebyte_TimerDelayCounter != 0 );
}

/*!
 * @brief ����������ʱ���� ��ʱ���жϵ���
 */
void Ebyte_BSP_TimerDecrement( void )
{
    if( Ebyte_TimerDelayCounter != 0 )
    {
        Ebyte_TimerDelayCounter--;
    }
}

/*!
 * @brief ��ȡ����״̬
 *
 * @param btn ��Ӧ�İ������
 *            @arg BSP_BUTTON_1 :����1
 *            @arg BSP_BUTTON_2 :����2
 * @return 0:����������  ��0:����δ����
 * @note  ���ذ���δ����ʱ IO��������״̬ ��Ϊ1�����º�IO�ӵ� ��Ϊ0
 */
uint8_t Ebyte_BSP_ReadButton( BSP_BUTTON_t btn )
{
    BitStatus result = RESET;
    switch( btn )
    {
        case BSP_BUTTON_1:
            result = GPIO_ReadInputDataBit( BSP_GPIO_PORT_BUTTON_1, BSP_GPIO_PIN_BUTTON_1 );
            break;
        case BSP_BUTTON_2:
            result = GPIO_ReadInputDataBit( BSP_GPIO_PORT_BUTTON_2, BSP_GPIO_PIN_BUTTON_2 );
            break;
        default :
            break;
    }
    return result;
}

/*!
 * @brief ���ڷ��ͺ���
 */
void Ebyte_BSP_UartTransmit( uint8_t* buffer, uint16_t length )
{
    uint8_t i;
    for( i = 0; i < length; i++ )
    {
        while( USART_GetFlagStatus( USART1, USART_FLAG_TXE ) == RESET );
        USART_SendData8( USART1, *buffer++ );
    }
}
