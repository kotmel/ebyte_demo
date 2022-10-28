#include "ebyte_callback.h"

uint8e_t E30x_Init(void);
uint8e_t E30x_GoReceive(void);
uint8e_t E30x_GoTransmit( uint8e_t *data, uint8e_t size );
uint8e_t E30x_GoSleep(void);
uint8e_t E30x_TaskForPoll( void );
uint8e_t E30x_GetDriverVersion(void);
uint8e_t* E30x_GetName(void);
uint8e_t E30x_GetStatus(void);
void E30x_TaskForIRQ( void );
