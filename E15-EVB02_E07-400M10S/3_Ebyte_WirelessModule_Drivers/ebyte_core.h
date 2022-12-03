/**
  **********************************************************************************
  * @file      ebyte_core.h
  * @brief     The upper API encapsulation layer of the EBYTE driver library helps customers abstract the underlying logic
  * @details See https://www.ebyte.com/ for details
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



#include "ebyte_e07x.h"

typedef struct
{
    uint8e_t   ( *Init )( void );   
    uint8e_t   ( *GoTransmit )( uint8e_t *buffer, uint8e_t size );
    uint8e_t   ( *GoSleep )( void );    
    uint8e_t   ( *GoReceive )( void );  
    uint8e_t   ( *TaskForPoll)( void );
    void       ( *TaskForIRQ )( void );
    uint8e_t   ( *GetStatus )( void );
    uint8e_t * ( *GetName ) (void );
    uint8e_t   ( *GetDriver ) (void );
}Ebyte_RF_t; 

extern const Ebyte_RF_t Ebyte_RF;



