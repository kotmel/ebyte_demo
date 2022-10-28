
/* Includes ------------------------------------------------------------------*/
#include "ebyte_kfifo.h"

uint8_t Ebyte_FIFO_Init( Ebyte_FIFO_t *fifo, uint16_t size )
{
    uint8_t result = 0;

    if( ( size & ( size - 1 ) ) != 0 ) //Warning ! Size must be 2^n ! Please view linux kfifo
    {
        result = 1;
    }

    fifo->size = size;
    fifo->in = 0;
    fifo->out = 0;
    return result;
}

uint8_t Ebyte_FIFO_Write( Ebyte_FIFO_t *fifo, uint8_t *pData, uint16_t length )
{
    uint8_t result = 0;

    uint32_t i, j;
    uint16_t endLength, orgLength;
    uint8_t *pFifoBuffer;

    orgLength = length;
    /* calculate the length of data that can be written */
    length = MIN( length, fifo->size - fifo->in + fifo->out );
    /* first put the data starting from fifo->in to buffer end */
    endLength  = MIN( length, fifo->size - ( fifo->in & ( fifo->size - 1 ) ) );
    pFifoBuffer = fifo->buffer + ( fifo->in & ( fifo->size - 1 ) );
    for( i = 0; i < endLength; i++ )
    {
        *( pFifoBuffer++ ) = *( pData++ );
    }
    /* then put the rest (if any) at the beginning of the buffer */
    j = length - endLength;
    if ( j > 0 )
    {
        pFifoBuffer = fifo->buffer;

        for( i = 0; i < j; i++ )
        {
            *( pFifoBuffer++ ) = *( pData++ );
        }
    }

    fifo->in += length;
    if( length < orgLength )
    {
        result = 1; // Means fifo is full , some data can not be written in
    }
    return result;
}

uint8_t Ebyte_FIFO_Read( Ebyte_FIFO_t *fifo, uint8_t *pData,  uint16_t length )
{
    uint8_t result = 0;
    uint16_t i, j;
    uint16_t endLength, orgLength;
    uint8_t *pFifoBuffer;

    orgLength = length;
    length  = MIN( length, fifo->in - fifo->out );
    /* first get the data from fifo->out until the end of the buffer */
    endLength = MIN( length, fifo->size - ( fifo->out & ( fifo->size - 1 ) ) );
    pFifoBuffer = fifo->buffer + ( fifo->out & ( fifo->size - 1 ) );
    for( i = 0; i < endLength; i++ )
    {
        *( pData++ ) = *( pFifoBuffer++ );
    }
    /* then get the rest (if any) from the beginning of the buffer */
    j = length - endLength;
    if ( j > 0 )
    {
        pFifoBuffer = fifo->buffer;

        for( i = 0; i < j; i++ )
        {
            *( pData++ ) = *( pFifoBuffer++ ) ;
        }
    }
    fifo->out += length;
    if( length < orgLength )
    {
        result = 1; //  not enough data
    }
    return result;
}

uint8_t Ebyte_FIFO_GetDataLength( Ebyte_FIFO_t *fifo, uint16_t *pLength )
{
    uint8_t result = 0;
    *pLength = ( fifo->in - fifo->out );
    return result;
}

uint8_t Ebyte_FIFO_Clear( Ebyte_FIFO_t *fifo )
{
    uint8_t result = 0;
    fifo->in = 0;
    fifo->out = 0;
    return result;
}

