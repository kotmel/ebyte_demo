
#include "ebyte_callback.h"

uint8e_t E10x_Init(void);
uint8e_t E10x_GoReceive(void);
uint8e_t E10x_GoTransmit( uint8e_t *data, uint8e_t size );
uint8e_t E10x_GoSleep(void);
uint8e_t E10x_TaskForPoll( void );
uint8e_t E10x_GetDriverVersion(void);
uint8e_t* E10x_GetName(void);
uint8e_t E10x_GetStatus(void);
void E10x_TaskForIRQ( void );
