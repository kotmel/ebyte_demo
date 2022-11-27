/**
  **********************************************************************************
  * @file board.c
  * @brief E15-EVB02 detection mode
  * @details See https://www.ebyte.com/ for details
  * @author JiangHeng
  * @date 2021-05-20
  * @version 1.0.0
  **********************************************************************************
  * @copyright BSD License 
  *            Chengdu Ebyte Electronic Technology Co., Ltd.
  *   ______   ____   __     __  _______   ______ 
  *  |  ____| |  _ \  \ \   / / |__   __| |  ____|
  *  | |__    | |_) |  \ \_/ /     | |    | |__   
  *  |  __|   |  _ <    \   /      | |    |  __|  
  *  | |____  | |_) |    | |       | |    | |____ 
  *  |______| |____/     |_|       |_|    |______| 
  *
  **********************************************************************************
  */

#include "ebyte_core.h"
#include "ebyte_debug.h"

extern unsigned char PC_isConnected;

const unsigned char SimulatedCommandsButton1[5] = { EBYTE_CMD_PACKAGE_START,
                                                    EBYTE_CMD_PACKAGE_START,
                                                    EBYTE_CMD_PACKAGE_START, 
                                                    EBYTE_CMD_TEST_BUTTON,
                                                    0x01
                                                    };

const unsigned char SimulatedCommandsButton2[5] = { EBYTE_CMD_PACKAGE_START,
                                                    EBYTE_CMD_PACKAGE_START,
                                                    EBYTE_CMD_PACKAGE_START, 
                                                    EBYTE_CMD_TEST_BUTTON,
                                                    0x02
                                                    };
const unsigned char SimulatedCommandsWireless1[5] = { EBYTE_CMD_PACKAGE_START,
                                                      EBYTE_CMD_PACKAGE_START,
                                                      EBYTE_CMD_PACKAGE_START, 
                                                      EBYTE_CMD_TEST_WIRELESS,
                                                      0x01
                                                      };      
const unsigned char SimulatedCommandsWireless2[5] = { EBYTE_CMD_PACKAGE_START,
                                                      EBYTE_CMD_PACKAGE_START,
                                                      EBYTE_CMD_PACKAGE_START, 
                                                      EBYTE_CMD_TEST_WIRELESS,
                                                      0x02
                                                      };   


/* !
* @brief test command check
*
* @param rxBuffer may contain command packets
* @param length rxBuffer data length
* @param txBuffer response data packet length>10
* @param tLength response packet length
* @return 0: command not detected 1: command detected
*/
unsigned char  Ebyte_DEBUG_CommandEcho( unsigned char *rxBuffer , unsigned char length, unsigned char *txBuffer, unsigned char *tLength)
{
    unsigned char result = 0;
    unsigned char *p;
    unsigned char tmp,version;
    unsigned char *nameString;
    unsigned long nameHex = 0;
    
    /* Only care about the data frame with a fixed length of 10. The chance of sticking packets is very small */
    if( length == EBYTE_CMD_PACKAGE_LENGTH )
    {
        p = rxBuffer;
        
        if( *p++== EBYTE_CMD_PACKAGE_START &&  *p++== EBYTE_CMD_PACKAGE_START &&  *p++== EBYTE_CMD_PACKAGE_START)
        {
            /* The first byte instruction code of the data body */
            tmp = *p;
            switch( tmp )
            {
              case EBYTE_CMD_TEST_MODE:
                
                  nameString = Ebyte_RF.GetName();
                  version = Ebyte_RF.GetDriver();
                  if( nameString[4]=='-' )// Split by symbol '-'
                  {
                      /* 4-bit encoding such as E220 = 0x0220 */
                      nameHex |= ((unsigned long)(nameString[1]-0x30)<<24);
                      nameHex |= ((unsigned long)(nameString[2]-0x30)<<20);
                      nameHex |= ((unsigned long)(nameString[3]-0x30)<<16);  
                      /* frequency band such as 400 = 0x0400 */
                      nameHex |= ((unsigned long)(nameString[5]-0x30)<<8);  
                      nameHex |= ((unsigned long)(nameString[6]-0x30)<<4); 
                      nameHex |=  (nameString[7]-0x30);
                      
                  }else
                  {
                      /* 3-bit encoding such as E22 = 0x0022 */
                      nameHex |= ((unsigned long)(nameString[1]-0x30)<<20);
                      nameHex |= ((unsigned long)(nameString[2]-0x30)<<16);  
                      /* 3-bit encoding such as E22 = 0x0022 */
                      nameHex |= ((unsigned long)(nameString[4]-0x30)<<8);  
                      nameHex |= ((unsigned long)(nameString[5]-0x30)<<4); 
                      nameHex |=  (nameString[6]-0x30);                      
                  }
           

                  *txBuffer++ = EBYTE_CMD_PACKAGE_START;
                  *txBuffer++ = EBYTE_CMD_PACKAGE_START;
                  *txBuffer++ = EBYTE_CMD_PACKAGE_START;
                  *txBuffer++ = EBYTE_CMD_TEST_MODE;
                  *txBuffer++ = (unsigned char)(nameHex>>24);
                  *txBuffer++ = (unsigned char)(nameHex>>16);
                  *txBuffer++ = (unsigned char)(nameHex>>8);
                  *txBuffer++ = (unsigned char)(nameHex);
                  *txBuffer++ = version;
                  
                  *tLength    = EBYTE_CMD_PACKAGE_LENGTH;
                  PC_isConnected = 1;  // mode change
              break;
            case EBYTE_CMD_TEST_BUTTON:
                  *txBuffer++ = EBYTE_CMD_PACKAGE_START;
                  *txBuffer++ = EBYTE_CMD_PACKAGE_START;
                  *txBuffer++ = EBYTE_CMD_PACKAGE_START;
                  *txBuffer++ = EBYTE_CMD_TEST_BUTTON;
                  *txBuffer   = *++p;
                  *tLength    = EBYTE_CMD_PACKAGE_LENGTH;
              break;
            case EBYTE_CMD_TEST_WIRELESS:
                  *txBuffer++ = EBYTE_CMD_PACKAGE_START;
                  *txBuffer++ = EBYTE_CMD_PACKAGE_START;
                  *txBuffer++ = EBYTE_CMD_PACKAGE_START;
                  *txBuffer++ = EBYTE_CMD_TEST_WIRELESS;
                  *txBuffer   = *++p;
                  *tLength    = EBYTE_CMD_PACKAGE_LENGTH;
              break;
            }
            
            result = 1;
        }
    }
    
    return result;
}