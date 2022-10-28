#include "ebyte_callback.h"


#if defined(EBYTE_E07_400M10S)
#define E07_FREQUENCY_START     433000000  //433 MHz
#elif defined(EBYTE_E07_900M10S)  
#define E07_FREQUENCY_START     915000000  //915 MHz
#else
#error Please define the type of product you use ！ 
#endif

#define E07_DATA_RATE           1200       //1.2 KBps
#define E07_FREQUENCY_DEVIATION 14300      //14.3 K
#define E07_BANDWIDTH           58000      //58 K
#define E07_OUTPUT_POWER        10         //只支持[10  7  5  0  -10  -15  -20 -30] 需要其他功率需要自行使用频谱仪校准
#define E07_PREAMBLE_SIZE       4          //前导码长度 8字节即8x8=64bit   [0:2 1:3 2:4 3:6 4:8 5:12 6:16 7:24]
#define E07_SYNC_WORD           0x2DD4     //同步字 2个字节
#define E07_IS_CRC              1          //CRC开关  [0:关闭  1:开启]

uint8e_t E07x_Init( void );
uint8e_t E07x_TaskForPoll(void);
uint8e_t E07x_GoTransmit( uint8e_t *data, uint8e_t size );
uint8e_t E07x_GoReceive( void );
uint8e_t E07x_GoSleep(void);
uint8e_t E07x_GetStatus(void);
uint8e_t E07x_GetDriverVersion(void);
uint8e_t* E07x_GetName(void);
void E07x_TaskForIRQ(void);
