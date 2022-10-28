/**
  **********************************************************************************
  * @file      ebyte_port.h
  * @brief     EBYTE驱动 基本数据类型
  * @details   详情请参见 https://www.ebyte.com/
  * @author    JiangHeng
  * @date      2021-05-13
  * @version   1.0.0
  **********************************************************************************
  * @copyright BSD License
  *            成都亿佰特电子科技有限公司
  *   ______   ____   __     __  _______   ______
  *  |  ____| |  _ \  \ \   / / |__   __| |  ____|
  *  | |__    | |_) |  \ \_/ /     | |    | |__
  *  |  __|   |  _ <    \   /      | |    |  __|
  *  | |____  | |_) |    | |       | |    | |____
  *  |______| |____/     |_|       |_|    |______|
  *
  **********************************************************************************
  */

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

uint8e_t Ebyte_Port_Dio0IoRead( void );
uint8e_t Ebyte_Port_SpiTransmitAndReceivce( uint8e_t send );



