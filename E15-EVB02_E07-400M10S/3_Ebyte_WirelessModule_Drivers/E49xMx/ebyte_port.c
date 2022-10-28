/**
  **********************************************************************************
  * @file      ebyte_port.h
  * @brief     EBYTE驱动库的硬件接口层 由客户自己提供IO接口    
  * @details   详情请参见 https://www.ebyte.com/       
  * @author    JiangHeng     
  * @date      2021-05-13     
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
#include "ebyte_port.h"


/*= !!!配置目标硬件平台头文件 =======================================*/
#include "board.h"  //E15-EVB02 评估板
/*==================================================================*/



uint8e_t Ebyte_Port_SdioIoControl( uint8e_t cmd )
{
    uint8e_t result = 0;
    
    switch(cmd)  
    {
        case 0: 
          Ebyte_BSP_E49SdioIoLow();
          break;
          
        case 1: 
          Ebyte_BSP_E49SdioIoHigh();
          break;
          
        case 2: 
          Ebyte_BSP_E49SdioIoInput();
          break;
          
        case 3: 
          Ebyte_BSP_E49SdioIoOutput();
          break;
          
        case 4: 
          result = Ebyte_BSP_E49SdioIoRead();
          break;
          
        default: break;
    }

    return result; 
}

void Ebyte_Port_SlckIoControl( uint8e_t cmd )
{
    if ( cmd == 1 )
    {
        Ebyte_BSP_E49SlckIoHigh();
    }
    else
    {
        Ebyte_BSP_E49SlckIoLow();
    }
}

void Ebyte_Port_CsbIoControl( uint8e_t cmd )
{
    if ( cmd == 1 )
    {
        Ebyte_BSP_E49CsbIoHigh();
    }
    else
    {
        Ebyte_BSP_E49CsbIoLow();
    }
}

void Ebyte_Port_FcsbIoControl( uint8e_t cmd )
{
    if ( cmd == 1 )
    {
        Ebyte_BSP_E49FcsbIoHigh();
    }
    else
    {
        Ebyte_BSP_E49FcsbIoLow();
    }
}

void Ebyte_Port_DelayUs( uint16e_t time )
{
    /* !必须提供: 延时函数 */
    
    while(time--)//这里示范了STM8L 16M时钟前提下的粗略软件延时 
    {
        nop();nop();nop();nop();
    }    
}



