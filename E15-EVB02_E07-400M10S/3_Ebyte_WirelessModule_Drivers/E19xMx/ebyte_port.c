/**
  **********************************************************************************
  * @file      ebyte_port.h
  * @brief     EBYTE驱动库的硬件接口层 由客户自己提供IO接口
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
#include "ebyte_port.h"


/*= !!!配置目标硬件平台头文件 =======================================*/
#include "board.h"  //E15-EVB02 评估板
/*==================================================================*/

/*!
 * @brief 配置目标硬件平台SPI接口收发函数
 *
 * @param send EBYTE驱动库上层调用需要传输的数据 1 Byte
 * @return SPI接收的数据 1 Byte
 */
uint8e_t Ebyte_Port_SpiTransmitAndReceivce( uint8e_t send )
{
    uint8e_t result = 0;
    /*!必须提供: SPI接口 */
    result = Ebyte_BSP_SpiTransAndRecv( send );//示例
    return result;
}

/*!
 * @brief 配置目标硬件平台SPI_CS(NSS)函数
 *
 * @param cmd EBYTE驱动库上层期望的控制指令
 *            @arg 0: 期望CS(NSS)引脚输出低电平逻辑信号 EBYTE模块SPI接口为低电平选中
 *            @arg 1: 期望CS(NSS)引脚输出高电平逻辑信号
 */
void Ebyte_Port_SpiCsIoControl( uint8e_t cmd )
{
    if( cmd == 1 )
    {
        /* SPI CS控制 高电平未选中 */
        Ebyte_BSP_RfSpiUnselected();//示例
    }
    else
    {
        /* SPI CS控制 低电平选中 */
        Ebyte_BSP_RfSpiSelected( );//示例
    }
}

/*!
 * @brief 配置目标硬件平台复位引脚 NRST 控制函数
 *
 * @param cmd EBYTE驱动库上层期望的控制指令
 *            @arg 0: 期望NRST 引脚输出低电平逻辑信号
 *            @arg 1: 期望NRST 引脚输出高电平逻辑信号
 * @note EBYTE模块NRST为低电平复位
 */
void Ebyte_Port_RstIoControl( uint8e_t cmd )
{
    if( cmd == 1 )
    {
        /*!必须提供: NRST控制IO 输出高电平 */
        Ebyte_BSP_E19ResetIoHigh();//示例
    }
    else
    {
        /*!必须提供: NRST控制IO 输出低电平 */
        Ebyte_BSP_E19ResetIoLow();//示例
    }
}

/*!
 * @brief (可选)配置目标硬件平台发送使能引脚 TXEN 控制函数
 *
 * @param cmd EBYTE驱动库上层期望的控制指令
 *            @arg 0: 期望TXEN 引脚输出低电平逻辑信号
 *            @arg 1: 期望TXEN 引脚输出高电平逻辑信号
 */
void Ebyte_Port_TxenIoControl( uint8e_t cmd )
{
    if( cmd == 1 )
    {
        /*!可选: 目标硬件电路对应TXEN控制IO 输出高电平 */
        Ebyte_BSP_E19TxIoEnable();//示例
    }
    else
    {
        /*!可选: 目标硬件电路对应TXEN控制IO 输出低电平 */
        Ebyte_BSP_E19TxIoDisable();//示例
    }
}

/*!
 * @brief 配置目标硬件平台接收使能引脚 RXEN 控制函数
 *
 * @param cmd EBYTE驱动库上层期望的控制指令
 *            @arg 0: 期望RXEN引脚输出低电平逻辑信号
 *            @arg 1: 期望RXEN引脚输出高电平逻辑信号
 * @note EBYTE模块RXEN为高电平生效
 */
void Ebyte_Port_RxenIoControl( uint8e_t cmd )
{
    if( cmd == 1 )
    {
        /*!必须提供: 目标硬件电路对应RXEN控制IO 输出高电平 */
        Ebyte_BSP_E19RxIoEnable();//示例
    }
    else
    {
        /*!必须提供: 目标硬件电路对应RXEN控制IO 输出低电平 */
        Ebyte_BSP_E19RxIoDisable();//示例
    }
}

/*!
 * @brief 配置目标硬件平台 同样引脚DIO0 读取函数
 */
uint8e_t Ebyte_Port_Dio0IoRead( void )
{
    uint8e_t result = 0 ;
    /*!必须提供: 目标硬件电路对应DIO0 IO 状态读取 */
    result = Ebyte_BSP_E19Dio0Read();//示例
    return result;
}

/*!
 * @brief 配置目标硬件平台延时函数
 *
 * @param time 期望延时毫秒
 */
void Ebyte_Port_DelayMs( uint16e_t time )
{
    /*!必须提供: 延时函数 */
    uint16e_t i, n;
    while( time-- ) //这里示例了STM8L 16M时钟前提下的粗略软件延时
    {
        for( i = 900; i > 0; i-- )
        {
            for( n = 1 ; n > 0 ; n-- )
            {
                asm( "nop" );
                asm( "nop" );
                asm( "nop" );
                asm( "nop" );
            }
        }
    }
}



