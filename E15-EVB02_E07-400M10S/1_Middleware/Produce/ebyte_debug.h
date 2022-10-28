

#define EBYTE_CMD_PACKAGE_START 0xC5
#define EBYTE_CMD_PACKAGE_LENGTH 0x0A
#define EBYTE_CMD_TEST_MODE 0x01
#define EBYTE_CMD_TEST_BUTTON 0x02
#define EBYTE_CMD_TEST_WIRELESS 0x03

extern const unsigned char SimulatedCommandsButton1[5];
extern const unsigned char SimulatedCommandsButton2[5];
extern const unsigned char SimulatedCommandsWireless1[5];
extern const unsigned char SimulatedCommandsWireless2[5];

unsigned char  Ebyte_DEBUG_CommandEcho( unsigned char *rxBuffer , unsigned char length, unsigned char *txBuffer, unsigned char *tLength);