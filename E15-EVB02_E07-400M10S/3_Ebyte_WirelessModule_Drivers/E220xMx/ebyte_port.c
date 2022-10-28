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

/* !
 * @brief 配置目标硬件平台SPI接口收发函数 
 * 
 * @param send EBYTE驱动库上层调用需要传输的数据 1 Byte 
 * @return SPI接收的数据 1 Byte
 * @note 当目标平台采用硬件SPI_NSS时，请配置 ebyte_conf.h文件 关闭驱动库的软件控制SPI_CS(NSS)
 *       当目标平台采用软件SPI_NSS时，还需额外配置 Ebyte_Port_SpiCsIoControl() 具体请查阅函数说明
 */
uint8e_t Ebyte_Port_SpiTransmitAndReceivce( uint8e_t send )
{
    uint8e_t result = 0;
    
    /* !必须提供: SPI接口 */
    result = Ebyte_BSP_SpiTransAndRecv( send );
    
    return result;
}

/* !
 * @brief (可选)配置目标硬件平台SPI_CS(NSS)函数 
 * 
 * @param cmd EBYTE驱动库上层期望的控制指令
 *            @arg 0: 期望CS(NSS)引脚输出低电平逻辑信号 EBYTE模块SPI接口为低电平选中
 *            @arg 1: 期望CS(NSS)引脚输出高电平逻辑信号
 * @note 当目标平台采用硬件SPI_NSS时，由配置文件定义 此函数将无效
 *       当目标平台采用软件SPI_NSS时，请配置此函数
 */
void Ebyte_Port_SpiCsIoControl( uint8e_t cmd )
{
    if ( cmd == 1 )
    {
       /* !可选: SPI CS控制 E22xx高电平未选中 */  
       Ebyte_BSP_RfSpiUnselected(); 
    }
    else
    {
       /* !可选: SPI CS控制 E22xx低电平选中 */  
       Ebyte_BSP_RfSpiSelected( );
    }
}

/* !
 * @brief 配置目标硬件平台复位引脚 NRST 控制函数 
 * 
 * @param cmd EBYTE驱动库上层期望的控制指令
 *            @arg 0: 期望NRST 引脚输出低电平逻辑信号 
 *            @arg 1: 期望NRST 引脚输出高电平逻辑信号
 * @note EBYTE模块NRST为低电平复位
 */
void Ebyte_Port_RstIoControl( uint8e_t cmd )
{
    if ( cmd == 1 )
    {
        /* !必须提供: NRST控制IO 输出高电平 */
        Ebyte_BSP_RfResetIoHigh();
    }
    else
    {
        /* !必须提供: 目标硬件电路对应NRST控制IO 输出低电平 */
        Ebyte_BSP_RfResetIoLow();
    }
}

/* !
 * @brief (可选)配置目标硬件平台发送使能引脚 TXEN 控制函数 
 * 
 * @param cmd EBYTE驱动库上层期望的控制指令
 *            @arg 0: 期望TXEN 引脚输出低电平逻辑信号 
 *            @arg 1: 期望TXEN 引脚输出高电平逻辑信号 
 * @note EBYTE模块TXEN为高电平生效
 *       当目标平台采用模块DIO2自动控制TXEN时，此函数将无效
 */
void Ebyte_Port_TxenIoControl( uint8e_t cmd )
{
    if ( cmd == 1 )
    {
        /* !可选: 目标硬件电路对应TXEN控制IO 输出高电平 */
        Ebyte_BSP_RfTxIoEnable();
    }
    else
    {
        /* !可选: 目标硬件电路对应TXEN控制IO 输出低电平 */
        Ebyte_BSP_RfTxIoDisable();
    }
}

/* !
 * @brief 配置目标硬件平台接收使能引脚 RXEN 控制函数 
 * 
 * @param cmd EBYTE驱动库上层期望的控制指令
 *            @arg 0: 期望RXEN引脚输出低电平逻辑信号 
 *            @arg 1: 期望RXEN引脚输出高电平逻辑信号 
 * @note EBYTE模块RXEN为高电平生效
 */
void Ebyte_Port_RxenIoControl( uint8e_t cmd )
{
    if ( cmd == 1 )
    {
        /* !必须提供: 目标硬件电路对应RXEN控制IO 输出高电平 */
        Ebyte_BSP_RfRxIoEnable();
    }
    else
    {
        /* !必须提供: 目标硬件电路对应RXEN控制IO 输出低电平 */
        Ebyte_BSP_RfRxIoDisable();
    }
}

/* !
 * @brief 配置目标硬件平台接收使能引脚 BUSY 控制函数 
 * 
 * @note EBYTE模块BUSY为高电平表示忙
 */
uint8e_t Ebyte_Port_BusyIoRead( void )
{
    uint8e_t result = 0 ;  
    
    /* !必须提供: 目标硬件电路对应BUSY IO 状态读取 */
    result = Ebyte_BSP_RfBusyIoRead();
    return result;
}

/* !
 * @brief 配置目标硬件平台延时函数
 * 
 * @param time 期望延时毫秒 
 * @note 请注意时序，模块初始化时将调用此函数，请注意中断是否会影响到此函数
 */
void Ebyte_Port_DelayMs( uint16e_t time )
{
    /* !必须提供: 延时函数 */
  
    uint16e_t i,n;
    
    while(time--)//这里示范了STM8L 16M时钟前提下的粗略软件延时 
    {
        for(i=900;i>0;i--)
        {
              for( n=1 ;n>0 ;n--)
              {
                  asm("nop"); 
                  asm("nop");
                  asm("nop");
                  asm("nop");
              }
        }
    }    
}



