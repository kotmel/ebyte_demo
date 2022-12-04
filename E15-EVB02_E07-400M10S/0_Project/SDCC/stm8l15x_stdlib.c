
#include "board.h"


// copy from stm8l15x_clk.c for size opt.
CONST uint8_t SYSDivFactor[5] = {1, 2, 4, 8, 16}; /*!< Holds the different Master clock Divider factors */
uint32_t CLK_GetClockFreq(void)
{
  uint32_t clockfrequency = 0;
  uint32_t sourcefrequency = 0;
  CLK_SYSCLKSource_TypeDef clocksource = CLK_SYSCLKSource_HSI;
  uint8_t tmp = 0, presc = 0;

  /* Get SYSCLK source. */
  clocksource = (CLK_SYSCLKSource_TypeDef)CLK->SCSR;

  if ( clocksource == CLK_SYSCLKSource_HSI)
  {
    sourcefrequency = HSI_VALUE;
  }
  else if ( clocksource == CLK_SYSCLKSource_LSI)
  {
    sourcefrequency = LSI_VALUE;
  }
  else if ( clocksource == CLK_SYSCLKSource_HSE)
  {
    sourcefrequency = HSE_VALUE;
  }
  else
  {
    clockfrequency = LSE_VALUE;
  }

  /* Get System clock divider factor*/
  tmp = (uint8_t)(CLK->CKDIVR & CLK_CKDIVR_CKM);
  presc = SYSDivFactor[tmp];

  /* Get System clock frequency */
  clockfrequency = sourcefrequency / presc;

  return((uint32_t)clockfrequency);
}

// copy from stm8l15x_clk.c for size opt.
void CLK_PeripheralClockConfig(CLK_Peripheral_TypeDef CLK_Peripheral, FunctionalState NewState)
{
  uint8_t reg = 0;

  /* Check the parameters */
  assert_param(IS_CLK_PERIPHERAL(CLK_Peripheral));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* get flag register */
  reg = (uint8_t)((uint8_t)CLK_Peripheral & (uint8_t)0xF0);

  if ( reg == 0x00)
  {
    if (NewState != DISABLE)
    {
      /* Enable the peripheral Clock */
      CLK->PCKENR1 |= (uint8_t)((uint8_t)1 << ((uint8_t)CLK_Peripheral & (uint8_t)0x0F));
    }
    else
    {
      /* Disable the peripheral Clock */
      CLK->PCKENR1 &= (uint8_t)(~(uint8_t)(((uint8_t)1 << ((uint8_t)CLK_Peripheral & (uint8_t)0x0F))));
    }
  }
  else if (reg == 0x10)
  {
    if (NewState != DISABLE)
    {
      /* Enable the peripheral Clock */
      CLK->PCKENR2 |= (uint8_t)((uint8_t)1 << ((uint8_t)CLK_Peripheral & (uint8_t)0x0F));
    }
    else
    {
      /* Disable the peripheral Clock */
      CLK->PCKENR2 &= (uint8_t)(~(uint8_t)(((uint8_t)1 << ((uint8_t)CLK_Peripheral & (uint8_t)0x0F))));
    }
  }
  else
  {
    if (NewState != DISABLE)
    {
      /* Enable the peripheral Clock */
      CLK->PCKENR3 |= (uint8_t)((uint8_t)1 << ((uint8_t)CLK_Peripheral & (uint8_t)0x0F));
    }
    else
    {
      /* Disable the peripheral Clock */
      CLK->PCKENR3 &= (uint8_t)(~(uint8_t)(((uint8_t)1 << ((uint8_t)CLK_Peripheral & (uint8_t)0x0F))));
    }
  }
}

void GPIO_Init(GPIO_TypeDef* GPIOx, uint8_t GPIO_Pin, GPIO_Mode_TypeDef GPIO_Mode)
{
  /*----------------------*/
  /* Check the parameters */
  /*----------------------*/

  assert_param(IS_GPIO_MODE(GPIO_Mode));
  assert_param(IS_GPIO_PIN(GPIO_Pin));

  /* Reset corresponding bit to GPIO_Pin in CR2 register */
  GPIOx->CR2 &= (uint8_t)(~(GPIO_Pin));

  /*-----------------------------*/
  /* Input/Output mode selection */
  /*-----------------------------*/

  if ((((uint8_t)(GPIO_Mode)) & (uint8_t)0x80) != (uint8_t)0x00) /* Output mode */
  {
    if ((((uint8_t)(GPIO_Mode)) & (uint8_t)0x10) != (uint8_t)0x00) /* High level */
    {
      GPIOx->ODR |= GPIO_Pin;
    } else /* Low level */
    {
      GPIOx->ODR &= (uint8_t)(~(GPIO_Pin));
    }
    /* Set Output mode */
    GPIOx->DDR |= GPIO_Pin;
  } else /* Input mode */
  {
    /* Set Input mode */
    GPIOx->DDR &= (uint8_t)(~(GPIO_Pin));
  }

  /*------------------------------------------------------------------------*/
  /* Pull-Up/Float (Input) or Push-Pull/Open-Drain (Output) modes selection */
  /*------------------------------------------------------------------------*/

  if ((((uint8_t)(GPIO_Mode)) & (uint8_t)0x40) != (uint8_t)0x00) /* Pull-Up or Push-Pull */
  {
    GPIOx->CR1 |= GPIO_Pin;
  } else /* Float or Open-Drain */
  {
    GPIOx->CR1 &= (uint8_t)(~(GPIO_Pin));
  }

  /*-----------------------------------------------------*/
  /* Interrupt (Input) or Slope (Output) modes selection */
  /*-----------------------------------------------------*/

  if ((((uint8_t)(GPIO_Mode)) & (uint8_t)0x20) != (uint8_t)0x00) /* Interrupt or Slow slope */
  {
    GPIOx->CR2 |= GPIO_Pin;
  } else /* No external interrupt or No slope control */
  {
    GPIOx->CR2 &= (uint8_t)(~(GPIO_Pin));
  }

}

BitStatus GPIO_ReadInputDataBit(GPIO_TypeDef* GPIOx, GPIO_Pin_TypeDef GPIO_Pin)
{
  return ((BitStatus)(GPIOx->IDR & (uint8_t)GPIO_Pin));
}

void GPIO_WriteBit(GPIO_TypeDef* GPIOx, GPIO_Pin_TypeDef GPIO_Pin, BitAction GPIO_BitVal)
{
  /* Check the parameters */
  assert_param(IS_GPIO_PIN(GPIO_Pin));
  assert_param(IS_STATE_VALUE(GPIO_BitVal));

  if (GPIO_BitVal != RESET)
  {
    GPIOx->ODR |= GPIO_Pin;

  }
  else
  {
    GPIOx->ODR &= (uint8_t)(~GPIO_Pin);
  }
}

void GPIO_ToggleBits(GPIO_TypeDef* GPIOx, uint8_t GPIO_Pin)
{
  GPIOx->ODR ^= GPIO_Pin;
}

void GPIO_ExternalPullUpConfig(GPIO_TypeDef* GPIOx, uint8_t GPIO_Pin, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_GPIO_PIN(GPIO_Pin));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE) /* External Pull-Up Set*/
  {
    GPIOx->CR1 |= GPIO_Pin;
  } else /* External Pull-Up Reset*/
  {
    GPIOx->CR1 &= (uint8_t)(~(GPIO_Pin));
  }
}

void SYSCFG_REMAPPinConfig(REMAP_Pin_TypeDef REMAP_Pin, FunctionalState NewState)
{
  uint8_t regindex = 0;
  /* Check the parameters */
  assert_param(IS_REMAP_PIN(REMAP_Pin));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Read register index */
  regindex = (uint8_t) ((uint16_t) REMAP_Pin >> 8);

  /* Check if REMAP_Pin is in RMPCR1 register */
  if (regindex == 0x01)
  {
    SYSCFG->RMPCR1 &= (uint8_t)((uint8_t)((uint8_t)REMAP_Pin << 4) | (uint8_t)0x0F);
    if (NewState != DISABLE)
    {
      SYSCFG->RMPCR1 |= (uint8_t)((uint16_t)REMAP_Pin & (uint16_t)0x00F0);
    }
  }
  /* Check if REMAP_Pin is in RMPCR2 register */
  else if (regindex == 0x02)
  {
    if (NewState != DISABLE)
    {
      SYSCFG->RMPCR2 |= (uint8_t) REMAP_Pin;
    }
    else
    {
      SYSCFG->RMPCR2 &= (uint8_t)((uint16_t)(~(uint16_t)REMAP_Pin));
    }
  }
  /* REMAP_Pin is in RMPCR3 register */
  else
  {
    if (NewState != DISABLE)
    {
      SYSCFG->RMPCR3 |= (uint8_t) REMAP_Pin;
    }
    else
    {
      SYSCFG->RMPCR3 &= (uint8_t)((uint16_t)(~(uint16_t)REMAP_Pin));
    }
  }
}

void SPI_Init(SPI_TypeDef* SPIx, SPI_FirstBit_TypeDef SPI_FirstBit,
              SPI_BaudRatePrescaler_TypeDef SPI_BaudRatePrescaler,
              SPI_Mode_TypeDef SPI_Mode, SPI_CPOL_TypeDef SPI_CPOL,
              SPI_CPHA_TypeDef SPI_CPHA, SPI_DirectionMode_TypeDef SPI_Data_Direction,
              SPI_NSS_TypeDef SPI_Slave_Management, uint8_t CRCPolynomial)
{
  /* Check structure elements */
  assert_param(IS_SPI_FIRSTBIT(SPI_FirstBit));
  assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
  assert_param(IS_SPI_MODE(SPI_Mode));
  assert_param(IS_SPI_POLARITY(SPI_CPOL));
  assert_param(IS_SPI_PHASE(SPI_CPHA));
  assert_param(IS_SPI_DATA_DIRECTION(SPI_Data_Direction));
  assert_param(IS_SPI_SLAVEMANAGEMENT(SPI_Slave_Management));
  assert_param(IS_SPI_CRC_POLYNOMIAL(CRCPolynomial));

  /* Frame Format, BaudRate, Clock Polarity and Phase configuration */
  SPIx->CR1 = (uint8_t)((uint8_t)((uint8_t)SPI_FirstBit |
                                  (uint8_t)SPI_BaudRatePrescaler) |
                        (uint8_t)((uint8_t)SPI_CPOL |
                                  SPI_CPHA));

  /* Data direction configuration: BDM, BDOE and RXONLY bits */
  SPIx->CR2 = (uint8_t)((uint8_t)(SPI_Data_Direction) | (uint8_t)(SPI_Slave_Management));

  if (SPI_Mode == SPI_Mode_Master)
  {
    SPIx->CR2 |= (uint8_t)SPI_CR2_SSI;
  }
  else
  {
    SPIx->CR2 &= (uint8_t)~(SPI_CR2_SSI);
  }

  /* Master/Slave mode configuration */
  SPIx->CR1 |= (uint8_t)(SPI_Mode);

  /* CRC configuration */
  SPIx->CRCPR = (uint8_t)CRCPolynomial;
}

void SPI_Cmd(SPI_TypeDef* SPIx, FunctionalState NewState)
{
  /* Check function parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    SPIx->CR1 |= SPI_CR1_SPE; /* Enable the SPI peripheral*/
  }
  else
  {
    SPIx->CR1 &= (uint8_t)(~SPI_CR1_SPE); /* Disable the SPI peripheral*/
  }
}

void EXTI_ClearITPendingBit(EXTI_IT_TypeDef EXTI_IT)
{
  uint16_t tempvalue = 0;

  /* Check function parameters */
  assert_param(IS_EXTI_ITPENDINGBIT(EXTI_IT));

  tempvalue = ((uint16_t)EXTI_IT & (uint16_t)0xFF00);

  if ( tempvalue == 0x0100)
  {
    EXTI->SR2 = (uint8_t)((uint16_t)EXTI_IT & (uint16_t)0x00FF);
  }
  else
  {
    EXTI->SR1 = (uint8_t) (EXTI_IT);
  }
}

/**
  * @brief  Enables or disables the specified USART interrupts.
  * @param  USARTx: where x can be 1 to select the specified USART peripheral.
  * @param  USART_IT specifies the USART interrupt sources to be enabled or disabled.
  *         This parameter can be one of the following values:
  *            @arg USART_IT_TXE: Transmit Data Register empty interrupt
  *            @arg USART_IT_TC: Transmission complete interrupt
  *            @arg USART_IT_RXNE: Receive Data register not empty interrupt
  *            @arg USART_IT_OR: Overrun error interrupt
  *            @arg USART_IT_IDLE: Idle line detection interrupt
  *            @arg USART_IT_ERR: Error interrupt
  * @param  NewState new state of the specified USART interrupts.
    *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void USART_ITConfig(USART_TypeDef* USARTx, USART_IT_TypeDef USART_IT, FunctionalState NewState)
{
  uint8_t usartreg, itpos = 0x00;
  assert_param(IS_USART_CONFIG_IT(USART_IT));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Get the USART register index */
  usartreg = (uint8_t)((uint16_t)USART_IT >> 0x08);
  /* Get the USART IT index */
  itpos = (uint8_t)((uint8_t)1 << (uint8_t)((uint8_t)USART_IT & (uint8_t)0x0F));

  if (NewState != DISABLE)
  {
    /**< Enable the Interrupt bits according to USART_IT mask */
    if (usartreg == 0x01)
    {
      USARTx->CR1 |= itpos;
    }
    else if (usartreg == 0x05)
    {
      USARTx->CR5 |= itpos;
    }
    /*uartreg =0x02*/
    else
    {
      USARTx->CR2 |= itpos;
    }
  }
  else
  {
    /**< Disable the interrupt bits according to USART_IT mask */
    if (usartreg == 0x01)
    {
      USARTx->CR1 &= (uint8_t)(~itpos);
    }
    else if (usartreg == 0x05)
    {
      USARTx->CR5 &= (uint8_t)(~itpos);
    }
    /*uartreg =0x02*/
    else
    {
      USARTx->CR2 &= (uint8_t)(~itpos);
    }
  }
}

/**
  * @brief  Returns the most recent received data by the USART peripheral.
  * @param  USARTx: where x can be 1 to select the specified USART peripheral.
  * @retval The received data.
  */
uint8_t USART_ReceiveData8(USART_TypeDef* USARTx)
{
  return USARTx->DR;
}

/**
  * @brief  Transmits 8 bit data through the USART peripheral.
  * @param  Data: The data to transmit.
  * @retval None
  */
void USART_SendData8(USART_TypeDef* USARTx, uint8_t Data)
{
  /* Transmit Data */
  USARTx->DR = Data;
}


/**
  * @brief  Enable the USART peripheral.
  * @param  USARTx: where x can be 1 to select the specified USART peripheral.
  * @param  NewState: The new state of the USART Communication.
    *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void USART_Cmd(USART_TypeDef* USARTx, FunctionalState NewState)
{
  if (NewState != DISABLE)
  {
    USARTx->CR1 &= (uint8_t)(~USART_CR1_USARTD); /**< USART Enable */
  }
  else
  {
    USARTx->CR1 |= USART_CR1_USARTD;  /**< USART Disable (for low power consumption) */
  }
}


/**
  * @brief  Initializes the USART according to the specified parameters.
  * @note   Configure in Push Pull or Open Drain mode the Tx pin by setting the
  *         correct I/O Port register according the product package and line
  *         configuration
  * @param  USARTx: where x can be 1 to select the specified USART peripheral.
  * @param  BaudRate: The baudrate.
  * @param  USART_WordLength: the word length
  *           This parameter can be one of the following values:
  *            @arg USART_WordLength_8b: 8 bits Data
  *            @arg USART_WordLength_9b: 9 bits Data  
  * @param  USART_StopBits: Stop Bit
  *           This parameter can be one of the following values:
  *            @arg USART_StopBits_1: One stop bit is transmitted at the end of frame
  *            @arg USART_StopBits_2: Two stop bits are transmitted at the end of frame
  *            @arg USART_StopBits_1_5: One and half stop bits
  * @param  USART_Parity: Parity
  *           This parameter can be one of the following values:
  *            @arg USART_Parity_No: No Parity
  *            @arg USART_Parity_Even: Even Parity
  *            @arg USART_Parity_Odd: Odd Parity    
  * @param  USART_Mode: Mode
  *           This parameter can be one of the following values:
  *            @arg USART_Mode_Rx: Receive Enable
  *            @arg USART_Mode_Tx: Transmit Enable  
  * @retval None
  */
void USART_Init(USART_TypeDef* USARTx, uint32_t BaudRate, USART_WordLength_TypeDef
                USART_WordLength, USART_StopBits_TypeDef USART_StopBits,
                USART_Parity_TypeDef USART_Parity,  USART_Mode_TypeDef USART_Mode)
{
  uint32_t BaudRate_Mantissa = 0;

  /* Check the parameters */
  assert_param(IS_USART_BAUDRATE(BaudRate));

  assert_param(IS_USART_WORDLENGTH(USART_WordLength));

  assert_param(IS_USART_STOPBITS(USART_StopBits));

  assert_param(IS_USART_PARITY(USART_Parity));

  assert_param(IS_USART_MODE(USART_Mode));

  /* Clear the word length and Parity Control bits */
  USARTx->CR1 &= (uint8_t)(~(USART_CR1_PCEN | USART_CR1_PS | USART_CR1_M));
  /* Set the word length bit according to USART_WordLength value */
  /* Set the Parity Control bit to USART_Parity value */
  USARTx->CR1 |= (uint8_t)((uint8_t)USART_WordLength | (uint8_t)USART_Parity);

  /* Clear the STOP bits */
  USARTx->CR3 &= (uint8_t)(~USART_CR3_STOP);
  /* Set the STOP bits number according to USART_StopBits value */
  USARTx->CR3 |= (uint8_t)USART_StopBits;

  /* Clear the LSB mantissa of USARTDIV */
  USARTx->BRR1 &= (uint8_t)(~USART_BRR1_DIVM);
  /* Clear the MSB mantissa of USARTDIV */
  USARTx->BRR2 &= (uint8_t)(~USART_BRR2_DIVM);
  /* Clear the Fraction bits of USARTDIV */
  USARTx->BRR2 &= (uint8_t)(~USART_BRR2_DIVF);

  BaudRate_Mantissa  = (uint32_t)(CLK_GetClockFreq() / BaudRate );
  /* Set the fraction of USARTDIV */
  USARTx->BRR2 = (uint8_t)((BaudRate_Mantissa >> (uint8_t)8) & (uint8_t)0xF0);
  /* Set the MSB mantissa of USARTDIV */
  USARTx->BRR2 |= (uint8_t)(BaudRate_Mantissa & (uint8_t)0x0F);
  /* Set the LSB mantissa of USARTDIV */
  USARTx->BRR1 = (uint8_t)(BaudRate_Mantissa >> (uint8_t)4);

  /* Disable the Transmitter and Receiver */
  USARTx->CR2 &= (uint8_t)~(USART_CR2_TEN | USART_CR2_REN);
  /* Set TEN and REN bits according to USART_Mode value */
  USARTx->CR2 |= (uint8_t)USART_Mode;
}


/**
  * @brief  Checks whether the specified USART flag is set or not.
  * @param  USARTx: Select the USARTx peripheral.
  * @param  USART_FLAG specifies the flag to check.
  *         This parameter can be one of the following values:
  *            @arg USART_FLAG_TXE: Transmit Data Register empty
  *            @arg USART_FLAG_TC: Transmission Complete 
  *            @arg USART_FLAG_RXNE: Read Data Register Not Empty
  *            @arg USART_FLAG_IDLE: Idle line detected
  *            @arg USART_FLAG_OR: OverRun error
  *            @arg USART_FLAG_NF: Noise error
  *            @arg USART_FLAG_FE: Framing Error
  *            @arg USART_FLAG_PE: Parity Error
  *            @arg USART_FLAG_SBK: Send Break characters               
  * @retval FlagStatus (SET or RESET)
  */
FlagStatus USART_GetFlagStatus(USART_TypeDef* USARTx, USART_FLAG_TypeDef USART_FLAG)
{
  FlagStatus status = RESET;

  /* Check parameters */
  assert_param(IS_USART_FLAG(USART_FLAG));

  if (USART_FLAG == USART_FLAG_SBK)
  {
    if ((USARTx->CR2 & (uint8_t)USART_FLAG) != (uint8_t)0x00)
    {
      /* USART_FLAG is set*/
      status = SET;
    }
    else
    {
      /* USART_FLAG is reset*/
      status = RESET;
    }
  }
  else
  {
    if ((USARTx->SR & (uint8_t)USART_FLAG) != (uint8_t)0x00)
    {
      /* USART_FLAG is set*/
      status = SET;
    }
    else
    {
      /* USART_FLAG is reset*/
      status = RESET;
    }
  }
  /* Return the USART_FLAG status*/
  return status;
}

/**
  * @brief  Clears the USARTx�s interrupt pending bits.
  * @param  USARTx: where x can be 1 to select the specified USART peripheral.
  * @param  USART_IT: specifies the interrupt pending bit to clear.
  *         This parameter can be one of the following values:
  *            @arg USART_IT_TC: Transmission complete interrupt.
  * @note   PE (Parity error), FE (Framing error), NE (Noise error),
  *         OR (OverRun error) and IDLE (Idle line detected) pending bits are
  *         cleared by software sequence: a read operation to USART_SR register
  *         (USART_GetITStatus()) followed by a read operation to USART_DR
  *         register (USART_ReceiveData8() or USART_ReceiveData9()).
  * @note   RXNE pending bit can be also cleared by a read to the USART_DR register
  *         (USART_ReceiveData8() or USART_ReceiveData9()).
  * @note   TC (Transmit complete) pending bit can be also cleared by software
  *         sequence: a read operation to USART_SR register (USART_GetITStatus())
  *         followed by a write operation to USART_DR register (USART_SendData8()
  *         or USART_SendData9()).
  * @note   TXE pending bit is cleared only by a write to the USART_DR register
  *         (USART_SendData8() or USART_SendData9()).
  * @retval None
  */
void USART_ClearITPendingBit(USART_TypeDef* USARTx, USART_IT_TypeDef USART_IT)
{
  assert_param(IS_USART_CLEAR_IT(USART_IT));

  /*< Clear RXNE or TC pending bit */
  USARTx->SR &= (uint8_t)(~USART_SR_TC);
}


/**
  * @brief  Sets the external interrupt sensitivity of the selected pin.
  * @note   Global interrupts must be disabled before calling this function.
  * @note   The modification of external interrupt sensitivity is only possible
  *         when he interrupts are disabled.
  * @note   The normal behavior is to disable the interrupts before calling this
  *         function, and re-enable them after.
  * @param  EXTI_Pin : The pin to configure.
  *          This parameter can be one of the following values:
  *            @arg EXTI_Pin_0: GPIO Pin 0
  *            @arg EXTI_Pin_1: GPIO Pin 1
  *            @arg EXTI_Pin_2: GPIO Pin 2
  *            @arg EXTI_Pin_3: GPIO Pin 3
  *            @arg EXTI_Pin_4: GPIO Pin 4
  *            @arg EXTI_Pin_5: GPIO Pin 5
  *            @arg EXTI_Pin_6: GPIO Pin 6
  *            @arg EXTI_Pin_7: GPIO Pin 7
  * @param  EXTI_Trigger : The external interrupt sensitivity value to set.
  *          This parameter can be one of the following values:
  *            @arg EXTI_Trigger_Falling_Low: Interrupt on Falling edge and Low level
  *            @arg EXTI_Trigger_Rising: Interrupt on Rising edge only
  *            @arg EXTI_Trigger_Falling: Interrupt on Falling edge only
  *            @arg EXTI_Trigger_Rising_Falling: Interrupt on Rising and Falling edges
  * @retval None
  */
void EXTI_SetPinSensitivity(EXTI_Pin_TypeDef EXTI_Pin, EXTI_Trigger_TypeDef EXTI_Trigger)
{

  /* Check function parameters */
  assert_param(IS_EXTI_PINNUM(EXTI_Pin));
  assert_param(IS_EXTI_TRIGGER(EXTI_Trigger));

  /* Clear port sensitivity bits */
  switch (EXTI_Pin)
  {
    case EXTI_Pin_0:
      EXTI->CR1 &=  (uint8_t)(~EXTI_CR1_P0IS);
      EXTI->CR1 |= (uint8_t)((uint8_t)(EXTI_Trigger) << EXTI_Pin);
      break;
    case EXTI_Pin_1:
      EXTI->CR1 &=  (uint8_t)(~EXTI_CR1_P1IS);
      EXTI->CR1 |= (uint8_t)((uint8_t)(EXTI_Trigger) << EXTI_Pin);
      break;
    case EXTI_Pin_2:
      EXTI->CR1 &=  (uint8_t)(~EXTI_CR1_P2IS);
      EXTI->CR1 |= (uint8_t)((uint8_t)(EXTI_Trigger) << EXTI_Pin);
      break;
    case EXTI_Pin_3:
      EXTI->CR1 &=  (uint8_t)(~EXTI_CR1_P3IS);
      EXTI->CR1 |= (uint8_t)((uint8_t)(EXTI_Trigger) << EXTI_Pin);
      break;
    case EXTI_Pin_4:
      EXTI->CR2 &=  (uint8_t)(~EXTI_CR2_P4IS);
      EXTI->CR2 |= (uint8_t)((uint8_t)(EXTI_Trigger) << ((uint8_t)EXTI_Pin & (uint8_t)0xEF));
      break;
    case EXTI_Pin_5:
      EXTI->CR2 &=  (uint8_t)(~EXTI_CR2_P5IS);
      EXTI->CR2 |= (uint8_t)((uint8_t)(EXTI_Trigger) << ((uint8_t)EXTI_Pin & (uint8_t)0xEF));
      break;
    case EXTI_Pin_6:
      EXTI->CR2 &=  (uint8_t)(~EXTI_CR2_P6IS);
      EXTI->CR2 |= (uint8_t)((uint8_t)(EXTI_Trigger) << ((uint8_t)EXTI_Pin & (uint8_t)0xEF));
      break;
    case EXTI_Pin_7:
      EXTI->CR2 &=  (uint8_t)(~EXTI_CR2_P7IS);
      EXTI->CR2 |= (uint8_t)((uint8_t)(EXTI_Trigger) << ((uint8_t)EXTI_Pin & (uint8_t)0xEF));
      break;
    default:
      break;
  }
}


/**
  * @brief  Enables or disables the specified TIM3 interrupts.
  * @param  TIM3_IT: Specifies the TIM3 interrupts sources to be enabled or disabled.
  *          This parameter can be any combination of the following values:
  *            @arg TIM3_IT_Update: Update
  *            @arg TIM3_IT_CC1: Capture Compare Channel1
  *            @arg TIM3_IT_CC2: Capture Compare Channel2
  *            @arg TIM3_IT_Trigger: Trigger
  *            @arg TIM3_IT_Break: Break
  * @param  NewState: The new state of the TIM3 peripheral.
  *          This parameter can be ENABLE or DISABLE
  * @retval None
  */
void TIM3_ITConfig(TIM3_IT_TypeDef TIM3_IT, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_TIM3_IT(TIM3_IT));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Enable the Interrupt sources */
    TIM3->IER |= (uint8_t)TIM3_IT;
  }
  else
  {
    /* Disable the Interrupt sources */
    TIM3->IER &= (uint8_t)(~(uint8_t)TIM3_IT);
  }
}

/**
  * @brief  Enables or disables the TIM3 peripheral.
  * @param  NewState: The new state of the TIM3 peripheral.
  *          This parameter can be ENABLE or DISABLE
  * @retval None
  */
void TIM3_Cmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* set or Reset the CEN Bit */
  if (NewState != DISABLE)
  {
    TIM3->CR1 |= TIM_CR1_CEN;
  }
  else
  {
    TIM3->CR1 &= (uint8_t)(~TIM_CR1_CEN);
  }
}

/**
  * @brief  Initializes the TIM3 Time Base Unit according to the specified  parameters.
  * @param  TIM3_Prescaler: Prescaler
  *          This parameter can be one of the following values:
  *            @arg TIM3_Prescaler_1: Time base Prescaler = 1 (No effect)
  *            @arg TIM3_Prescaler_2: Time base Prescaler = 2
  *            @arg TIM3_Prescaler_4: Time base Prescaler = 4
  *            @arg TIM3_Prescaler_8: Time base Prescaler = 8
  *            @arg TIM3_Prescaler_16: Time base Prescaler = 16
  *            @arg TIM3_Prescaler_32: Time base Prescaler = 32
  *            @arg TIM3_Prescaler_64: Time base Prescaler = 64
  *            @arg TIM3_Prescaler_128: Time base Prescaler = 128
  * @param  TIM3_CounterMode: Counter mode
  *          This parameter can be one of the following values:
  *            @arg TIM3_CounterMode_Up: Counter Up Mode
  *            @arg TIM3_CounterMode_Down: Counter Down Mode
  *            @arg TIM3_CounterMode_CenterAligned1: Counter Central aligned Mode 1
  *            @arg TIM3_CounterMode_CenterAligned2: Counter Central aligned Mode 2
  *            @arg TIM3_CounterMode_CenterAligned3: Counter Central aligned Mode 3
  * @param  TIM3_Period: This parameter must be a value between 0x0000 and 0xFFFF.
  * @retval None
  */

void TIM3_TimeBaseInit(TIM3_Prescaler_TypeDef TIM3_Prescaler,
                       TIM3_CounterMode_TypeDef TIM3_CounterMode,
                       uint16_t TIM3_Period)
{

  assert_param(IS_TIM3_PRESCALER(TIM3_Prescaler));
  assert_param(IS_TIM3_COUNTER_MODE(TIM3_CounterMode));



  /* Set the Autoreload value */
  TIM3->ARRH = (uint8_t)(TIM3_Period >> 8) ;
  TIM3->ARRL = (uint8_t)(TIM3_Period);

  /* Set the Prescaler value */
  TIM3->PSCR = (uint8_t)(TIM3_Prescaler);

  /* Select the Counter Mode */
  TIM3->CR1 &= (uint8_t)((uint8_t)(~TIM_CR1_CMS)) & ((uint8_t)(~TIM_CR1_DIR));
  TIM3->CR1 |= (uint8_t)(TIM3_CounterMode);

  /* Generate an update event to reload the Prescaler value immediately */
  TIM3->EGR = TIM3_EventSource_Update;
}


/**
  * @brief  Clears the TIM's pending flags.
  * @param  TIM3_FLAG: Specifies the flag to clear.
  *          This parameter can be any combination of the following values:
  *            @arg TIM3_FLAG_Update: Update
  *            @arg TIM3_FLAG_CC1: Capture Compare Channel1
  *            @arg TIM3_FLAG_CC2: Capture Compare Channel2
  *            @arg TIM3_FLAG_Trigger: Trigger
  *            @arg TIM3_FLAG_Break: Break
  * @retval None
  */
void TIM3_ClearFlag(TIM3_FLAG_TypeDef TIM3_FLAG)
{
  /* Check the parameters */
  assert_param(IS_TIM3_CLEAR_FLAG((uint16_t)TIM3_FLAG));
  /* Clear the flags (rc_w0) clear this bit by writing 0. Writing '1' has no effect*/
  TIM3->SR1 = (uint8_t)(~(uint8_t)(TIM3_FLAG));
  TIM3->SR2 = (uint8_t)(~(uint8_t)((uint16_t)TIM3_FLAG >> 8));
}



/**
  * @brief  Initializes the TIM2 Time Base Unit according to the specified  parameters.
  * @param  TIM2_Prescaler: Prescaler
  *          This parameter can be one of the following values:
  *            @arg TIM2_Prescaler_1: Time base Prescaler = 1 (No effect)
  *            @arg TIM2_Prescaler_2: Time base Prescaler = 2
  *            @arg TIM2_Prescaler_4: Time base Prescaler = 4
  *            @arg TIM2_Prescaler_8: Time base Prescaler = 8
  *            @arg TIM2_Prescaler_16: Time base Prescaler = 16
  *            @arg TIM2_Prescaler_32: Time base Prescaler = 32
  *            @arg TIM2_Prescaler_64: Time base Prescaler = 64
  *            @arg TIM2_Prescaler_128: Time base Prescaler = 128
  * @param  TIM2_CounterMode: Counter mode
  *          This parameter can be one of the following values:
  *            @arg TIM2_CounterMode_Up: Counter Up Mode
  *            @arg TIM2_CounterMode_Down: Counter Down Mode
  *            @arg TIM2_CounterMode_CenterAligned1: Counter Central aligned Mode 1
  *            @arg TIM2_CounterMode_CenterAligned2: Counter Central aligned Mode 2
  *            @arg TIM2_CounterMode_CenterAligned3: Counter Central aligned Mode 3
  * @param  TIM2_Period: This parameter must be a value between 0x0000 and 0xFFFF.
  * @retval None
  */

void TIM2_TimeBaseInit(TIM2_Prescaler_TypeDef TIM2_Prescaler,
                       TIM2_CounterMode_TypeDef TIM2_CounterMode,
                       uint16_t TIM2_Period)
{

  assert_param(IS_TIM2_PRESCALER(TIM2_Prescaler));
  assert_param(IS_TIM2_COUNTER_MODE(TIM2_CounterMode));



  /* Set the Autoreload value */
  TIM2->ARRH = (uint8_t)(TIM2_Period >> 8) ;
  TIM2->ARRL = (uint8_t)(TIM2_Period);

  /* Set the Prescaler value */
  TIM2->PSCR = (uint8_t)(TIM2_Prescaler);

  /* Select the Counter Mode */
  TIM2->CR1 &= (uint8_t)((uint8_t)(~TIM_CR1_CMS)) & ((uint8_t)(~TIM_CR1_DIR));
  TIM2->CR1 |= (uint8_t)(TIM2_CounterMode);

  /* Generate an update event to reload the Prescaler value immediately */
  TIM2->EGR = TIM2_EventSource_Update;
}


/**
  * @brief  Clears the TIM's pending flags.
  * @param  TIM2_FLAG: Specifies the flag to clear.
  *          This parameter can be any combination of the following values:
  *            @arg TIM2_FLAG_Update: Update
  *            @arg TIM2_FLAG_CC1: Capture Compare Channel1
  *            @arg TIM2_FLAG_CC2: Capture Compare Channel2
  *            @arg TIM2_FLAG_Trigger: Trigger
  *            @arg TIM2_FLAG_Break: Break
  * @retval None
  */
void TIM2_ClearFlag(TIM2_FLAG_TypeDef TIM2_FLAG)
{
  /* Check the parameters */
  assert_param(IS_TIM2_CLEAR_FLAG((uint16_t)TIM2_FLAG));
  /* Clear the flags (rc_w0) clear this bit by writing 0. Writing '1' has no effect*/
  TIM2->SR1 = (uint8_t)(~(uint8_t)(TIM2_FLAG));
  TIM2->SR2 = (uint8_t)(~(uint8_t)((uint16_t)TIM2_FLAG >> 8));
}


/**
  * @brief  Enables or disables the specified TIM2 interrupts.
  * @param  TIM2_IT: Specifies the TIM2 interrupts sources to be enabled or disabled.
  *          This parameter can be any combination of the following values:
  *            @arg TIM2_IT_Update: Update
  *            @arg TIM2_IT_CC1: Capture Compare Channel1
  *            @arg TIM2_IT_CC2: Capture Compare Channel2
  *            @arg TIM2_IT_Trigger: Trigger
  *            @arg TIM2_IT_Break: Break
  * @param  NewState: The new state of the TIM2 peripheral.
  *          This parameter can be ENABLE or DISABLE
  * @retval None
  */
void TIM2_ITConfig(TIM2_IT_TypeDef TIM2_IT, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_TIM2_IT(TIM2_IT));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Enable the Interrupt sources */
    TIM2->IER |= (uint8_t)TIM2_IT;
  }
  else
  {
    /* Disable the Interrupt sources */
    TIM2->IER &= (uint8_t)(~(uint8_t)TIM2_IT);
  }
}


/**
  * @brief  Enables or disables the TIM2 peripheral.
  * @param  NewState: The new state of the TIM2 peripheral.
  *          This parameter can be ENABLE or DISABLE
  * @retval None
  */
void TIM2_Cmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* set or Reset the CEN Bit */
  if (NewState != DISABLE)
  {
    TIM2->CR1 |= TIM_CR1_CEN;
  }
  else
  {
    TIM2->CR1 &= (uint8_t)(~TIM_CR1_CEN);
  }
}

/**
  * @brief  Configures the system clock (SYSCLK).
  * @note   The HSI is used (enabled by hardware) as system clock source after
  *         startup from Reset, wake-up from Halt and active Halt modes, or in case
  *         of failure of the HSE used as system clock (if the Clock Security System CSS is enabled).
  * @note   A switch from one clock source to another occurs only if the target
  *         clock source is ready (clock stable after startup delay or PLL locked).
  *         You can use CLK_GetSYSCLKSource() function to know which clock is
  *         currently used as system clock source.
  * @param  CLK_SYSCLKSource: specifies the clock source used as system clock.
  *          This parameter can be one of the following values:
  *            @arg CLK_SYSCLKSource_HSI: HSI selected as system clock source
  *            @arg CLK_SYSCLKSource_HSE: HSE selected as system clock source
  *            @arg CLK_SYSCLKSource_LSI: LSI selected as system clock source
  *            @arg CLK_SYSCLKSource_LSE: LSE selected as system clock source
  * @retval None
  */
void CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_TypeDef CLK_SYSCLKSource)
{
  /* check teh parameters */
  assert_param(IS_CLK_SOURCE(CLK_SYSCLKSource));

  /* Selection of the target clock source */
  CLK->SWR = (uint8_t)CLK_SYSCLKSource;
}

/**
  * @brief  Configures the System clock (SYSCLK) dividers.
  * @param  CLK_SYSCLKDiv : Specifies the system clock divider to apply.
  *          This parameter can be one of the following values:
  *            @arg CLK_SYSCLKDiv_1
  *            @arg CLK_SYSCLKDiv_2
  *            @arg CLK_SYSCLKDiv_4
  *            @arg CLK_SYSCLKDiv_8
  *            @arg CLK_SYSCLKDiv_16
  *            @arg CLK_SYSCLKDiv_64
  *            @arg CLK_SYSCLKDiv_128
  * @retval None
  */
void CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_TypeDef CLK_SYSCLKDiv)
{
  /* check the parameters */
  assert_param(IS_CLK_SYSTEM_DIVIDER(CLK_SYSCLKDiv));

  CLK->CKDIVR = (uint8_t)(CLK_SYSCLKDiv);
}



/**
  * @brief  Checks whether the specified SPI flag is set or not.
  * @param  SPIx: where x can be 1 to select the specified SPI peripheral.
  * @param  SPI_FLAG: Specifies the flag to check.
  *          This parameter can be one of the following values:
  *            @arg SPI_FLAG_BSY: Busy
  *            @arg SPI_FLAG_OVR: Overrun
  *            @arg SPI_FLAG_MODF: Mode fault
  *            @arg SPI_FLAG_CRCERR: CRC error
  *            @arg SPI_FLAG_WKUP: Wake-up
  *            @arg SPI_FLAG_TXE: Transmit buffer empty
  *            @arg SPI_FLAG_RXNE: Receive buffer empty
  * @retval Indicates the state of SPI_FLAG.
  *         This parameter can be SET or RESET.
  */
FlagStatus SPI_GetFlagStatus(SPI_TypeDef* SPIx, SPI_FLAG_TypeDef SPI_FLAG)
{
  FlagStatus status = RESET;
  /* Check parameters */
  assert_param(IS_SPI_FLAG(SPI_FLAG));

  /* Check the status of the specified SPI flag */
  if ((SPIx->SR & (uint8_t)SPI_FLAG) != (uint8_t)RESET)
  {
    status = SET; /* SPI_FLAG is set */
  }
  else
  {
    status = RESET; /* SPI_FLAG is reset*/
  }

  /* Return the SPI_FLAG status */
  return status;
}

/**
  * @brief  Transmits a Data through the SPI peripheral.
  * @param  SPIx: where x can be 1 to select the specified SPI peripheral.
  * @param  Data: Byte to be transmitted.
  * @retval None
  */
void SPI_SendData(SPI_TypeDef* SPIx, uint8_t Data)
{
  SPIx->DR = Data; /* Write in the DR register the data to be sent*/
}

/**
  * @brief  Returns the most recent received data by the SPI peripheral.
  * @param  SPIx: where x can be 1 to select the specified SPI peripheral.
  * @retval The value of the received data.
  */
uint8_t SPI_ReceiveData(SPI_TypeDef* SPIx)
{
  return ((uint8_t)SPIx->DR); /* Return the data in the DR register*/
}

/**
  * @brief  Clears the TIM's interrupt pending bits.
  * @param  TIM3_IT: Specifies the pending bit to clear.
  *          This parameter can be any combination of the following values:
  *            @arg TIM3_IT_Update: Update
  *            @arg TIM3_IT_CC1: Capture Compare Channel1
  *            @arg TIM3_IT_CC2: Capture Compare Channel2
  *            @arg TIM3_IT_Trigger: Trigger
  *            @arg TIM3_IT_Break: Break
  * @retval None
  */
void TIM3_ClearITPendingBit(TIM3_IT_TypeDef TIM3_IT)
{
  /* Check the parameters */
  assert_param(IS_TIM3_IT(TIM3_IT));

  /* Clear the IT pending Bit */
  TIM3->SR1 = (uint8_t)(~(uint8_t)TIM3_IT);
}


/**
  * @brief  Clears the TIM's interrupt pending bits.
  * @param  TIM2_IT: Specifies the pending bit to clear.
  *          This parameter can be any combination of the following values:
  *            @arg TIM2_IT_Update: Update
  *            @arg TIM2_IT_CC1: Capture Compare Channel1
  *            @arg TIM2_IT_CC2: Capture Compare Channel2
  *            @arg TIM2_IT_Trigger: Trigger
  *            @arg TIM2_IT_Break: Break
  * @retval None
  */
void TIM2_ClearITPendingBit(TIM2_IT_TypeDef TIM2_IT)
{
  /* Check the parameters */
  assert_param(IS_TIM2_IT(TIM2_IT));

  /* Clear the IT pending Bit */
  TIM2->SR1 = (uint8_t)(~(uint8_t)TIM2_IT);
}


/**
  * @brief  Initializes the TIM2 Channel1 according to the specified parameters.
  * @param  TIM2_OCMode: Output Compare Mode
  *          This parameter can be one of the following values:
  *            @arg TIM2_OCMode_Timing: Timing (Frozen) Mode
  *            @arg TIM2_OCMode_Active: Active Mode
  *            @arg TIM2_OCMode_Inactive: Inactive Mode
  *            @arg TIM2_OCMode_Toggle: Toggle Mode
  *            @arg TIM2_OCMode_PWM1: PWM Mode 1
  *            @arg TIM2_OCMode_PWM2: PWM Mode 2
  * @param  TIM2_OutputState: Output state
  *          This parameter can be one of the following values:
  *            @arg TIM2_OutputState_Disable: Output compare State disabled (channel output disabled)
  *            @arg TIM2_OutputState_Enable: Output compare State enabled (channel output enabled)
  * @param  TIM2_Pulse: This parameter must be a value between 0x0000 and 0xFFFF.
  * @param  TIM2_OCPolarity: Polarity
  *          This parameter can be one of the following values:
  *            @arg TIM2_OCPolarity_High: Output compare polarity  = High
  *            @arg TIM2_OCPolarity_Low: Output compare polarity  = Low
  * @param  TIM2_OCIdleState: Output Compare Idle State
  *          This parameter can be one of the following values:
  *            @arg TIM2_OCIdleState_Reset: Output Compare Idle state  = Reset
  *            @arg TIM2_OCIdleState_Set: Output Compare Idle state  = Set
  * @retval None
  */
void TIM2_OC1Init(TIM2_OCMode_TypeDef TIM2_OCMode,
                  TIM2_OutputState_TypeDef TIM2_OutputState,
                  uint16_t TIM2_Pulse,
                  TIM2_OCPolarity_TypeDef TIM2_OCPolarity,
                  TIM2_OCIdleState_TypeDef TIM2_OCIdleState)
{
  uint8_t tmpccmr1 = 0;

  /* Check the parameters */
  assert_param(IS_TIM2_OC_MODE(TIM2_OCMode));
  assert_param(IS_TIM2_OUTPUT_STATE(TIM2_OutputState));
  assert_param(IS_TIM2_OC_POLARITY(TIM2_OCPolarity));
  assert_param(IS_TIM2_OCIDLE_STATE(TIM2_OCIdleState));

  tmpccmr1 = TIM2->CCMR1;

  /* Disable the Channel 1: Reset the CCE Bit */
  TIM2->CCER1 &= (uint8_t)(~TIM_CCER1_CC1E);
  /* Reset the Output Compare Bits */
  tmpccmr1 &= (uint8_t)(~TIM_CCMR_OCM);

  /* Set the Output Compare Mode */
  tmpccmr1 |= (uint8_t)TIM2_OCMode;

  TIM2->CCMR1 = tmpccmr1;

  /* Set the Output State */
  if (TIM2_OutputState == TIM2_OutputState_Enable)
  {
    TIM2->CCER1 |= TIM_CCER1_CC1E;
  }
  else
  {
    TIM2->CCER1 &= (uint8_t)(~TIM_CCER1_CC1E);
  }

  /* Set the Output Polarity */
  if (TIM2_OCPolarity == TIM2_OCPolarity_Low)
  {
    TIM2->CCER1 |= TIM_CCER1_CC1P;
  }
  else
  {
    TIM2->CCER1 &= (uint8_t)(~TIM_CCER1_CC1P);
  }

  /* Set the Output Idle state */
  if (TIM2_OCIdleState == TIM2_OCIdleState_Set)
  {
    TIM2->OISR |= TIM_OISR_OIS1;
  }
  else
  {
    TIM2->OISR &= (uint8_t)(~TIM_OISR_OIS1);
  }

  /* Set the Pulse value */
  TIM2->CCR1H = (uint8_t)(TIM2_Pulse >> 8);
  TIM2->CCR1L = (uint8_t)(TIM2_Pulse);
}
