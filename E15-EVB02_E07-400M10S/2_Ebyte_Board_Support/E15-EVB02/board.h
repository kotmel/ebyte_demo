/**
  **********************************************************************************
  * @file      board.h
  * @brief     E15-EVB02 board-level software driver layer
  * @details   ������μ� https://www.ebyte.com/       
  * @author    JiangHeng     
  * @date      2021-05-06     
  * @version   1.0.0     
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

#include "stm8l15x_conf.h"
#include "board_mini_printf.h"

/* pin configuration LED */
#define BSP_GPIO_PORT_LED_1       GPIOC
#define BSP_GPIO_PIN_LED_1        GPIO_Pin_6

#define BSP_GPIO_PORT_LED_2       GPIOC
#define BSP_GPIO_PIN_LED_2        GPIO_Pin_5

/* Pin configuration button */
#define BSP_GPIO_PORT_BUTTON_1    GPIOA
#define BSP_GPIO_PIN_BUTTON_1     GPIO_Pin_4

#define BSP_GPIO_PORT_BUTTON_2    GPIOA
#define BSP_GPIO_PIN_BUTTON_2     GPIO_Pin_5

/* pin configuration SPI communication interface */
#define BSP_GPIO_PORT_SPI_NSS     GPIOB
#define BSP_GPIO_PIN_SPI_NSS      GPIO_Pin_4

#define BSP_GPIO_PORT_SPI_MOSI    GPIOB
#define BSP_GPIO_PIN_SPI_MOSI     GPIO_Pin_6

#define BSP_GPIO_PORT_SPI_MISO    GPIOB
#define BSP_GPIO_PIN_SPI_MISO     GPIO_Pin_7

#define BSP_GPIO_PORT_SPI_SCK     GPIOB
#define BSP_GPIO_PIN_SPI_SCK      GPIO_Pin_5

/* Pin configuration communication serial port */
#define BSP_GPIO_PORT_UART_TX     GPIOA
#define BSP_GPIO_PIN_UART_TX      GPIO_Pin_2

#define BSP_GPIO_PORT_UART_RX     GPIOA
#define BSP_GPIO_PIN_UART_RX      GPIO_Pin_3

/* Pin configuration EBYTE wireless module control */
/* E22 E220���ż��� */
#define BSP_GPIO_PORT_NRST        GPIOC
#define BSP_GPIO_PIN_NRST         GPIO_Pin_2
#define BSP_GPIO_PORT_BUSY        GPIOC
#define BSP_GPIO_PIN_BUSY         GPIO_Pin_1
#define BSP_GPIO_PORT_RXEN        GPIOD
#define BSP_GPIO_PIN_RXEN         GPIO_Pin_4
#define BSP_GPIO_PORT_TXEN        GPIOD
#define BSP_GPIO_PIN_TXEN         GPIO_Pin_4
/* E07 */
#define BSP_GPIO_PORT_E07_GDO0    GPIOC
#define BSP_GPIO_PIN_E07_GDO0     GPIO_Pin_0    
#define BSP_GPIO_PORT_E07_GDO1    GPIOD
#define BSP_GPIO_PIN_E07_GDO1     GPIO_Pin_4 
/* E10 */
#define BSP_GPIO_PORT_E10_DIO2    GPIOC
#define BSP_GPIO_PIN_E10_DIO2     GPIO_Pin_1    
#define BSP_GPIO_PORT_E10_DIO3    GPIOC
#define BSP_GPIO_PIN_E10_DIO3     GPIO_Pin_0
#define BSP_GPIO_PORT_E10_IRQ     GPIOD
#define BSP_GPIO_PIN_E10_IRQ      GPIO_Pin_4
#define BSP_GPIO_PORT_E10_SDN     GPIOC
#define BSP_GPIO_PIN_E10_SDN      GPIO_Pin_2
/* E19  */
#define BSP_GPIO_PORT_E19_NRST        GPIOC
#define BSP_GPIO_PIN_E19_NRST         GPIO_Pin_0
#define BSP_GPIO_PORT_E19_DIO0        GPIOC
#define BSP_GPIO_PIN_E19_DIO0         GPIO_Pin_1
#define BSP_GPIO_PORT_E19_RXEN        GPIOD
#define BSP_GPIO_PIN_E19_RXEN         GPIO_Pin_4
#define BSP_GPIO_PORT_E19_TXEN        GPIOC
#define BSP_GPIO_PIN_E19_TXEN         GPIO_Pin_2
/* E30 */
#define BSP_GPIO_PORT_E30_IRQ     GPIOD
#define BSP_GPIO_PIN_E30_IRQ      GPIO_Pin_4
#define BSP_GPIO_PORT_E30_SDN     GPIOC
#define BSP_GPIO_PIN_E30_SDN      GPIO_Pin_2
/* E31 */
#define BSP_GPIO_PORT_E31_IRQ     GPIOC
#define BSP_GPIO_PIN_E31_IRQ      GPIO_Pin_2
/* E49 */
#define BSP_GPIO_PORT_E49_DIO1    GPIOC
#define BSP_GPIO_PIN_E49_DIO1     GPIO_Pin_1
#define BSP_GPIO_PORT_E49_DIO2    GPIOC
#define BSP_GPIO_PIN_E49_DIO2     GPIO_Pin_2
#define BSP_GPIO_PORT_E49_DIO3    
#define BSP_GPIO_PIN_E49_DIO3      
#define BSP_GPIO_PORT_E49_CSB     GPIOB
#define BSP_GPIO_PIN_E49_CSB      GPIO_Pin_4
#define BSP_GPIO_PORT_E49_FCSB    GPIOB
#define BSP_GPIO_PIN_E49_FCSB     GPIO_Pin_7
#define BSP_GPIO_PORT_E49_SDIO    GPIOB
#define BSP_GPIO_PIN_E49_SDIO     GPIO_Pin_6
#define BSP_GPIO_PORT_E49_SLCK    GPIOB
#define BSP_GPIO_PIN_E49_SLCK     GPIO_Pin_5


/* Parameter configuration SPI */
#define BSP_RF_SPI                  SPI1
#define BSP_RF_SPI_CLOCK            CLK_Peripheral_SPI1
#define Ebyte_BSP_RfSpiSelected()   GPIO_WriteBit( BSP_GPIO_PORT_SPI_NSS , BSP_GPIO_PIN_SPI_NSS, RESET )
#define Ebyte_BSP_RfSpiUnselected() GPIO_WriteBit( BSP_GPIO_PORT_SPI_NSS , BSP_GPIO_PIN_SPI_NSS, SET )

/* �������� ����ģ�����IO */
/* E22/E220���� */
#define Ebyte_BSP_RfResetIoLow()  GPIO_WriteBit( BSP_GPIO_PORT_NRST , BSP_GPIO_PIN_NRST, RESET )
#define Ebyte_BSP_RfResetIoHigh() GPIO_WriteBit( BSP_GPIO_PORT_NRST , BSP_GPIO_PIN_NRST, SET )
#define Ebyte_BSP_RfTxIoDisable() GPIO_WriteBit( BSP_GPIO_PORT_TXEN , BSP_GPIO_PIN_TXEN, RESET )
#define Ebyte_BSP_RfTxIoEnable()  GPIO_WriteBit( BSP_GPIO_PORT_TXEN , BSP_GPIO_PIN_TXEN, SET )
#define Ebyte_BSP_RfRxIoDisable() GPIO_WriteBit( BSP_GPIO_PORT_RXEN , BSP_GPIO_PIN_RXEN, RESET )
#define Ebyte_BSP_RfRxIoEnable()  GPIO_WriteBit( BSP_GPIO_PORT_RXEN , BSP_GPIO_PIN_RXEN, SET ) 
#define Ebyte_BSP_RfBusyIoRead()  GPIO_ReadInputDataBit( BSP_GPIO_PORT_BUSY , BSP_GPIO_PIN_BUSY)

/* E07 */
#define Ebyte_BSP_RfGdo0IoRead()  GPIO_ReadInputDataBit( BSP_GPIO_PORT_E07_GDO0 , BSP_GPIO_PIN_E07_GDO0)
#define Ebyte_BSP_RfGdo1IoRead()  GPIO_ReadInputDataBit( BSP_GPIO_PORT_E07_GDO1 , BSP_GPIO_PIN_E07_GDO1)
/* E10 */
#define Ebyte_BSP_E10IrqIoRead()  GPIO_ReadInputDataBit( BSP_GPIO_PORT_E10_IRQ , BSP_GPIO_PIN_E10_IRQ)
#define Ebyte_BSP_E10Dio2Read()   GPIO_ReadInputDataBit( BSP_GPIO_PORT_E10_DIO2 , BSP_GPIO_PIN_E10_DIO2)
#define Ebyte_BSP_E10Dio3Read()   GPIO_ReadInputDataBit( BSP_GPIO_PORT_E10_DIO3 , BSP_GPIO_PIN_E10_DIO3)
#define Ebyte_BSP_E10SdnIoLow()   GPIO_WriteBit( BSP_GPIO_PORT_E10_SDN , BSP_GPIO_PIN_E10_SDN, RESET )
#define Ebyte_BSP_E10SdnIoHigh()  GPIO_WriteBit( BSP_GPIO_PORT_E10_SDN , BSP_GPIO_PIN_E10_SDN, SET )
/* E19 */
#define Ebyte_BSP_E19ResetIoLow()  GPIO_WriteBit( BSP_GPIO_PORT_E19_NRST , BSP_GPIO_PIN_E19_NRST, RESET )
#define Ebyte_BSP_E19ResetIoHigh() GPIO_WriteBit( BSP_GPIO_PORT_E19_NRST , BSP_GPIO_PIN_E19_NRST, SET )
#define Ebyte_BSP_E19TxIoDisable() GPIO_WriteBit( BSP_GPIO_PORT_E19_TXEN , BSP_GPIO_PIN_E19_TXEN, RESET )
#define Ebyte_BSP_E19TxIoEnable()  GPIO_WriteBit( BSP_GPIO_PORT_E19_TXEN , BSP_GPIO_PIN_E19_TXEN, SET )
#define Ebyte_BSP_E19RxIoDisable() GPIO_WriteBit( BSP_GPIO_PORT_E19_RXEN , BSP_GPIO_PIN_E19_RXEN, RESET )
#define Ebyte_BSP_E19RxIoEnable()  GPIO_WriteBit( BSP_GPIO_PORT_E19_RXEN , BSP_GPIO_PIN_E19_RXEN, SET ) 
#define Ebyte_BSP_E19Dio0Read()   GPIO_ReadInputDataBit( BSP_GPIO_PORT_E19_DIO0 , BSP_GPIO_PIN_E19_DIO0)
/* E31 */
#define Ebyte_BSP_E31IrqIoRead()  GPIO_ReadInputDataBit( BSP_GPIO_PORT_E31_IRQ , BSP_GPIO_PIN_E31_IRQ)
/* E49 */
#define Ebyte_BSP_E49Dio1IoRead() GPIO_ReadInputDataBit( BSP_GPIO_PORT_E49_DIO1 , BSP_GPIO_PIN_E49_DIO1) 
#define Ebyte_BSP_E49Dio2IoRead() GPIO_ReadInputDataBit( BSP_GPIO_PORT_E49_DIO2 , BSP_GPIO_PIN_E49_DIO2) 
#define Ebyte_BSP_E49FcsbIoLow()  GPIO_WriteBit( BSP_GPIO_PORT_E49_FCSB , BSP_GPIO_PIN_E49_FCSB, RESET ) 
#define Ebyte_BSP_E49FcsbIoHigh() GPIO_WriteBit( BSP_GPIO_PORT_E49_FCSB , BSP_GPIO_PIN_E49_FCSB, SET ) 
#define Ebyte_BSP_E49CsbIoLow()   GPIO_WriteBit( BSP_GPIO_PORT_E49_CSB , BSP_GPIO_PIN_E49_CSB, RESET )
#define Ebyte_BSP_E49CsbIoHigh()  GPIO_WriteBit( BSP_GPIO_PORT_E49_CSB , BSP_GPIO_PIN_E49_CSB, SET ) 
#define Ebyte_BSP_E49SlckIoLow()  GPIO_WriteBit( BSP_GPIO_PORT_E49_SLCK , BSP_GPIO_PIN_E49_SLCK, RESET ) 
#define Ebyte_BSP_E49SlckIoHigh() GPIO_WriteBit( BSP_GPIO_PORT_E49_SLCK , BSP_GPIO_PIN_E49_SLCK, SET ) 
#define Ebyte_BSP_E49SdioIoLow()  GPIO_WriteBit( BSP_GPIO_PORT_E49_SDIO , BSP_GPIO_PIN_E49_SDIO, RESET ) 
#define Ebyte_BSP_E49SdioIoHigh() GPIO_WriteBit( BSP_GPIO_PORT_E49_SDIO , BSP_GPIO_PIN_E49_SDIO, SET ) 
#define Ebyte_BSP_E49SdioIoOutput()  GPIO_Init( BSP_GPIO_PORT_E49_SDIO, BSP_GPIO_PIN_E49_SDIO, GPIO_Mode_Out_PP_High_Fast )
#define Ebyte_BSP_E49SdioIoInput()   GPIO_Init( BSP_GPIO_PORT_E49_SDIO, BSP_GPIO_PIN_E49_SDIO, GPIO_Mode_In_FL_No_IT )
#define Ebyte_BSP_E49SdioIoRead()    GPIO_ReadInputDataBit( BSP_GPIO_PORT_E49_SDIO , BSP_GPIO_PIN_E49_SDIO) 
/* ȫ���ж� */
#if defined(__CSMC__) || defined(_SDCC_)
#define Ebyte_BSP_GlobalIntEnable()               enableInterrupts()
#define Ebyte_BSP_GlobalIntDisable()              disableInterrupts()
#else
#define Ebyte_BSP_GlobalIntEnable()                __enable_interrupt()
#define Ebyte_BSP_GlobalIntDisable()               __disable_interrupt()
#endif
/* Parameter configuration UART */
#define BSP_USER_UART             USART1
#define BSP_USER_UART_CLOCK       CLK_Peripheral_USART1
#define BSP_USER_UART_BAUDRATE    9600                  // baud rate
#define BSP_USER_UART_PARITY      USART_Parity_No       // USART_Parity_No: no parity USART_Parity_Even: odd parity USART_Parity_Odd: even parity
#define BSP_USER_UART_IRQ         USART1_RX_IRQn        // Interrupt type
#define BSP_USER_UART_IRQ_LEVEL   ITC_PriorityLevel_2   // Priority


/* parameter configuration button event queue */
#define BSP_BTN_FIFO_LENGTH       16

/* Parameter configuration debugging print information to close the print needs to comment out the macro definition EBYTE_DEBUG */
#define EBYTE_DEBUG 
#ifdef __CSMC__
//kkk cosmic compiler doesn't support macros vith variable number of  parameters 
#define DEBUG mprintf
#else
#ifdef EBYTE_DEBUG
#define DEBUG(format, ...)  mprintf(format, ##__VA_ARGS__)  
#else
#define DEBUG(...)     
#endif
#endif
typedef enum { BSP_LED_1 = 0, BSP_LED_2 } BSP_LED_t;
typedef enum { OFF = 0, ON , TOGGLE} BSP_LED_Ctl_t;
typedef enum { UART_8N1 = 0, UART_8O1, UART_8E1 } BSP_UART_Parity_t;

typedef enum { BSP_BUTTON_1 = 0, BSP_BUTTON_2 } BSP_BUTTON_t;
typedef enum
{
    BTN_1_SHORT, // button 1 short press
    BTN_1_LONG,	 // Key 1 long press
    BTN_2_SHORT, // button 2 short press
    BTN_2_LONG	 // long press button 2
}BSP_BTN_EVENT_t;

typedef struct
{
    uint8_t fifoLen   ;
    uint8_t fifoRead  ;
    uint8_t fifoWrite ;
    uint8_t buff[BSP_BTN_FIFO_LENGTH];
}BSP_BTN_FIFO_t;

extern BSP_BTN_FIFO_t BSP_BTN_FIFO;

void Ebyte_BSP_Init( void );
void Ebyte_BTN_FIFO_Init(BSP_BTN_FIFO_t *fifo);
void Ebyte_BSP_DelayMs( volatile uint32_t nTime );
void Ebyte_BSP_TimerDecrement(void);
void Ebyte_BSP_LedControl( BSP_LED_t LEDx , BSP_LED_Ctl_t ctl);
void Ebyte_BSP_UartTransmit( uint8_t *buffer , uint16_t length );

uint8_t Ebyte_BSP_ReadButton( BSP_BUTTON_t btn );
uint8_t Ebyte_BSP_SpiTransAndRecv( uint8_t data );

uint8_t Ebyte_BTN_FIFO_Push(BSP_BTN_FIFO_t *fifo, BSP_BTN_EVENT_t event);
uint8_t Ebyte_BTN_FIFO_Pop(BSP_BTN_FIFO_t *fifo, BSP_BTN_EVENT_t *event);
uint32_t Ebyte_BSP_TimerGetTick(void);
