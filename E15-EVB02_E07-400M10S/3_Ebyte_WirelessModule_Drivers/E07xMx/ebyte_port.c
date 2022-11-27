/**
  **********************************************************************************
  * @file      ebyte_port.h
  * @brief     The hardware interface layer of the EBYTE driver library is provided by the customer himself E07 IO interface
  * @details   See https://www.ebyte.com/ for details
  * @author    JiangHeng     
  * @date      2021-05-13     
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
#include "ebyte_port.h"


/* = !!! Configure target hardware platform header file ========================================== = */
#include "board.h"  // E15-EVB02 evaluation board
/*==================================================================*/

/* !
 * @brief Configure target hardware platform SPI interface transceiver function
 *
 * @param send EBYTE The upper layer of the driver library calls the data that needs to be transmitted 1 Byte
 * @return SPI received data 1 Byte
 */
uint8e_t Ebyte_Port_SpiTransmitAndReceivce( uint8e_t send )
{
    uint8e_t result = 0;
    
    /* ! Must provide: SPI interface */
    result = Ebyte_BSP_SpiTransAndRecv( send );//ʾ��
    
    return result;
}

/* !
 * @brief (optional) Configure target hardware platform SPI_CS(NSS) function
 *
 * @param cmd EBYTE drive library upper expected control command
 * @arg 0: It is expected that the CS (NSS) pin outputs a low-level logic signal, and the SPI interface of the EBYTE module is selected as a low level
 * @arg 1: Expect the CS(NSS) pin to output a high-level logic signal
 */
void Ebyte_Port_SpiCsIoControl( uint8e_t cmd )
{
    if ( cmd == 1 )
    {
       /* !Optional: SPI CS control E22xx high level unselected */
       Ebyte_BSP_RfSpiUnselected(); 
    }
    else
    {
       /* !Optional: SPI CS controls E22xx low level selection */
       Ebyte_BSP_RfSpiSelected( );
    }
}


/* !
 * @brief Configure target hardware platform receive enable pin GDO 0 control function
 *
 * @note The GDO0 pin is a configurable digital signal output pin, generally used as a FIFO status indicator signal
 *
 */
uint8e_t Ebyte_Port_Gdo0IoRead( void )
{
    uint8e_t result = 0 ;  
    
    /* !Must provide: target hardware circuit corresponding to BUSY IO status reading */
    result = Ebyte_BSP_RfGdo0IoRead();
    return result;
}

/* !
 * @brief Configure the target hardware platform to receive the enable pin GDO 1 control function
 */
uint8e_t Ebyte_Port_Gdo1IoRead( void )
{
    uint8e_t result = 0 ;  
    
    /* !Must provide: target hardware circuit corresponding to BUSY IO status reading */
    result = Ebyte_BSP_RfGdo0IoRead();
    return result;
}


/* !
 * @brief Configure target hardware platform delay function
 *
 * @param time expected delay in milliseconds
 * @note Please pay attention to the timing, this function will be called when the module is initialized, please pay attention to whether the interrupt will affect this function
 */
void Ebyte_Port_DelayMs( uint16e_t time )
{
    /* !Must provide: delay function */
  
    uint16e_t i,n;
    
    while(time--)// Here demonstrates the rough software delay under the premise of STM8L 16M clock
    {
        for(i=900;i>0;i--)
        {
              for( n=1 ;n>0 ;n--)
              {
				#ifdef __CSMC__
				  __asm("nop"); 
                  __asm("nop");
                  __asm("nop");
                  __asm("nop");
                #elif defined(_SDCC_)
                  __asm__("nop"); 
                  __asm__("nop");
                  __asm__("nop");
                  __asm__("nop");
				#else
                  asm("nop"); 
                  asm("nop");
                  asm("nop");
                  asm("nop");
				#endif
              }
        }
    }    
}



