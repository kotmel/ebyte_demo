/**
  **********************************************************************************
  * @file      ebyte_core.c
  * @brief     The upper API encapsulation layer of the EBYTE driver library abstracts the underlying logic
  * @details   See https://www.ebyte.com/ for details
  * @author    JiangHeng     
  * @date      2021-05-13     
  * @version   1.0.0     
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

/* ָ�� E22x �ײ��������� */
#if defined(EBYTE_E22_400M22S)||defined(EBYTE_E22_900M22S)
const Ebyte_RF_t Ebyte_RF =
{
  E22x_Init,
  E22x_GoTransmit,
  E22x_GoSleep,
  E22x_GoReceive,  
  E22x_TaskForPoll,
  E22x_TaskForIRQ,
  0,
  E22x_GetName,
  E22x_GetDriverVersion
};

/* ָ�� E220x �ײ��������� */
#elif defined(EBYTE_E220_400M22S)||defined(EBYTE_E220_900M22S)
const Ebyte_RF_t Ebyte_RF =
{
  E220x_Init,
  E220x_GoTransmit,
  E220x_GoSleep,
  E220x_GoReceive,
  E220x_TaskForPoll,
  E220x_TaskForIRQ,
  0,
  E220x_GetName,
  E220x_GetDriverVersion
};

/* point to E07x underlying driver function */
#elif defined(EBYTE_E07_400M10S)||defined(EBYTE_E07_900M10S)
const Ebyte_RF_t Ebyte_RF =
{
  E07x_Init,
  E07x_GoTransmit,
  E07x_GoSleep,
  E07x_GoReceive,  
  E07x_TaskForPoll,
  E07x_TaskForIRQ,
  E07x_GetStatus,
  E07x_GetName,
  E07x_GetDriverVersion
};

/* ָ�� E10x �ײ��������� */
#elif defined(EBYTE_E10_400M20S)
const Ebyte_RF_t Ebyte_RF =
{
  E10x_Init,
  E10x_GoTransmit,
  E10x_GoSleep,
  E10x_GoReceive,  
  E10x_TaskForPoll,
  E10x_TaskForIRQ,
  E10x_GetStatus,
  E10x_GetName,
  E10x_GetDriverVersion
};

/* ָ�� E30x �ײ��������� */
#elif defined(EBYTE_E30_900M20S)
const Ebyte_RF_t Ebyte_RF =
{
  E30x_Init,
  E30x_GoTransmit,
  E30x_GoSleep,
  E30x_GoReceive,  
  E30x_TaskForPoll,
  E30x_TaskForIRQ,
  E30x_GetStatus,
  E30x_GetName,
  E30x_GetDriverVersion
};

/* ָ�� E31x �ײ��������� */
#elif defined(EBYTE_E31_400M17S)||defined(EBYTE_E31_900M17S)
const Ebyte_RF_t Ebyte_RF =
{
  E31x_Init,
  E31x_GoTransmit,
  E31x_GoSleep,
  E31x_GoReceive,  
  E31x_TaskForPoll,
  E31x_TaskForIRQ,
  E31x_GetStatus,
  E31x_GetName,
  E31x_GetDriverVersion
};

/* ָ�� E49x �ײ��������� */
#elif defined(EBYTE_E49_400M20S)||defined(EBYTE_E49_900M20S)
const Ebyte_RF_t Ebyte_RF =
{
  E49x_Init,
  E49x_GoTransmit,
  E49x_GoSleep,
  E49x_GoReceive,  
  E49x_TaskForPoll,
  E49x_TaskForIRQ,
  E49x_GetStatus,
  E49x_GetName,
  E49x_GetDriverVersion
};
#elif defined(EBYTE_E19_433M20SC)
const Ebyte_RF_t Ebyte_RF =
{
  E19x_Init,
  E19x_GoTransmit,
  E19x_GoSleep,
  E19x_GoReceive,  
  E19x_TaskForPoll,
  E19x_TaskForIRQ,
  E19x_GetStatus,
  E19x_GetName,
  E19x_GetDriverVersion
};
#else
/* ebyte_conf.h ������ѡ��Ĳ�Ʒ�ͺŲ���ȷ */
#error No product selected !
#endif






