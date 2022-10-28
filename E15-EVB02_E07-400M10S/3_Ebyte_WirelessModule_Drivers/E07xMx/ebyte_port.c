/**
  **********************************************************************************
  * @file      ebyte_port.h
  * @brief     EBYTE�������Ӳ���ӿڲ� �ɿͻ��Լ��ṩE07 IO�ӿ�    
  * @details   ������μ� https://www.ebyte.com/       
  * @author    JiangHeng     
  * @date      2021-05-13     
  * @version   1.0.0     
  **********************************************************************************
  * @copyright BSD License 
  *            �ɶ��ڰ��ص��ӿƼ����޹�˾  
  *   ______   ____   __     __  _______   ______ 
  *  |  ____| |  _ \  \ \   / / |__   __| |  ____|
  *  | |__    | |_) |  \ \_/ /     | |    | |__   
  *  |  __|   |  _ <    \   /      | |    |  __|  
  *  | |____  | |_) |    | |       | |    | |____ 
  *  |______| |____/     |_|       |_|    |______| 
  *
  **********************************************************************************
  */
#include "ebyte_port.h"


/*= !!!����Ŀ��Ӳ��ƽ̨ͷ�ļ� =======================================*/
#include "board.h"  //E15-EVB02 ������
/*==================================================================*/

/*!
 * @brief ����Ŀ��Ӳ��ƽ̨SPI�ӿ��շ����� 
 * 
 * @param send EBYTE�������ϲ������Ҫ��������� 1 Byte 
 * @return SPI���յ����� 1 Byte
 */
uint8e_t Ebyte_Port_SpiTransmitAndReceivce( uint8e_t send )
{
    uint8e_t result = 0;
    
    /*!�����ṩ: SPI�ӿ� */
    result = Ebyte_BSP_SpiTransAndRecv( send );//ʾ��
    
    return result;
}

/*!
 * @brief (��ѡ)����Ŀ��Ӳ��ƽ̨SPI_CS(NSS)���� 
 * 
 * @param cmd EBYTE�������ϲ������Ŀ���ָ��
 *            @arg 0: ����CS(NSS)��������͵�ƽ�߼��ź� EBYTEģ��SPI�ӿ�Ϊ�͵�ƽѡ��
 *            @arg 1: ����CS(NSS)��������ߵ�ƽ�߼��ź�
 */
void Ebyte_Port_SpiCsIoControl( uint8e_t cmd )
{
    if ( cmd == 1 )
    {
       /*!��ѡ: SPI CS���� E22xx�ߵ�ƽδѡ�� */  
       Ebyte_BSP_RfSpiUnselected(); 
    }
    else
    {
       /*!��ѡ: SPI CS���� E22xx�͵�ƽѡ�� */  
       Ebyte_BSP_RfSpiSelected( );
    }
}


/*!
 * @brief ����Ŀ��Ӳ��ƽ̨����ʹ������ GDO 0 ���ƺ��� 
 * 
 * @note GDO0����Ϊ�����������ź�������ţ�һ����ΪFIFO״ָ̬ʾ�ź�
 *       
 */
uint8e_t Ebyte_Port_Gdo0IoRead( void )
{
    uint8e_t result = 0 ;  
    
    /*!�����ṩ: Ŀ��Ӳ����·��ӦBUSY IO ״̬��ȡ */
    result = Ebyte_BSP_RfGdo0IoRead();
    return result;
}

/*!
 * @brief ����Ŀ��Ӳ��ƽ̨����ʹ������ GDO 1 ���ƺ���      
 */
uint8e_t Ebyte_Port_Gdo1IoRead( void )
{
    uint8e_t result = 0 ;  
    
    /*!�����ṩ: Ŀ��Ӳ����·��ӦBUSY IO ״̬��ȡ */
    result = Ebyte_BSP_RfGdo0IoRead();
    return result;
}


/*!
 * @brief ����Ŀ��Ӳ��ƽ̨��ʱ����
 * 
 * @param time ������ʱ���� 
 * @note ��ע��ʱ��ģ���ʼ��ʱ�����ô˺�������ע���ж��Ƿ��Ӱ�쵽�˺���
 */
void Ebyte_Port_DelayMs( uint16e_t time )
{
    /*!�����ṩ: ��ʱ���� */
  
    uint16e_t i,n;
    
    while(time--)//����ʾ����STM8L 16Mʱ��ǰ���µĴ���������ʱ 
    {
        for(i=900;i>0;i--)
        {
              for( n=1 ;n>0 ;n--)
              {
				#ifdef __CSMC__
				  __asm("nop"); 
                  __asm("nop");
                  __asm("nop");
                  __asm("nop");
                #elif defined(_SDCC_)
                  __asm__("nop"); 
                  __asm__("nop");
                  __asm__("nop");
                  __asm__("nop");
				#else
                  asm("nop"); 
                  asm("nop");
                  asm("nop");
                  asm("nop");
				#endif
              }
        }
    }    
}



