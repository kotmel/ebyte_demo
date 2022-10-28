/*	BASIC INTERRUPT VECTOR TABLE FOR STM8 devices
 *	Copyright (c) 2007 STMicroelectronics
 */


#include "stm8l15x_it.h"

typedef void @far (*interrupt_handler_t)(void);

struct interrupt_vector {
	unsigned char interrupt_instruction;
	interrupt_handler_t interrupt_handler;
};

#if 0
@far @interrupt void NonHandledInterrupt (void)
{
	/* in order to detect unexpected events during development, 
	   it is recommended to set a breakpoint on the following instruction
	*/
	return;
}
#endif

@far @interrupt void EXTI2_IRQHandler (void);
@far @interrupt void EXTI3_IRQHandler (void);
@far @interrupt void EXTI4_IRQHandler (void);
@far @interrupt void EXTI5_IRQHandler (void);
@far @interrupt void ADC1_COMP_IRQHandler (void);
@far @interrupt void TIM3_UPD_OVF_TRG_BRK_USART3_TX_IRQHandler (void);
@far @interrupt void USART1_RX_TIM5_CC_IRQHandler (void);


extern void _stext();     /* startup routine */


struct interrupt_vector const _vectab[] = {
	{0x82, (interrupt_handler_t)_stext}, /* reset */
	{0x82, NonHandledInterrupt}, /* trap  */
	{0x82, NonHandledInterrupt}, /* irq0  */// reserved
	{0x82, NonHandledInterrupt}, /* irq1  */
	{0x82, NonHandledInterrupt}, /* irq2  */
	{0x82, NonHandledInterrupt}, /* irq3  */
	{0x82, NonHandledInterrupt}, /* irq4  */
	{0x82, NonHandledInterrupt}, /* irq5  */
	{0x82, NonHandledInterrupt}, /* irq6  */
	{0x82, NonHandledInterrupt}, /* irq7  */
	{0x82, NonHandledInterrupt}, /* irq8  */
	{0x82, NonHandledInterrupt}, /* irq9  */
	{0x82, EXTI2_IRQHandler}, /* irq10 */
	{0x82, EXTI3_IRQHandler}, /* irq11 */
	{0x82, EXTI4_IRQHandler}, /* irq12 */
	{0x82, EXTI5_IRQHandler}, /* irq13 */
	{0x82, NonHandledInterrupt}, /* irq14 */
	{0x82, NonHandledInterrupt}, /* irq15 */
	{0x82, NonHandledInterrupt}, /* irq16 */
	{0x82, NonHandledInterrupt}, /* irq17 */
	{0x82, ADC1_COMP_IRQHandler}, /* irq18 */
	{0x82, NonHandledInterrupt}, /* irq19 */
	{0x82, NonHandledInterrupt}, /* irq20 */
	{0x82, TIM3_UPD_OVF_TRG_BRK_USART3_TX_IRQHandler}, /* irq21 */
	{0x82, NonHandledInterrupt}, /* irq22 */
	{0x82, NonHandledInterrupt}, /* irq23 */
	{0x82, NonHandledInterrupt}, /* irq24 */
	{0x82, NonHandledInterrupt}, /* irq25 */
	{0x82, NonHandledInterrupt}, /* irq26 */
	{0x82, NonHandledInterrupt}, /* irq27 */
	{0x82, USART1_RX_TIM5_CC_IRQHandler}, /* irq28 */
	{0x82, NonHandledInterrupt}, /* irq29 */
};

