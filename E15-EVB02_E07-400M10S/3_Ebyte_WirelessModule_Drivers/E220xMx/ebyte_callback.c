/**
  **********************************************************************************
  * @file      ebyte_callback.h
  * @brief     EBYTE��������շ���ɻص����� �ɿͻ�ʵ���Լ����߼�����
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
#include "ebyte_callback.h"

/*= !!!����Ŀ��Ӳ��ƽ̨ͷ�ļ� =======================================*/
#include "board.h"  //E15-EVB02 ������
#include "ebyte_debug.h"  //E15-EVB02 ������
/*= !!!����Ŀ��Ӳ������       =======================================*/
extern void UserTransmitDoneCallback(void);
extern void UserReceiveDoneCallback( uint8_t *buffer, uint8_t length);
/*==================================================================*/



/* !
 * @brief ������ɻص��ӿ� �ɿͻ�ʵ���Լ��ķ�������߼�
 *
 * @param state �ϲ�ص��ṩ��״̬�� �ͻ������ʾ��ע���ҵ���Ӧ����
 *
 * @note E22x ģ����ṩ��״̬��
 *         IRQ_TX_DONE                             = 0x0001,
 *         IRQ_RX_DONE                             = 0x0002,
 *         IRQ_PREAMBLE_DETECTED                   = 0x0004,
 *         IRQ_SYNCWORD_VALID                      = 0x0008,
 *         IRQ_HEADER_VALID                        = 0x0010,
 *         IRQ_HEADER_ERROR                        = 0x0020,
 *         IRQ_CRC_ERROR                           = 0x0040,
 *         IRQ_CAD_DONE                            = 0x0080,
 *         IRQ_CAD_ACTIVITY_DETECTED               = 0x0100,
 *         IRQ_RX_TX_TIMEOUT                       = 0x0200,
 */
void Ebyte_Port_TransmitCallback( uint16e_t state )
{
    /* ����: ������� */
    if( state &= 0x0001 )
    {
        //To-do ʵ���Լ����߼�
        UserTransmitDoneCallback();      
    }
    /* ����: �쳣��ʱ */
    else if ( state &= 0x0200 )
    {
        //To-do ʵ���Լ����߼�
    }
    /* ����: δ֪���� */
    else
    {
        /* ����״̬��ʶ����ȷ��������Ӳ��
           �������� 1:SPIͨ�Ų���ȷ 2:ģ�鹩�粻�� */
        while( 1 );
    }
}

/* !
 * @brief ������ɻص��ӿ� �ɿͻ�ʵ���Լ��ķ�������߼�
 *
 * @param state �ϲ�ص��ṩ��״̬�� �ͻ������ʾ��ע���ҵ���Ӧ����
 *
 */
void Ebyte_Port_ReceiveCallback(  uint16e_t state, uint8e_t *buffer, uint8e_t length )
{
    /* ����: ���� */
    if( state &= 0x0002 )
    {
        //To-do ʵ���Լ����߼�
        UserReceiveDoneCallback( buffer , length);
    }
    /* ����: �쳣��ʱ */
    else if ( state &= 0x0200 )
    {
        //To-do ʵ���Լ����߼�
    }
    /* ����: δ֪���� */
    else
    {
        /* ����״̬��ʶ����ȷ��������Ӳ��
           �������� 1:SPIͨ�Ų���ȷ 2:ģ�鹩�粻�� */
        while( 1 );
    }
}