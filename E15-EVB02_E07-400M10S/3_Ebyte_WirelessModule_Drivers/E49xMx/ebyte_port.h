typedef  unsigned  char   uint8e_t;
typedef  unsigned  short  uint16e_t;
typedef  unsigned  long   uint32e_t;

typedef  signed char     int8e_t;
typedef  signed short    int16e_t;
typedef  signed long     int32e_t;

uint8e_t Ebyte_Port_SdioIoControl( uint8e_t cmd );
void Ebyte_Port_SlckIoControl( uint8e_t cmd );
void Ebyte_Port_CsbIoControl( uint8e_t cmd );
void Ebyte_Port_FcsbIoControl( uint8e_t cmd );
void Ebyte_Port_DelayUs( uint16e_t time );




