/**
  **********************************************************************************
  * @file      main.c
  * @brief E15-EVB02 evaluation board firmware
  * @details This routine is: Serial port wireless transparent transmission example For details, please refer to https://www.ebyte.com/
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

#include "ebyte_core.h"
#include "ebyte_kfifo.h"
#include "ebyte_debug.h"

#if defined(_SDCC_)
// interrupt function prototypes must be visible to main function !
#include "stm8l15x_it.h"
#endif

#include "aes.h"
#include "string.h"

uint8_t key[] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };
uint8_t iv[]  = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };

void Task_Transmit( void );
void Task_Button( void );

/* ���� ���ݴ洢���� */
Ebyte_FIFO_t hfifo;

/* Serial port frame receiving completion flag */
uint8_t Uart_isRecvReady = 0;

/* Serial port FIFO check mark */
uint8_t FIFO_isTimeCheckReady = 0;

/* Serial custom receive/send cache */
uint8_t TxBuffer[64] = {0};
uint8_t RxBuffer[64] = {0};
uint8_t PcEchoBuffer[20] = {0};

uint8_t BufferPing[5] = {'p', 'i', 'n', 'g'};
uint8_t BufferPong[5] = {'p', 'o', 'n', 'g'};

/* Serial port FIFO check mark */
uint8_t Callback_isPingCheckReady = 0;

/* Self-test mode identification */
uint8_t PC_isConnected = 0;

static BSP_BTN_EVENT_t BTN_Event;

static void decrypt_cbc(uint8_t* in, uint8_t len)
{
    struct AES_ctx ctx;
    DEBUG("decrypt\r\n");
    AES_init_ctx_iv(&ctx, key, iv);
    AES_CBC_decrypt_buffer(&ctx, in, len);
}

static void encrypt_cbc(uint8_t* in, uint8_t len)
{
    struct AES_ctx ctx;

    DEBUG("encrypt\r\n");
    AES_init_ctx_iv(&ctx, key, iv);
    AES_CBC_encrypt_buffer(&ctx, in, len);
}


/* !
 * @brief main function
 */
int main( void )
{
    uint8_t result;
    uint8_t* pName;
    /* Onboard hardware resource initialization */
    Ebyte_BSP_Init();
    /* (Optional) The serial port interrupt receiving FIFO can be processed by itself according to needs */
    Ebyte_FIFO_Init( &hfifo, EBYTE_FIFO_SIZE );
    /* EBYTE wireless module initialization, please be sure to select the corresponding Workspace in IAR according to the specific product */
    result = Ebyte_RF.Init();
    if( result != 0 )
    {
        DEBUG( "Wireless module initialization failed ! \r\n" );
        while( 1 ); // Initialization failed, please check the hardware
    }
    else
    {
        pName = Ebyte_RF.GetName();
        DEBUG( "Wireless module initialization success . Name:%s  \r\n", pName );
    }
    /* MCU enable global interrupt */
    Ebyte_BSP_GlobalIntEnable();
    
    
    DEBUG( "===== This is an example of wireless transmission ==== \r\n" );
    DEBUG( "Please push button1 or button2 \r\n" );
    DEBUG( "Waiting ........ \r\n" );
    while( 1 )
    {
        /* key event response */
        Task_Button();
        /* Task: Detect serial port data and send it wirelessly, please customize as needed */
        Task_Transmit();
        /* Task: EBYTE driver library must periodically execute the task and the customer does not need to modify it */
        // kkk handled in IRQ Ebyte_RF.TaskForPoll();
    }
}
/* !
* @brief The data received by the serial port of the polling task is sent through the module
*
* @note It is recommended that customers package the data to be sent, otherwise additional wireless transmission overhead needs to be considered
* Pay attention to the mode switching of the wireless module. Generally, it can be in receiving/sleep/period sleep. This example process is: receive mode->send mode->receive mode
*/
void Task_Transmit( void )
{
    uint16_t length = 0;
    uint8_t pcEchoLength = 0;
    uint8_t pongLength = 0;
    /* Whether there is data that needs to be transmitted wirelessly (from the serial port receiving FIFO buffer) */
    Ebyte_FIFO_GetDataLength( &hfifo, &length );
    /* Involving asynchronous interrupt, here is a simple demonstration to judge two conditions
    1; There is data in the serial port FIFO and there have been completely received frames. At this time, wireless transmission is enabled (the method of time-breaking frame is adopted, and the continuous transmission is lower than the judgment time, which will cause data sticking. Please consider the frame judgment according to the specific project)
    2: There is data in the serial port FIFO but the serial port has exited the receiving state, and the timeout detection condition is established (it will cause the remaining unsent data in the FIFO to be packaged and sent wirelessly, please consider the frame judgment according to the specific project) */
    if( ( length != 0 && Uart_isRecvReady ) || ( length != 0 && FIFO_isTimeCheckReady ) )
    {
        Ebyte_BSP_LedControl( BSP_LED_1, ON );
        /* read FIFO data into TxBuffer */
        Ebyte_FIFO_Read( &hfifo, TxBuffer, length );
        /* PC special command response */
        if( Ebyte_DEBUG_CommandEcho( TxBuffer, length, PcEchoBuffer, &pcEchoLength ) )
        {
            Ebyte_BSP_UartTransmit( PcEchoBuffer, pcEchoLength );
        }
        /* Wireless transparent transmission if there is no special command */
        else
        {
            /* kkk encrypt 16 byte long packets */
            if (length == 16) {
                encrypt_cbc(TxBuffer, length);
            }
            /* Enable the wireless module to send */
            Ebyte_RF.GoTransmit( TxBuffer, length );
        }
        /* Reduce the frame count every time a frame is sent. The serial port interrupt may have written multiple frames */
        if( Uart_isRecvReady )
        {
            Uart_isRecvReady --;
        }
        if( FIFO_isTimeCheckReady )
        {
            FIFO_isTimeCheckReady = 0;
        }
        Ebyte_BSP_LedControl( BSP_LED_1, OFF );
    }
    /* Receive completion callback function detects ping command and replies pong */
    if( Callback_isPingCheckReady )
    {
        if( PC_isConnected )
        {
            pongLength = 5;
        }
        else
        {
            DEBUG( "\r\n #SEND: pong \r\n" );
            pongLength = 4;
        }
        /* Enable the wireless module to send */
        Ebyte_BSP_DelayMs(10);// There is a delay in receiving/transmitting switching of compatible modules
        Ebyte_RF.GoTransmit( BufferPong, pongLength );
        Callback_isPingCheckReady = 0;
    }
}

/* !
 * @brief key event response
 */
void Task_Button( void )
{
    uint8_t pcEchoLength = 0;
    uint8_t pingLength = 0;
    if( ! Ebyte_BTN_FIFO_Pop( &BSP_BTN_FIFO, &BTN_Event ) )
    {
        switch( BTN_Event )
        {
            /* Key 1 short press */
            case BTN_1_SHORT:
                Ebyte_BSP_LedControl( BSP_LED_1, ON );
                if( PC_isConnected )
                {
                    /* Notify PC */
                    Ebyte_DEBUG_CommandEcho( ( uint8_t* )SimulatedCommandsButton1, EBYTE_CMD_PACKAGE_LENGTH, PcEchoBuffer, &pcEchoLength );
                    Ebyte_BSP_UartTransmit( PcEchoBuffer, pcEchoLength );
                    BufferPing[4] = 0x01;
                    pingLength = 5;
                }
                else
                {
                    DEBUG( "\r\n #SEND: ping \r\n" );
                    pingLength = 4;
                }
                /* send ping */
                Ebyte_RF.GoTransmit( BufferPing, pingLength );
                Ebyte_BSP_LedControl( BSP_LED_1, OFF );
                break;
            /* Key 1 long press */
            case BTN_1_LONG:
                Ebyte_BSP_LedControl( BSP_LED_1, TOGGLE );
                break;
            /* Key 2 short press */
            case BTN_2_SHORT:
                Ebyte_BSP_LedControl( BSP_LED_2, ON );
                if( PC_isConnected )
                {
                    /* Notify PC */
                    Ebyte_DEBUG_CommandEcho( ( uint8_t* )SimulatedCommandsButton2, EBYTE_CMD_PACKAGE_LENGTH, PcEchoBuffer, &pcEchoLength );
                    Ebyte_BSP_UartTransmit( PcEchoBuffer, pcEchoLength );
                    BufferPing[4] = 0x02;
                    pingLength = 5;
                }
                else
                {
                    DEBUG( "\r\n #SEND: ping \r\n" );
                    pingLength = 4;
                }
                /* send ping */
                Ebyte_RF.GoTransmit( BufferPing, pingLength );
                Ebyte_BSP_LedControl( BSP_LED_2, OFF );
                break;
            /* Key 2 long press */
            case BTN_2_LONG:
                Ebyte_BSP_LedControl( BSP_LED_2, TOGGLE );
                break;
            default :
                break;
        }
    }
}

/* !
 * @brief The user sends the completion callback function
 *
 * @note can be filled into ebyte_callback.h
 */
void UserTransmitDoneCallback( void )
{
    /* This example directly enters the receiving mode after sending */
    Ebyte_RF.GoReceive( );
}

/* !
* @brief The user receives the completion callback function
*
* @note can be filled into ebyte_callback.h
 */
void UserReceiveDoneCallback( uint8_t* buffer, uint8_t length )
{
    uint8_t  j,  pcEchoLength;
    uint8_t* p;
    /* Determine whether it is a ping pong command by length */
    if( length == 4 || length == 5 )
    {
        p = buffer;
        for( j = 0; j < 4; j++ )
        {
            if( BufferPing[j] != *p++ )
            {
                break;
            }
        }
        // Compared to the end, it means a complete match of ping
        if( j == 4 )
        {
            Callback_isPingCheckReady = 1;// notify reply Pong
            if( length == 5 && PC_isConnected )
            {
                BufferPong[4] = buffer[4];// The fifth byte is the button identification
            }
        }
        if( j != 4 && length == 5 && PC_isConnected )
        {
            p = buffer;
            for( j = 0; j < 4; j++ )
            {
                if( BufferPong[j] != *p++ )
                {
                    break;
                }
            }
            if( j == 4 )// fully match pong
            {
                if( 0x01 == buffer[4] )
                {
                    Ebyte_DEBUG_CommandEcho( ( uint8_t* )SimulatedCommandsWireless1, EBYTE_CMD_PACKAGE_LENGTH, PcEchoBuffer, &pcEchoLength );
                }
                else
                {
                    Ebyte_DEBUG_CommandEcho( ( uint8_t* )SimulatedCommandsWireless2, EBYTE_CMD_PACKAGE_LENGTH, PcEchoBuffer, &pcEchoLength );
                }
                Ebyte_BSP_UartTransmit( PcEchoBuffer, pcEchoLength );
            }
        }
    }
    Ebyte_BSP_LedControl( BSP_LED_1, ON );
    if( ! PC_isConnected )
    {
        /* decrypt 16 byte long packets */
        if (length == 16) {
            decrypt_cbc(buffer, length);
        }
        DEBUG( "\r\n #RECV: " );

        Ebyte_BSP_UartTransmit( buffer, length );
    }
    Ebyte_BSP_LedControl( BSP_LED_1, OFF );
    /* Continue to receive
    Note: When the module calls this callback function, the receiving state has ended, if you want to continue receiving, you need to enter the receiving state again */
    Ebyte_RF.GoReceive( );
}