/**
  **********************************************************************************
  * @file      board_button.c
  * @brief     ͨGeneral key queue driver library
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

#include "board.h"

/* !
 * @brief key queue initialization
 * 
 * @param fifo pointer to the key event queue structure
 */
void Ebyte_BTN_FIFO_Init(BSP_BTN_FIFO_t *fifo)
{
    fifo->fifoLen   = 0;
    fifo->fifoRead  = 0;
    fifo->fifoWrite = 0;
}

/* !
 * @brief key queue enqueue
 * 
 * @param  fifo pointer to the key event queue structure
 * @param  event The event of the key is mainly the short press/long press of each key
 * @return 0: Normal 1: Queue overflow
 * @note   The enqueue position can be cycled but will not overwrite the enqueue data. When the queue is full, the data will be lost!
 */
uint8_t Ebyte_BTN_FIFO_Push(BSP_BTN_FIFO_t *fifo, BSP_BTN_EVENT_t event)
{
    /* The length of the queued data packet is pre-incremented */
    fifo->fifoLen++;

    /* If the queue length is greater than the set length */
    if(fifo->fifoLen > BSP_BTN_FIFO_LENGTH)
    {
        fifo->fifoLen = BSP_BTN_FIFO_LENGTH;// Entry length no longer increases
        return 1; 
    }
    
    /* normal enqueue */
    fifo->buff[fifo->fifoWrite] = event;
    
    /* If the entry position has reached the end of the queue */
    if(++fifo->fifoWrite >= BSP_BTN_FIFO_LENGTH)
    {
        fifo->fifoWrite = 0; // then the next enqueue data will return to the head of the queue and start enqueuing
    }

    return 0;

}

/* !
 * @brief key queue dequeue
 * 
 * @param fifo pointer to the key event queue structure
 * @param event The event of the key is mainly the short press/long press of each key
 * @return 0: Normal 1: The queue is empty
 */
uint8_t Ebyte_BTN_FIFO_Pop(BSP_BTN_FIFO_t *fifo, BSP_BTN_EVENT_t *event)
{
    /* If the queue length is 0, the queue is empty */
    if(fifo->fifoLen == 0)
    {
        return 1;
    }
    
/* The length of the data in the queue is reduced */
    fifo->fifoLen--;

    /* Dequeue normally */
    *event =  (BSP_BTN_EVENT_t )(fifo->buff[fifo->fifoRead]);

    /* If the dequeue position has reached the end of the queue */
    if(++fifo->fifoRead >= BSP_BTN_FIFO_LENGTH)
    {
        fifo->fifoRead = 0;// then the next time the queue will start from the head of the queue
    }

    return 0;
}
