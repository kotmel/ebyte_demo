
typedef  unsigned  char   uint8e_t;
typedef  unsigned  short  uint16e_t;
typedef  unsigned  long   uint32e_t;

typedef  signed char     int8e_t;
typedef  signed short    int16e_t;
typedef  signed long     int32e_t;

void Ebyte_Port_RstIoControl( uint8e_t cmd );
void Ebyte_Port_TxenIoControl( uint8e_t cmd );
void Ebyte_Port_RxenIoControl( uint8e_t cmd );
void Ebyte_Port_DelayMs( uint16e_t time );
void Ebyte_Port_SpiCsIoControl( uint8e_t cmd );

uint8e_t Ebyte_Port_BusyIoRead( void );
uint8e_t Ebyte_Port_SpiTransmitAndReceivce( uint8e_t send );



