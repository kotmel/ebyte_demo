/**
  **********************************************************************************
  * @file      board_ebyte_E22xx.h
  * @brief     E49 系列 驱动库     
  * @details   详情请参见 https://www.ebyte.com/       
  * @author    JiangHeng     
  * @date      2021-05-08     
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

/**
 * @note 配置 E49-400M20S 频率
 *       以 410.000 MHz作为起始频点
 */
#if defined(EBYTE_E49_400M20S )
   #define EBYTE_E49x_FREQUENCY_STEP 200   //通道步进 2.5KHz*200 = 500KHz = 0.5MHz
   #define EBYTE_E49x_FREQUENCY_CHANNEL 46 //通道步进累积  那么最终频率为 410.0MHz + 0.5MHz*46  = 433.0MHz
   
/**
 * @note 配置 E49-900M20S 频率
 *       以 850.000 MHz作为起始频点
 */
#elif defined(EBYTE_E49_900M20S)
   #define EBYTE_E49x_FREQUENCY_STEP 200   //通道步进 2.5KHz*200 = 500KHz = 0.5MHz
   #define EBYTE_E49x_FREQUENCY_CHANNEL 0 //通道步进累积  那么最终频率为 850.0MHz + 0.5MHz*36  = 868.0MHz
#endif

/* 如何检测模块发送完成了  0:异步中断通知  1:同步阻塞直到发送完成 */
#define EBYTE_TRANSMIT_MODE_BLOCKED   0     
   
/* 配置发送完成时 产生中断的GPIO */
#define TRANSMIT_IRQ_GPIO  2  //1:GPIO1  2:GPIO2  3:GPIO3
   
/* 配置接收完成时 产生中断的GPIO */
#define RECEIVE_IRQ_GPIO   2  //1:GPIO1  2:GPIO2  3:GPIO3 
   
 
   
uint8e_t E49x_Init(void);
uint8e_t E49x_GoTransmit( uint8e_t *data, uint8e_t size );
uint8e_t E49x_GoReceive(void);    
uint8e_t E49x_GoSleep(void);
uint8e_t E49x_TaskForPoll(void);
uint8e_t E49x_GetStatus(void);
uint8e_t E49x_GetDriverVersion(void);
uint8e_t* E49x_GetName(void);
void E49x_TaskForIRQ(void);