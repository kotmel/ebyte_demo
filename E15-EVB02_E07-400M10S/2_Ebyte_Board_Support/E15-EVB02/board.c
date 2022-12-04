/**
  **********************************************************************************
* @file board.c
* @brief E15-EVB02 board-level software driver layer
* @details See https://www.ebyte.com/ for details
* @author JiangHeng
* @date 2021-05-06
* @version 1.0.0
  **********************************************************************************
  * @copyright BSD License
  *            Chengdu Ebyte Electronic Technology Co., Ltd.
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

// / Key event queue
BSP_BTN_FIFO_t BSP_BTN_FIFO;


// / Auxiliary delay calculation for timer interrupt decrement
volatile uint32_t Ebyte_TimerDelayCounter = 0;


/* !
* @brief internal clock initialization
*/
void Ebyte_BSP_HSI_Init( void )
{
/* Internal 16M HSI clock */
    CLK_SYSCLKSourceConfig( CLK_SYSCLKSource_HSI );
    /* 1 frequency division 16M/1 */
    CLK_SYSCLKDivConfig( CLK_SYSCLKDiv_1 );
}

/* !
* @brief Initialization for E22 E220 common pins
 */
void Ebyte_BSP_E07xGPIO_Init( void )
{
    GPIO_Init( BSP_GPIO_PORT_E07_GDO0, BSP_GPIO_PIN_E07_GDO0, GPIO_Mode_In_PU_IT );
    EXTI_SetPinSensitivity( EXTI_Pin_0, EXTI_Trigger_Rising );

    GPIO_Init( BSP_GPIO_PORT_E07_GDO1, BSP_GPIO_PIN_E07_GDO1, GPIO_Mode_In_PU_No_IT );
}

/* !
* @brief initialize all IO
*
* @note target hardware: EBYTE E15-EVB02
*/
void Ebyte_BSP_GPIO_Init( void )
{
    /* Pin initialization according to the module category */
    Ebyte_BSP_E07xGPIO_Init();
    /* LED pins */
    GPIO_Init( BSP_GPIO_PORT_LED_1, BSP_GPIO_PIN_LED_1, GPIO_Mode_Out_PP_Low_Slow );
    GPIO_Init( BSP_GPIO_PORT_LED_2, BSP_GPIO_PIN_LED_2, GPIO_Mode_Out_PP_Low_Slow );
    /* button pin */
    GPIO_Init( BSP_GPIO_PORT_BUTTON_1, BSP_GPIO_PIN_BUTTON_1, GPIO_Mode_In_PU_No_IT );
    GPIO_Init( BSP_GPIO_PORT_BUTTON_2, BSP_GPIO_PIN_BUTTON_2, GPIO_Mode_In_PU_No_IT );
}

/* !
* @brief communication serial port initialization
*
* @note Please note that different MCUs may not require port mapping
*/
void Ebyte_BSP_UART_Init( void )
{
    /* clock */
    CLK_PeripheralClockConfig( BSP_USER_UART_CLOCK, ENABLE );
    /* GPIO */
    GPIO_ExternalPullUpConfig( BSP_GPIO_PORT_UART_TX, BSP_GPIO_PIN_UART_TX, ENABLE );
    GPIO_ExternalPullUpConfig( BSP_GPIO_PORT_UART_RX, BSP_GPIO_PIN_UART_RX, ENABLE );
    /* Port remapping */
    SYSCFG_REMAPPinConfig( REMAP_Pin_USART1TxRxPortA, ENABLE );
    /* Basic parameter configuration E15-EVB02 default baud rate 9600 8N1 */
    USART_Init( BSP_USER_UART, BSP_USER_UART_BAUDRATE, USART_WordLength_8b, USART_StopBits_1, BSP_USER_UART_PARITY, ( USART_Mode_TypeDef )( USART_Mode_Rx | USART_Mode_Tx ) );  // Allow receiving and sending
    /* Turn on receive interrupt */
    USART_ITConfig( BSP_USER_UART, USART_IT_RXNE, ENABLE );
    /* Serial port enable */
    USART_Cmd( BSP_USER_UART, ENABLE );
}

/* !
* @brief Wireless module communication SPI interface initialization function
*/
void Ebyte_BSP_SPI_Init( void )
{
    /* clock */
    CLK_PeripheralClockConfig( CLK_Peripheral_SPI1, ENABLE );
    /* GPIO */
    GPIO_Init( BSP_GPIO_PORT_SPI_NSS,  BSP_GPIO_PIN_SPI_NSS,  GPIO_Mode_Out_PP_High_Fast ); // chip select CS
    GPIO_ExternalPullUpConfig( BSP_GPIO_PORT_SPI_SCK, BSP_GPIO_PIN_SPI_MOSI | BSP_GPIO_PIN_SPI_MISO | BSP_GPIO_PIN_SPI_SCK, ENABLE ); // MOSI MISO SCK
    /* Parameter configuration */
    SPI_Init( BSP_RF_SPI,
              SPI_FirstBit_MSB,                 // Start transmission from high bit
              SPI_BaudRatePrescaler_2,          // 16M/2 SCK rate
              SPI_Mode_Master,                  // Master mode
              SPI_CPOL_Low,                     // According to CPOL=0
              SPI_CPHA_1Edge,                   // Sampling data according to the first clock edge of CPHA=0
              SPI_Direction_2Lines_FullDuplex,  // full duplex
              SPI_NSS_Soft,                     // Software control slave CS chip selection
              0x07 );                           // CRC parameter
    /* enable */
    SPI_Cmd( BSP_RF_SPI, ENABLE );
}

/* !
* @brief RF module SPI communication receiving/sending function
*
* @param data send data
* @return receive data
* @note SPI_SendData()/SPI_ReceiveData() in the stm8l must be guarded by status check
*/
uint8_t Ebyte_BSP_SpiTransAndRecv( uint8_t data )
{
#if 0
    SPI_SendData(BSP_RF_SPI, data);
    while( SPI_GetFlagStatus(BSP_RF_SPI, SPI_FLAG_TXE) == RESET )
     ;

    while( SPI_GetFlagStatus(BSP_RF_SPI, SPI_FLAG_RXNE) == RESET )
     ;

    return SPI_ReceiveData(BSP_RF_SPI);
#else
    BSP_RF_SPI->DR = data;
    while( ( BSP_RF_SPI->SR & SPI_FLAG_TXE ) == RESET );
    while( ( BSP_RF_SPI->SR & SPI_FLAG_RXNE ) == RESET );
    return BSP_RF_SPI->DR;
    #endif
}
/* !
* @brief timer initialization
*
* @note uses TIM3 to generate 1ms periodic interrupt
* The main clock of TIM3 is HSI 16MHz, 128 frequency division is 16 MHz / 128 = 125 000 Hz
* The 1ms counting cycle of target timing is ( 0.001 x 125000 - 1) = 124
 */
void Ebyte_BSP_TIMER_Init( void )
{
    /* clock */
    CLK_PeripheralClockConfig( CLK_Peripheral_TIM3, ENABLE );
    /* parameter */
    TIM3_TimeBaseInit( TIM3_Prescaler_128, TIM3_CounterMode_Up, 124 );
    /* enable interrupt */
    TIM3_ClearFlag( TIM3_FLAG_Update );
    TIM3_ITConfig( TIM3_IT_Update, ENABLE );
    /* enable */
    TIM3_Cmd( ENABLE );
}

// 16 MHz / 128 = 125 000 Hz
// 125000 / 65535 = 1.907
void Ebyte_BSP_TIMER2_Init( void )
{
    /* clock */
    CLK_PeripheralClockConfig( CLK_Peripheral_TIM2, ENABLE );
    /* parameter */
    TIM2_TimeBaseInit( TIM2_Prescaler_128, TIM2_CounterMode_Up, 65535 );
    TIM2_OC1Init(TIM2_OCMode_PWM1, TIM2_OutputState_Disable, 32768, TIM2_OCPolarity_High, TIM2_OCIdleState_Reset);
    /* enable interrupt */
    TIM2_ClearFlag( TIM2_FLAG_Update );
    TIM2_ITConfig( TIM2_IT_Update, ENABLE );
    TIM2_ClearFlag( TIM2_FLAG_CC1 );
    TIM2_ITConfig( TIM2_FLAG_CC1, ENABLE );
     /* enable */
    TIM2_Cmd( ENABLE );
}

/*!
 * @brief E15-EVB02 onboard resource initialization
 *
 * @note internal clock HSI x 16MHz
 *       User communication serial port x USART1
 *       Wireless module communication interface x SPI1
 *       Timer x TIM3
 *       Button x 2
 *       LED x 2
 *       Timer x TIM2
 */
void Ebyte_BSP_Init( void )
{
    /* clock initialization */
    Ebyte_BSP_HSI_Init();
    /* IO initialization */
    Ebyte_BSP_GPIO_Init();
    /* Serial port initialization */
    Ebyte_BSP_UART_Init();
    /* SPI interface initialization */
    Ebyte_BSP_SPI_Init();
    /* Timer initialization */
    Ebyte_BSP_TIMER_Init();
    Ebyte_BSP_TIMER2_Init(); // kk 2Hz blink LED 1 just for test
    /* Key event queue initialization */
    Ebyte_BTN_FIFO_Init( &BSP_BTN_FIFO );
}

/*!
 * @brief control LED on/off/flip
 *
 * @param LEDx Two light-emitting diodes on board
 *              @arg BSP_LED_1 : LED1
 *              @arg BSP_LED_2 : LED2
 *
 * @param ctl    on/off
 *              @arg OFF     : off
 *              @arg ON      : on
 *              @arg TOGGLE  : flip
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
 * @brief Timer-based millisecond delay function
 *
 * @param nTime unit: milliseconds
 */
void Ebyte_BSP_DelayMs( volatile uint32_t nTime )
{
    Ebyte_TimerDelayCounter = nTime;
    while( Ebyte_TimerDelayCounter != 0 );
}

/*!
 * @brief Auxiliary millisecond delay calculation timer interrupt call
 */
void Ebyte_BSP_TimerDecrement( void )
{
    if( Ebyte_TimerDelayCounter != 0 )
    {
        Ebyte_TimerDelayCounter--;
    }
}

/*!
 * @brief read button state
 *
 * @param btn corresponding button number
 *            @arg BSP_BUTTON_1 :Button 1
 *            @arg BSP_BUTTON_2 :Button 2
 * @return 0:the button is pressed, not 0: the button is not pressed
 * @note When the onboard button is not pressed, the IO is in the pull-up state, which means 1; when the IO is grounded, it is 0
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
 * @brief serial port sending function
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
