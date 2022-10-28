/**
  **********************************************************************************
  * @file      main.c
  * @brief     E15-EVB02 评估板固件
  * @details   本例程为: 串口无线透传示例  详情请参见 https://www.ebyte.com/
  * @author    JiangHeng
  * @date      2021-05-06
  * @version   1.0.0
  **********************************************************************************
  * @copyright BSD License
  *            成都亿佰特电子科技有限公司
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

void Task_Transmit( void );
void Task_Button( void );

/* 串口 数据存储队列 */
Ebyte_FIFO_t hfifo;

/* 串口 帧接收完成标识 */
uint8_t Uart_isRecvReady = 0;

/* 串口 FIFO检查标识 */
uint8_t FIFO_isTimeCheckReady = 0;

/* 串口 自定义收/发缓存 */
uint8_t TxBuffer[64] = {0};
uint8_t RxBuffer[64] = {0};
uint8_t PcEchoBuffer[20] = {0};

uint8_t BufferPing[5] = {'p', 'i', 'n', 'g'};
uint8_t BufferPong[5] = {'p', 'o', 'n', 'g'};

/* 串口 FIFO检查标识 */
uint8_t Callback_isPingCheckReady = 0;

/* 自检模式 标识 */
uint8_t PC_isConnected = 0;

static BSP_BTN_EVENT_t BTN_Event;

/* !
 * @brief 主函数
 */
int main( void )
{
    uint8_t result;
    uint8_t* pName;
    /* 板载硬件资源 初始化 */
    Ebyte_BSP_Init();
    /* (可选) 串口中断接收FIFO 可根据需要自行处理 */
    Ebyte_FIFO_Init( &hfifo, EBYTE_FIFO_SIZE );
    /* EBYTE 无线模块初始化 请务必根据具体产品在IAR中选择对应的Workspace*/
    result = Ebyte_RF.Init();
    if( result != 0 )
    {
        DEBUG( "Wireless module initialization failed ! \r\n" );
        while( 1 ); //初始化失败 请检查硬件
    }
    else
    {
        pName = Ebyte_RF.GetName();
        DEBUG( "Wireless module initialization success . Name:%s  \r\n", pName );
    }
    /* MCU 开全局中断 */
    Ebyte_BSP_GlobalIntEnable();
    
    DEBUG( "===== This is an example of wireless transmission ==== \r\n" );
    DEBUG( "Please push button1 or button2 \r\n" );
    DEBUG( "Waiting ........ \r\n" );
    while( 1 )
    {
        /* 按键事件响应 */
        Task_Button();
        /* 任务:检测串口数据并无线发送 客户请按需自定义 */
        Task_Transmit();
        /* 任务:EBYTE驱动库必须的周期执行任务  客户无需修改 */
        Ebyte_RF.TaskForPoll();
    }
}

/* !
 * @brief  轮询任务 串口接收的数据通过模块发送
 *
 * @note 建议客户将需要发送的数据进行组包处理，否则需要考虑额外的无线传输开销
 *       注意无线模块的模式切换，一般情况下可以处于接收/休眠/周期休眠  此示例过程为: 接收模式->发送模式->接收模式
 */
void Task_Transmit( void )
{
    uint16_t length = 0;
    uint8_t pcEchoLength = 0;
    uint8_t pongLength = 0;
    /* 是否有数据需要无线传输 (来自串口接收FIFO缓存) */
    Ebyte_FIFO_GetDataLength( &hfifo, &length );
    /* 涉及到异步中断,这里简单示范 判断两种条件
       1; 串口FIFO中有数据且已经有完整接收的帧，此时启用无线发送( 采用时间断帧方式,连续传输时低于判断时间会导致数据粘包 请根据具体项目自行考虑帧判断 )
       2: 串口FIFO中有数据但串口已经退出了接收状态，超时检测条件成立 ( 会导致FIFO剩余未发送数据被打包无线发送 请根据具体项目自行考虑帧判断 )*/
    if( ( length != 0 && Uart_isRecvReady ) || ( length != 0 && FIFO_isTimeCheckReady ) )
    {
        Ebyte_BSP_LedControl( BSP_LED_1, ON );
        /* 读取FIFO 数据放入TxBuffer */
        Ebyte_FIFO_Read( &hfifo, TxBuffer, length );
        /* PC特殊指令响应 */
        if( Ebyte_DEBUG_CommandEcho( TxBuffer, length, PcEchoBuffer, &pcEchoLength ) )
        {
            Ebyte_BSP_UartTransmit( PcEchoBuffer, pcEchoLength );
        }
        /* 非特殊指令则无线透传 */
        else
        {
            /* 启用无线模块进行发送  */
            Ebyte_RF.GoTransmit( TxBuffer, length );
        }
        /* 每发完一帧就减少帧计数 串口中断可能已经写入了多帧 */
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
    /* 接收完成回调函数检测到了 ping 命令 回复 pong */
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
        /* 启用无线模块进行发送  */
        Ebyte_BSP_DelayMs(10);//兼容部分模块收/发切换存在延迟
        Ebyte_RF.GoTransmit( BufferPong, pongLength );
        Callback_isPingCheckReady = 0;
    }
}

/* !
 * @brief 按键事件响应
 */
void Task_Button( void )
{
    uint8_t pcEchoLength = 0;
    uint8_t pingLength = 0;
    if( ! Ebyte_BTN_FIFO_Pop( &BSP_BTN_FIFO, &BTN_Event ) )
    {
        switch( BTN_Event )
        {
            /* 按键1 短按 */
            case BTN_1_SHORT:
                Ebyte_BSP_LedControl( BSP_LED_1, ON );
                if( PC_isConnected )
                {
                    /* 通知PC */
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
                /* 发送 ping */
                Ebyte_RF.GoTransmit( BufferPing, pingLength );
                Ebyte_BSP_LedControl( BSP_LED_1, OFF );
                break;
            /* 按键1 长按 */
            case BTN_1_LONG:
                Ebyte_BSP_LedControl( BSP_LED_1, TOGGLE );
                break;
            /* 按键2 短按 */
            case BTN_2_SHORT:
                Ebyte_BSP_LedControl( BSP_LED_2, ON );
                if( PC_isConnected )
                {
                    /* 通知PC */
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
                /* 发送 ping */
                Ebyte_RF.GoTransmit( BufferPing, pingLength );
                Ebyte_BSP_LedControl( BSP_LED_2, OFF );
                break;
            /* 按键2 长按 */
            case BTN_2_LONG:
                Ebyte_BSP_LedControl( BSP_LED_2, TOGGLE );
                break;
            default :
                break;
        }
    }
}

/* !
 * @brief 用户发送完成回调函数
 *
 * @note 填充到ebyte_callback.h中即可
 */
void UserTransmitDoneCallback( void )
{
    /* 这里示例 发送完成后直接进入接收模式 */
    Ebyte_RF.GoReceive( );
}

/* !
 * @brief 用户接收完成回调函数
 *
 * @note 填充到ebyte_callback.h中即可
 */
void UserReceiveDoneCallback( uint8_t* buffer, uint8_t length )
{
    uint8_t  j,  pcEchoLength;
    uint8_t* p;
    /* 通过长度判断是否是ping pong指令  */
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
        //比较到了末尾表示完全匹配ping
        if( j == 4 )
        {
            Callback_isPingCheckReady = 1;//通知回复Pong
            if( length == 5 && PC_isConnected )
            {
                BufferPong[4] = buffer[4];//第5字节为按键标识
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
            if( j == 4 )//完全匹配pong
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
        DEBUG( "\r\n #RECV: " );
        Ebyte_BSP_UartTransmit( buffer, length );
    }
    Ebyte_BSP_LedControl( BSP_LED_1, OFF );
    /* 继续接收
       注意：模块调用此回调函数时，已经结束了接收状态，如果要继续接收，则需要再次进入接收状态 */
    Ebyte_RF.GoReceive( );
}