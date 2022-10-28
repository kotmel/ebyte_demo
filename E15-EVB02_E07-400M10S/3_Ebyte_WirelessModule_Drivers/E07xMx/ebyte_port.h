typedef  unsigned  char   uint8e_t;
typedef  unsigned  short  uint16e_t;
typedef  unsigned  long   uint32e_t;

typedef  signed char     int8e_t;
typedef  signed short    int16e_t;
typedef  signed long     int32e_t;

void Ebyte_Port_DelayMs( uint16e_t time );
void Ebyte_Port_SpiCsIoControl( uint8e_t cmd );

uint8e_t Ebyte_Port_Gdo0IoRead( void );
uint8e_t Ebyte_Port_Gdo1IoRead( void );
uint8e_t Ebyte_Port_SpiTransmitAndReceivce( uint8e_t send );



