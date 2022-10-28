/**
  **********************************************************************************
  * @file      ebyte_e19x.h
  * @brief     E19 (sx1278) 系列 驱动库
  * @details   详情请参见 https://www.ebyte.com/
  * @author    JiangHeng
  * @date      2021-05-08
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
#include "ebyte_callback.h"

/**
 * 载波中心频率
 *
 * @note 中国-工业和信息化部
 *
 * 《中华人民共和国无线电频率划分规定》(工业和信息化部令第46号令 2018年7月1日起施行)
 *  规定的无需特别批准ISM应用频段( Sub-1G范围 ):
 *          6.765—6.795MHz （中心频率6.780MHz）
 *         13.553—13.567MHz (中心频率13.560MHz)
 *         26.957—27.283MHz (中心频率27.120MHz)
 *          40.66—40.7MHz   (中心频率40.68MHz）
 *
 * 《微功率(短距离)无线电设备的技术要求》（信部无〔2005〕423号）(工业和信息化部第52号文 2019年)
 *  规定的微功率无线电设备应用频段( Sub-1G范围 ):
 *  A类      9KHz—190KHz
 *  B类    1.7MHz—2.1MHz
 *         2.2MHz—3.0MHz
 *         3.1MHz—4.1MHz
 *         4.2MHz—5.6MHz
 *         5.7MHz—6.2MHz
 *         7.3MHz—8.3MHz
 *         8.4MHz—9.9MHz
 *  C类  6.765MHz—6.795MHz
 *      13.553MHz—13.567MHz
 *      26.957MHz—27.283MHz
 *  D类    315kHz—30MHz
 *  E类  40.66MHz—40.7MHz
 *  -------------------------
 *      26.975MHz—27.255MHz    模型、玩具: 海摸/车模
 *       40.61MHz—40.75MHz     模型、玩具: 海摸/车模
 *       40.77MHz—40.85MHz     模型、玩具: 空模
 *          45MHz—45.475MHz    模拟式电话机: 座机类
 *          48MHz—48.475MHz    模拟式电话机: 手机类
 *       72.13MHz—72.84MHz     模型、玩具: 空模
 *        75.4MHz—76.0MHz      声音传输及计量仪表
 *          84MHz—87MHz        声音传输及计量仪表
 *          87MHz—108MHz       声音传输及计量仪表
 *         174MHz—216MHz       生物医学设备
 *       189.9MHz—223.0MHz     声音传输及计量仪表
 *         223MHz—224MHz       电子吊秤专用
 *         224MHz—231MHz       无线数据传送设备
 *         314MHz—316MHz       民用设备控制
 *         407MHz—425MHz       生物医学设备
 *      409.75MHz—409.9875MHz  公众对讲机
 *     419.950MHz—419.275MHz   工业用无线遥控设备
 *         430MHz—432MHz       民用设备控制
 *         433MHz—434.79MHz    民用设备控制
 *    450.0125MHz—450.2125MHz  民用设备控制
 *         470MHz—510MHz       民用计量设备
 *         470MHz—566MHz       通用无线遥控设备
 *         608MHz—630MHz       生物医学设备
 *         614MHz—787MHz       通用无线遥控设备
 *
 * @note 联合国-国际电信联盟-无线电通信部门(ITU-R)
 *
 * 《Report ITU-R SM.2180: Impact of Industrial, Scientific and Medical (ISM) Equipment on Radiocommunication Services》(2011年1月)
 *  SM.2180报告指定无需特别批准的ISM应用频段( Sub-1G范围 ):
 *          6.765—6.795MHz （中心频率6.780MHz）
 *         13.553—13.567MHz (中心频率13.560MHz)
 *         26.957—27.283MHz (中心频率27.120MHz)
 *          40.66—40.7MHz   (中心频率40.68MHz）
 *         433.05—434.79MHz (中心频率433.92MHz）   限制在ITU 1区使用 (欧洲和非洲以及蒙古、原苏联以北的地区和欧洲、非两洲以外原苏联及土耳其的领土)
 *            902—928MHz    (中心频率915MHz）      限制在ITU 2区使用 (南、北美洲和夏威夷)
 *  中国在ITU 3区(亚洲、大洋洲和两洲以外的伊朗领土)
 *
 * @note 欧盟
 *  《COMMISSION DECISION of 9 November 2006 - on harmonisation of the radio spectrum for use by short-range devices》(2006/771/EC)
 *   欧盟无线电开放频段中的：无需特别批准频段还包含有：
 *          868.0—869.25MHz
 *
 *   如需CE认证，请了解欧盟无线设备指令RED 2014/53/EU
 *
 * @note 美国
 *  《47 CFR Part 18 - INDUSTRIAL, SCIENTIFIC, AND MEDICAL EQUIPMENT》
 *   FCC第18部分规则规定了ISM频段
 *
 *   如需FCC认证，请了解 47 CFR Part 15
 */

/* EBYTE_E19_433M20SC 模块基础通信频率  */
#if defined(EBYTE_E19_433M20SC)
#define   E19X_FREQUENCY_START                          434000000 // Hz 起始频率


#endif


#define   E19X_MODULATION_TYPE         0   //调制方式 0:LoRa   
#define   E19X_OUTPUT_POWER           20   //最大输出 20dBm

#define   E19X_LORA_BANDWIDTH          7   //LoRa调制参数 频宽BW      [ 0: 7.8 kHz, 1: 10.4 kHz, 2: 15.6 kHz, 3: 20.8 kHz, 4: 31.2 kHz 5: 41.6 kHz, 6: 62.5 kHz, 7: 125 kHz, 8: 250 kHz, 9: 500 kHz ]
#define   E19X_LORA_SPREADING_FACTOR   7   //LoRa调制参数 扩频因子SF  [ 6: 64, 7: 128, 8: 256, 9: 512, 10: 1024, 11: 2048, 12: 4096  chips]
#define   E19X_LORA_CORING_RATE        1   //LoRa调制参数 编码率CR    [ 1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8 ]
#define   E19X_LORA_CRC                1   //CRC开关                  [ 0: OFF, 1: ON]
#define   E19X_LORA_LOW_DATARATE       1   //低空速优化开关            [ 0: OFF, 1: ON]
#define   E19X_LORA_PREAMBLE_LENGTH   20   //LoRa前导码长度  
#define   E19X_LORA_SYNC_WORD       0x12   //LoRa同步字       如果考虑采用LoRaWAN 可以修改为公共同步字: 0x34

/* 如何检测模块发送完成了  0:异步中断通知  1:同步阻塞直到发送完成 */
#define EBYTE_TRANSMIT_MODE_BLOCKED   1

uint8e_t E19x_Init( void );
uint8e_t E19x_GoReceive( void );
uint8e_t E19x_GoTransmit( uint8e_t* data, uint8e_t size );
uint8e_t E19x_GoSleep( void );
uint8e_t E19x_TaskForPoll( void );
uint8e_t* E19x_GetName(void);
uint8e_t E19x_GetDriverVersion( void );
uint8e_t E19x_GetStatus( void );
void E19x_TaskForIRQ( void );