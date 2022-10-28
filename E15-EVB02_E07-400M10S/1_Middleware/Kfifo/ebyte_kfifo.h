#include "board.h"

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define EBYTE_FIFO_SIZE 64  //  Warning ! Size must be 2^n ! Please view linux kfifo

typedef struct
{
    uint32_t in;
    uint32_t out;
    uint32_t size;
    uint8_t buffer[EBYTE_FIFO_SIZE];

} Ebyte_FIFO_t;

uint8_t Ebyte_FIFO_Init( Ebyte_FIFO_t *fifo, uint16_t size );
uint8_t Ebyte_FIFO_Write( Ebyte_FIFO_t *fifo, uint8_t *pData, uint16_t length );
uint8_t Ebyte_FIFO_GetDataLength( Ebyte_FIFO_t *fifo, uint16_t *pLength );
uint8_t Ebyte_FIFO_Read( Ebyte_FIFO_t *fifo, uint8_t *pData,  uint16_t length );
uint8_t Ebyte_FIFO_Clear( Ebyte_FIFO_t *fifo );
