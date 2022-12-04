/**
  ******************************************************************************
  * @file    stm8l15x_it.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    09/28/2010
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all peripherals interrupt service routine.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */

/* Includes ------------------------------------------------------------------*/
#include "stm8l15x_it.h"
#include "ebyte_kfifo.h"
#include "ebyte_core.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint8_t Uart_isInRecvState = 0;
static uint8_t Uart_isContinuousRecv = 0;
static uint32_t Uart_TickCounter = 0;
extern uint8_t Uart_isRecvReady;

static uint16_t Button1_TickCounter = 0;
static uint16_t Button2_TickCounter = 0;

extern Ebyte_FIFO_t hfifo;
extern uint8_t FIFO_isTimeCheckReady;
static uint32_t FIFO_TickCounter = 0;

extern 

/* Private function prototypes -----------------------------------------------*/
void IT_Timer_ButtonCheck(void);
void IT_Timer_UartCheck(void);
/* Private functions ---------------------------------------------------------*/

/** @addtogroup IT_Functions
  * @{
  */

#ifdef _COSMIC_
/**
  * @brief Dummy interrupt routine
  * @par Parameters:
  * None
  * @retval
  * None
*/
INTERRUPT_HANDLER(NonHandledInterrupt, 0)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}
#endif

/**
  * @brief TRAP interrupt routine
  * @par Parameters:
  * None
  * @retval
  * None
*/
INTERRUPT_HANDLER_TRAP(TRAP_IRQHandler)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief FLASH Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(FLASH_IRQHandler, 1)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief DMA1 channel0 and channel1 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(DMA1_CHANNEL0_1_IRQHandler, 2)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief DMA1 channel2 and channel3 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(DMA1_CHANNEL2_3_IRQHandler, 3)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief RTC / CSS_LSE Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(RTC_CSSLSE_IRQHandler, 4)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief External IT PORTE/F and PVD Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTIE_F_PVD_IRQHandler, 5)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief External IT PORTB / PORTG Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTIB_G_IRQHandler, 6)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief External IT PORTD /PORTH Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTID_H_IRQHandler, 7)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

volatile int received;

/**
  * @brief External IT PIN0 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI0_IRQHandler, 8)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  if (GPIO_ReadInputDataBit( BSP_GPIO_PORT_E07_GDO0 , BSP_GPIO_PIN_E07_GDO0)) {
    GPIO_WriteBit(DEBUG_Port, DEBUG_Pin0, SET);
  } else {
    GPIO_WriteBit(DEBUG_Port, DEBUG_Pin0, RESET);
  }
  GPIO_ToggleBits(DEBUG_Port, DEBUG_Pin1);
  Ebyte_RF.TaskForIRQ();
  EXTI_ClearITPendingBit(EXTI_IT_Pin0);
  //received = 1;

}

/**
  * @brief External IT PIN1 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI1_IRQHandler, 9)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief External IT PIN2 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI2_IRQHandler, 10)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
    Ebyte_RF.TaskForIRQ();
    EXTI_ClearITPendingBit(EXTI_IT_Pin2);
}

/**
  * @brief External IT PIN3 Interrupt routine.
  * @param  None
  * @retval None
  */

INTERRUPT_HANDLER(EXTI3_IRQHandler, 11)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
    EXTI_ClearITPendingBit(EXTI_IT_Pin3);  
}

/**
  * @brief External IT PIN4 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI4_IRQHandler, 12)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  EXTI_ClearITPendingBit(EXTI_IT_Pin4);  
}

/**
  * @brief External IT PIN5 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI5_IRQHandler, 13)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
    EXTI_ClearITPendingBit(EXTI_IT_Pin5);
}

/**
  * @brief External IT PIN6 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI6_IRQHandler, 14)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief External IT PIN7 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI7_IRQHandler, 15)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief LCD /AES Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(LCD_AES_IRQHandler, 16)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief CLK switch/CSS/TIM1 break Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(SWITCH_CSS_BREAK_DAC_IRQHandler, 17)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief ADC1/Comparator Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(ADC1_COMP_IRQHandler, 18)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  /* Clear the comparator1 event flag */
  //COMP_ClearFlag(COMP_Selection_COMP1);
}

/**
  * @brief TIM2 Update/Overflow/Trigger/Break /USART2 TX Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TIM2_UPD_OVF_TRG_BRK_USART2_TX_IRQHandler, 19)
{
  // autoreload register - full pulse period
  GPIO_ToggleBits( BSP_GPIO_PORT_LED_1, BSP_GPIO_PIN_LED_1 );
  TIM2_ClearITPendingBit(TIM2_IT_Update);
   
}

/**
  * @brief Timer2 Capture/Compare / USART2 RX Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TIM2_CC_USART2_RX_IRQHandler, 20)
{
   // capture/compare reg - half pulse
   GPIO_ToggleBits( BSP_GPIO_PORT_LED_1, BSP_GPIO_PIN_LED_1 );
   TIM2_ClearITPendingBit(TIM2_IT_CC1);
}

/**
  * @brief Timer3 Update/Overflow/Trigger/Break Interrupt routine.
  * @param  None
  * @retval None
  */
#if defined(__CSMC__)	
INTERRUPT_HANDLER(@svlreg TIM3_UPD_OVF_TRG_BRK_USART3_TX_IRQHandler, 21)
#else
INTERRUPT_HANDLER(TIM3_UPD_OVF_TRG_BRK_USART3_TX_IRQHandler, 21)
#endif
{
   /* key detection */
  IT_Timer_ButtonCheck();
  
   /* Serial port status detection */
  IT_Timer_UartCheck();
  
  /* timer delay auxiliary calculation */
  Ebyte_BSP_TimerDecrement();
  
  TIM3_ClearITPendingBit(TIM3_IT_Update);
}

/**
  * @brief Timer3 Capture/Compare /USART3 RX Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TIM3_CC_USART3_RX_IRQHandler, 22)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief TIM1 Update/Overflow/Trigger/Commutation Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TIM1_UPD_OVF_TRG_COM_IRQHandler, 23)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief TIM1 Capture/Compare Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TIM1_CC_IRQHandler, 24)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief TIM4 Update/Overflow/Trigger Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TIM4_UPD_OVF_TRG_IRQHandler, 25)
{

}

/**
  * @brief SPI1 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(SPI1_IRQHandler, 26)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief USART1 TX / TIM5 Update/Overflow/Trigger/Break Interrupt  routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(USART1_TX_TIM5_UPD_OVF_TRG_BRK_IRQHandler, 27)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief USART1 RX / Timer5 Capture/Compare Interrupt routine.
  * @param  None
  * @retval None
  */
#if defined(__CSMC__)	
INTERRUPT_HANDLER(  @svlreg USART1_RX_TIM5_CC_IRQHandler, 28)
#else
INTERRUPT_HANDLER(USART1_RX_TIM5_CC_IRQHandler, 28)
#endif
{
  uint8_t temp;
   /* The first frame judges the state machine to trigger the timer to count for 10ms, then the frame will be broken if the next byte is not received */
  if( !Uart_isInRecvState )
  {
      Uart_isInRecvState =  1;
  }
  Uart_isContinuousRecv = 1;  

  /* Receive serial port data 1 Byte */
  temp = USART_ReceiveData8(USART1) ;
  
  /* Write to buffer queue 1 Byte */
  Ebyte_FIFO_Write( &hfifo, &temp, 1 );

  /* Clear the interrupt flag */
  USART_ClearITPendingBit( USART1, USART_IT_RXNE );  
}

/**
  * @brief I2C1 / SPI2 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(I2C1_SPI2_IRQHandler, 29)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */

}

/* !
* @brief timer interrupt state machine auxiliary button detection
 */
void IT_Timer_ButtonCheck(void)
{
     /* If button 1 is pressed */
  if( !Ebyte_BSP_ReadButton( BSP_BUTTON_1 ) )
  {
    Button1_TickCounter++;
  }
  else
  {
    if( Button1_TickCounter > 1000 ) // 1 second long press
    {
       Ebyte_BTN_FIFO_Push( &BSP_BTN_FIFO, BTN_1_LONG);
    }
    else if( Button1_TickCounter > 50 ) // 50ms short press
    {
       Ebyte_BTN_FIFO_Push( &BSP_BTN_FIFO, BTN_1_SHORT);
    }
    else {} // 50 milliseconds or less is considered to be jittering and does not operate
    
    Button1_TickCounter=0;   
  }
  
  /* If button 2 is pressed */
  if( !Ebyte_BSP_ReadButton( BSP_BUTTON_2 ) )
  {
    Button2_TickCounter++;
  }
  else
  {
    if( Button2_TickCounter > 1000 )  // 1 second long press
    {
       Ebyte_BTN_FIFO_Push( &BSP_BTN_FIFO, BTN_2_LONG);
    }
    else if( Button2_TickCounter > 50 ) // 50ms short press
    {
       Ebyte_BTN_FIFO_Push( &BSP_BTN_FIFO, BTN_2_SHORT);
    }
    else {} // 50 milliseconds or less is considered to be jittering and does not operate
    
    Button2_TickCounter=0;   
  }    
}

/* !
* @brief Timer interrupt state machine auxiliary time frame
 */
void IT_Timer_UartCheck(void)
{
   /* The serial port starts counting when the first byte is received */
  if( Uart_isInRecvState )
  {
     Uart_TickCounter++;
  
     /* If the second byte is not received for more than 10ms, the frame is considered broken */
     if( Uart_TickCounter > 10 )
     {
        /* Notify the main function to receive a frame */
        Uart_isRecvReady ++;
        /* stop timing */
        Uart_isInRecvState = 0;
        Uart_TickCounter = 0;
     }
     
      /* Reset FIFO timeout detection */
     FIFO_TickCounter = 0;
  }
  else
  {
        /* If there are unsent frames when the serial port does not receive data, the timeout detection is established
        after 500ms to clear the data in the FIFO */
      if(  (!Uart_isInRecvState)  &&  Uart_isRecvReady )
      {
          FIFO_TickCounter++;
          if( FIFO_TickCounter > 500)
          {
              FIFO_isTimeCheckReady=1;
              Uart_isRecvReady = 0;
              FIFO_TickCounter = 0;
          }
      }
  }
  
  /* The serial port recounts every time it receives 1 byte */
  if( Uart_isInRecvState && Uart_isContinuousRecv )
  {
    Uart_TickCounter = 0;
    Uart_isContinuousRecv = 0;
  }
}

/**
  * @}
  */
/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/