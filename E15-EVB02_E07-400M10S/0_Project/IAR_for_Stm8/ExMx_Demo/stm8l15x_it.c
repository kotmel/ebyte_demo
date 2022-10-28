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
  COMP_ClearFlag(COMP_Selection_COMP1);
}

/**
  * @brief TIM2 Update/Overflow/Trigger/Break /USART2 TX Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TIM2_UPD_OVF_TRG_BRK_USART2_TX_IRQHandler, 19)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief Timer2 Capture/Compare / USART2 RX Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TIM2_CC_USART2_RX_IRQHandler, 20)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
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
  /* 按键检测 */
  IT_Timer_ButtonCheck();
  
  /* 串口状态检测 */
  IT_Timer_UartCheck();
  
  /* 定时器延时辅助计算 */
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
  /* 首帧判断 状态机 触发定时器计时 10ms后未收到下一字节则断帧  */
  if( !Uart_isInRecvState )
  {
      Uart_isInRecvState =  1;
  }
  Uart_isContinuousRecv = 1;  

  /* 接收串口数据 1 Byte */
  temp = USART_ReceiveData8(USART1) ;
  
  /* 写入缓存队列 1 Byte */
  Ebyte_FIFO_Write( &hfifo, &temp, 1 );

  /* 清除中断标识 */
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
 * @brief 定时器中断 状态机 辅助按键检测
 */
void IT_Timer_ButtonCheck(void)
{
    /* 如果按键1被按下 */
  if( !Ebyte_BSP_ReadButton( BSP_BUTTON_1 ) )
  {
    Button1_TickCounter++;
  }
  else
  {
    if( Button1_TickCounter > 1000 )  // 1秒 长按
    {
       Ebyte_BTN_FIFO_Push( &BSP_BTN_FIFO, BTN_1_LONG);
    }
    else if( Button1_TickCounter > 50 ) //50毫秒 短按
    {
       Ebyte_BTN_FIFO_Push( &BSP_BTN_FIFO, BTN_1_SHORT);
    }
    else {} //50毫秒以下  认为是抖动 不操作
    
    Button1_TickCounter=0;   
  }
  
  /* 如果按键2被按下 */
  if( !Ebyte_BSP_ReadButton( BSP_BUTTON_2 ) )
  {
    Button2_TickCounter++;
  }
  else
  {
    if( Button2_TickCounter > 1000 )  // 1秒 长按
    {
       Ebyte_BTN_FIFO_Push( &BSP_BTN_FIFO, BTN_2_LONG);
    }
    else if( Button2_TickCounter > 50 ) //50毫秒 短按
    {
       Ebyte_BTN_FIFO_Push( &BSP_BTN_FIFO, BTN_2_SHORT);
    }
    else {} //50毫秒以下  认为是抖动 不操作
    
    Button2_TickCounter=0;   
  }    
}

/* !
 * @brief 定时器中断 状态机 辅助时间断帧
 */
void IT_Timer_UartCheck(void)
{
  /* 串口接收到第一字节起就开始计时 */
  if( Uart_isInRecvState )
  {
     Uart_TickCounter++;
  
     /* 超过10ms没有接收到第二字节 就认为断帧 */
     if( Uart_TickCounter > 10 )
     {
        /* 通知主函数接收到一帧 */
        Uart_isRecvReady ++;
        /* 停止计时 */
        Uart_isInRecvState = 0;
        Uart_TickCounter = 0;
     }
     
     /* 复位FIFO超时检测 */
     FIFO_TickCounter = 0;
  }
  else
  {
      /* 如果在串口没有接收数据时 还存在没有发送完的帧 500ms后超时检测成立 清算FIFO中的数据 */
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
  
  /* 串口每接收到1个字节就重新计数 */
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