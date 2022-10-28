/**
  **********************************************************************************
  * @file      board_button.c
  * @brief     通用 按键队列驱动库         
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

#include "board.h"

/* !
 * @brief 按键队列初始化
 * 
 * @param fifo 指向按键事件队列结构体的指针
 */
void Ebyte_BTN_FIFO_Init(BSP_BTN_FIFO_t *fifo)
{
    fifo->fifoLen   = 0;
    fifo->fifoRead  = 0;
    fifo->fifoWrite = 0;
}

/* !
 * @brief 按键队列入队 
 * 
 * @param  fifo  指向按键事件队列结构体的指针
 * @param  event 按键的事件  主要为各按键的短按/长按
 * @return 0:正常  1:队列溢出
 * @note   入队位置可循环但不会覆盖已入队数据，队满时会导致数据丢失！
 */
uint8_t Ebyte_BTN_FIFO_Push(BSP_BTN_FIFO_t *fifo, BSP_BTN_EVENT_t event)
{
    /* 入队数据包长度预先自增 */
    fifo->fifoLen++;

    /* 如果入队长度大于了设定长度 */
    if(fifo->fifoLen > BSP_BTN_FIFO_LENGTH)
    {
        fifo->fifoLen = BSP_BTN_FIFO_LENGTH;//入队长度不再增加
        return 1; 
    }
    
    /* 正常入队 */
    fifo->buff[fifo->fifoWrite] = event;
    
    /* 如果入队位置已经到了队尾 */
    if(++fifo->fifoWrite >= BSP_BTN_FIFO_LENGTH)
    {
        fifo->fifoWrite = 0; //那么下一个入队数据将回到队首开始入队
    }

    return 0;

}

/* !
 * @brief 按键队列出队
 * 
 * @param fifo  指向按键事件队列结构体的指针
 * @param event 按键的事件  主要为各按键的短按/长按
 * @return 0:正常  1:队列为空
 */
uint8_t Ebyte_BTN_FIFO_Pop(BSP_BTN_FIFO_t *fifo, BSP_BTN_EVENT_t *event)
{
    /* 如果入队长度为0 即空队列 */
    if(fifo->fifoLen == 0)
    {
        return 1;
    }
    
    /* 入队数据长度自减 */
    fifo->fifoLen--;

    /* 正常出队 */
    *event =  (BSP_BTN_EVENT_t )(fifo->buff[fifo->fifoRead]);

    /* 如果出队位置已经到了队尾 */
    if(++fifo->fifoRead >= BSP_BTN_FIFO_LENGTH)
    {
        fifo->fifoRead = 0;//那么下一次将从队首开始出队
    }

    return 0;
}
