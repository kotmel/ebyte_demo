/**
  **********************************************************************************
  * @file      ebyte_callback.h
  * @brief     EBYTE驱动库的收发完成回调函数 由客户实现自己的逻辑代码 
  * @details   详情请参见 https://www.ebyte.com/       
  * @author    JiangHeng     
  * @date      2021-05-26     
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
#include "ebyte_callback.h"

/*= !!!配置目标硬件平台头文件 =======================================*/
#include "board.h"  //E15-EVB02 评估板
/*= !!!配置目标硬件变量       =======================================*/
extern void UserTransmitDoneCallback(void);
extern void UserReceiveDoneCallback( uint8_t *buffer, uint8_t length);
/*==================================================================*/

/*!
 * @brief 发送完成回调接口 由客户实现自己的发送完成逻辑
 * 
 * @param state 上层回调提供的状态码 客户请根据示例注释找到对应区域
 *  
 */
void Ebyte_Port_TransmitCallback( uint16e_t state )
{       
    /* 发送: 正常完成 */
    if( state &= 0x0001 )
    {
        //To-do 实现自己的逻辑 
        UserTransmitDoneCallback();
    }
    /* 发送: 其他情况 */
    else 
    {
        //To-do 实现自己的逻辑 
    }
}

/*!
 * @brief 接收完成回调接口 由客户实现自己的发送完成逻辑
 * 
 * @param state 上层回调提供的状态码 客户请根据示例注释找到对应区域
 */
void Ebyte_Port_ReceiveCallback(  uint16e_t state, uint8e_t *buffer, uint8e_t length )
{
    /* 接收: 正常 */
    if( state &= 0x0001 )
    {
        //To-do 实现自己的逻辑     
        UserReceiveDoneCallback( buffer , length);//示例

    }
    /* 接收: 额外情况 */
    else 
    {
        //To-do 实现自己的逻辑 
    }

}