/**
  **********************************************************************************
  * @file      ebyte_callback.h
  * @brief     EBYTE driver library's send and receive completion callback function is implemented by the customer's own logic code
  * @details   See https://www.ebyte.com/ for details
  * @author    JiangHeng
  * @date      2021-05-26
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
  **************************************************************************************
  */
#include  "ebyte_callback.h"

/*= !!! Configure target hardware platform header file ========================================== = */
#include  "board.h"  // E15-EVB02 evaluation board
/*= !!! Configure target hardware variables ============================================= */
extern void UserTransmitDoneCallback(void);
extern void UserReceiveDoneCallback( uint8_t *buffer, uint8_t length);
/*==================================================================*/



/* !
 * @brief The send completion callback interface is implemented by the client to implement its own send completion logic
 *
 * @param state The status code provided by the upper layer callback Customers please find the corresponding area according to the sample comments
 *
 */
void Ebyte_Port_TransmitCallback( uint16e_t state )
{
    /* Send: completed normally */
    if( state &= 0x0001 )
    {
        //To-do implements its own logic
        UserTransmitDoneCallback ();
    }
    /* Send: exception timeout */
    else if ( state &= 0x0200 )
    {
    //To-do implements its own logic
    }
    /* Send: unknown error */
    else
    {
        /* The sending status flag is incorrect, please check the software and hardware
        FAQ 1: Incorrect SPI communication 2: Insufficient power supply of the module */
        while (1);
    }
}

/* !
 * @brief The receiving completion callback interface is implemented by the client to implement its own sending completion logic
 *
 * @param state The status code provided by the upper layer callback Customers please find the corresponding area according to the sample comments
 */
void Ebyte_Port_ReceiveCallback( uint16e_t state, uint8e_t *buffer, uint8e_t length )
{

    /* Receive: normal */
    if ( state &= 0x0002 )
    {
        UserReceiveDoneCallback ( buffer , length);
    }
    /* Receive: exception timeout */
    else  if ( state &= 0x0200 )
    {
        // To-do implements its own logic
    }
    /* Receive: unknown error */
    else
    {
        /* The sending status flag is incorrect, please check the software and hardware
        FAQ 1: Incorrect SPI communication 2: Insufficient power supply of the module */
        while(1);
    }
}