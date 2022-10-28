#include "ebyte_callback.h"

#if defined(EBYTE_E31_400M17S)
#define E31X_FREQUENCY_START  433300000 //433.3 MHz
#elif defined(EBYTE_E31_900M17S)
#define E31X_FREQUENCY_START  868300000 //868.3 MHz
#endif

#define E31X_OUTPUT_POWER   0x0FFF    //默认最大0x0FFF 即17dBm 
#define E31X_PREAMBLE_SIZE    0x08    //前导码长度 字节数 8x8=64bit
#define E31X_SYNC_WORD  0xAACCAACC    //默认4byte 同步字 注意为LSB 0xAACCAACC实际上是0x33553355(MSB)
#define E31X_IS_CRC              1    //CRC 开关  [0:关闭 1:打开] 


uint8e_t E31x_Init(void);
uint8e_t E31x_GoReceive(void);
uint8e_t E31x_GoTransmit( uint8e_t *data, uint8e_t size );
uint8e_t E31x_GoSleep(void);
uint8e_t E31x_TaskForPoll( void );
uint8e_t E31x_GetDriverVersion(void);
uint8e_t* E31x_GetName(void);
uint8e_t E31x_GetStatus(void);
void E31x_TaskForIRQ( void );
