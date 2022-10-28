#include "ebyte_e49x.h"

/* 辅助识别驱动程序版本号 */
#define EBYTE_E49_PROGRAM_TYPE 0x10 

/// 辅助识别模块
#if defined(EBYTE_E49_400M20S)
static uint8e_t E49x_NameString[] = "E49-400M20S";
#elif defined(EBYTE_E49_900M20S)
static uint8e_t E49x_NameString[] = "E49-900M20S";
#endif 

#define  GPIO1_DOUT          0x00
#define  GPIO1_DIN           0x00
#define  GPIO1_IRQ1          0x01
#define  GPIO1_IRQ2          0x02
#define  GPIO1_DCLK          0x03

#define  GPIO2_IRQ1          0x00
#define  GPIO2_IRQ2          0x04
#define  GPIO2_DOUT          0x08
#define  GPIO2_DIN           0x08
#define  GPIO2_DCLK          0x0C

#define  GPIO3_CLKO          0x00
#define  GPIO3_DOUT          0x10
#define  GPIO3_DIN           0x10
#define  GPIO3_IRQ2          0x20
#define  GPIO3_DCLK          0x30

#define  FIFO_READ           0x00
#define  FIFO_WRITE          0x01   

#define  IRQ_RX_ACTIVE       0x00
#define  IRQ_TX_ACTIVE       0x01
#define  IRQ_RSSI_VLD        0x02
#define  IRQ_PREAM_OK        0x03
#define  IRQ_SYNC_OK         0x04
#define  IRQ_NODE_OK         0x05
#define  IRQ_CRC_OK          0x06
#define  IRQ_PKT_OK          0x07
#define  IRQ_SL_TMO          0x08
#define  IRQ_RX_TMO          0x09
#define  IRQ_TX_DONE         0x0A
#define  IRQ_RX_FIFO_NMTY    0x0B
#define  IRQ_RX_FIFO_TH      0x0C
#define  IRQ_RX_FIFO_FULL    0x0D
#define  IRQ_RX_FIFO_WBYTE   0x0E
#define  IRQ_RX_FIFO_OVF     0x0F
#define  IRQ_TX_FIFO_NMTY    0x10
#define  IRQ_TX_FIFO_TH      0x11
#define  IRQ_TX_FIFO_FULL    0x12
#define  IRQ_STATE_IS_STBY   0x13
#define  IRQ_STATE_IS_FS     0x14
#define  IRQ_STATE_IS_RX     0x15
#define  IRQ_STATE_IS_TX     0x16
#define  IRQ_LED             0x17
#define  IRQ_TRX_ACTIVE      0x18
#define  IRQ_PKT_DONE        0x19

#define  SL_TMO_IRQ_EN          0x80
#define  RX_TMO_IRQ_EN          0x40
#define  TX_DONE_IRQ_EN         0x20
#define  PREAM_OK_IRQ_EN        0x10
#define  SYNC_OK_IRQ_EN         0x08
#define  NODE_OK_IRQ_EN         0x04
#define  CRC_OK_IRQ_EN          0x02
#define  PKT_DONE_IRQ_EN        0x01

#define GET_IRQ_SL_TMO   0x2000
#define GET_IRQ_RX_TMO   0x1000
#define GET_IRQ_TX_DONE  0x0800
#define GET_IRQ_LBD      0x0080
#define GET_IRQ_COL_ERR  0x0040
#define GET_IRQ_PKT_ERR  0x0020
#define GET_IRQ_PREAM_OK 0x0010
#define GET_IRQ_SYNC_OK  0x0008
#define GET_IRQ_NODE_OK  0x0004
#define GET_IRQ_CRC_OK   0x0002
#define GET_IRQ_PKT_OK   0x0001

typedef enum 
{
    GO_INIT          =0x00,        
    GO_BUSY,          
    GO_STBY,          
    GO_RECEIVE,       
    GO_WAIT_RECEIVE,          
    GO_TRANSMIT,      
    GO_WAIT_TRANSMIT, 
    GO_SLEEP,         
    GO_ERROR         
}E49x_Status_t;

static E49x_Status_t E49x_Status = GO_INIT;
static uint8e_t E49x_ReceiveBuffer[64] = {0};
static uint8e_t E49x_IRQ_Trigger = 0;
/* * 
 * 基准参数 
 * 起始频率:850MHz
 * Data Rate:2.4kbps
 * Deviation:4.8kHz
 * Bandwidth:Auto
 */
uint8e_t E49x_Config_CMT[12] = {
    0x00,0x66,0xEC,0x1D,0xF0,0x80,0x14,0x08,0x11,0x02,0x02,0x00
};
uint8e_t E49x_Config_System[12] = {
    0xAE,0xE0,0x35,0x00,0x00,0xF4,0x10,0xE2,0x42,0x20,0x00,0x81    
};
uint8e_t E49x_Config_Frequency[8] = {
    0x41,0x6D,0x80,0x86,0x41,0x62,0x27,0x16  
};
uint8e_t E49x_Config_DataRate[24] = {
    0x32,0x18,0x10,0x99,0xC1,0x9B,0x06,0x0A,0x9F,0x39,0x29,0x29,0xC0,0x51,0x2A,0x53,0x00,0x00,0xB4,0x00,0x00,0x01,0x00,0x00   
};
uint8e_t E49x_Config_Baseband[29] = {
    0x12,0x08,0x00,0xAA,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0xD4,0x2D,0x01,0x1F,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x60,0xFF,0x00,0x00,0x1F,0x10   
};
uint8e_t E49x_Config_TX[11] = {
    0x50,0x83,0x01,0x00,0x42,0xB0,0x00,0x8A,0x18,0x3F,0x7F
};

/* !
 * @brief IO模拟半双工SPI时钟周期  辅助延时
 * 
 * @param time 节拍
 */
static void E49x_SpiDelay(uint8e_t time)
{
    uint8e_t n = time;
    while(n--);
}

/* !
 * @brief IO模拟半双工SPI 1Byte=8Bit 逐位输出 
 * 
 * @param data 输出数据
 * @note 时序请参考文档 AN142第2节 SPI写寄存器时序
 */
static void E49x_HalfSpiTransmit( uint8e_t data )
{
    uint8e_t i;
  
    /* 数据 1Byte=8bit逐位输出 */
    for( i=0 ; i<8 ; i++)
    {
        /* SLCK=0 即为CPOL=0 空闲时总线为低 */
        Ebyte_Port_SlckIoControl(0);
        
        /* 数据位与0x80 即为MSB方式输出 */
        if( data & 0x80 )
        {
            Ebyte_Port_SdioIoControl(1);
        }
        else
        {
            Ebyte_Port_SdioIoControl(0);
        }
        
        E49x_SpiDelay(7);
        
        /* 数据位调整 */
        data <<=1;
        
        /* SLCK= 1锁定数据 即为CPOA=0 第一个跳变进行采样 */
        Ebyte_Port_SlckIoControl(1);
        
        E49x_SpiDelay(7);
    }  
}

/* !
 * @brief IO模拟半双工SPI 逐位读取 8Bit数据位 
 * 
 * @return 读取的数据 1Byte
 * @note 时序请参考文档 AN142第2节 SPI读寄存器时序
 */
static uint8e_t E49x_HalfSpiReceive( void )
{
    uint8e_t i;  
    uint8e_t data = 0xFF;
    
    for( i=0 ; i<8; i++ )
    {
        /* SLCK=0 即为CPOL=0 空闲时总线为低 */
        Ebyte_Port_SlckIoControl(0);
        
        E49x_SpiDelay(7);
        
        data <<= 1;
        
        /* SLCK=1 触发从机输出Bit */
        Ebyte_Port_SlckIoControl(1);
        
        /* 读取SDIO 记录Bit */
        if( Ebyte_Port_SdioIoControl(4))
        {
            data |= 0x01;
        }
        else
        {
            data &= ~0x01;
        }
        
        E49x_SpiDelay(7);
    }
    
    return data;
}

/* !
 * @brief 通过地址向内部寄存器写入数据
 * 
 * @param address 寄存器地址 
 * @param data 数据 
 * @note  2线半双工SPI+2线CS片选通信接口，不是标准4线全双工SPI接口
 */
static void E49x_SetRegister( uint8e_t address, uint8e_t data )
{

    /* SDIO 引脚*/
    Ebyte_Port_SdioIoControl(3);//设置SDIO为输出模式
    Ebyte_Port_SdioIoControl(1);//设置SDIO输出高电平  
    
    /* SLCK 引脚 */
    Ebyte_Port_SlckIoControl(0);
    
    /* CS   引脚 */
    Ebyte_Port_FcsbIoControl(1);
    Ebyte_Port_CsbIoControl(0); //CS二选一 CSB输出低电平表示选择寄存器
    
    /* 至少等待半个 SLCK周期 */
    E49x_SpiDelay(20);
    
    /* 写地址 */
    E49x_HalfSpiTransmit( address & 0x7F );//地址特殊处理 与0x7F 让r/w位=0
    
    /* 写数据 */
    E49x_HalfSpiTransmit( data ); 
    
    /* 释放总线 */
    Ebyte_Port_SlckIoControl(0);
    E49x_SpiDelay(20);
    Ebyte_Port_CsbIoControl(1);
    
    /* SDIO */
    Ebyte_Port_SdioIoControl(2); //默认切换为输入 
}

/* !
 * @brief 通过地址向内部寄存器读取数据
 * 
 * @param address 寄存器地址 
 * @return data 数据 1Byte 
 * @note  2线半双工SPI+2线CS片选通信接口，不是标准4线全双工SPI接口
 *        时序请参考文档 AN142第2节
 */
static uint8e_t E49x_GetRegister( uint8e_t address )
{
    uint8e_t result = 0xFF;

    /* SDIO 引脚 */
    Ebyte_Port_SdioIoControl(3);//设置SDIO为输出模式
    Ebyte_Port_SdioIoControl(1);//设置SDIO输出高电平  

    /* SLCK 引脚 */
    Ebyte_Port_SlckIoControl(0);
    
    /* CS   引脚 */
    Ebyte_Port_FcsbIoControl(1);
    Ebyte_Port_CsbIoControl(0); //CS二选一 CSB输出低电平表示选择寄存器
    
    /* 至少等待半个 SLCK周期 */
    E49x_SpiDelay(20);
    
    /* 写地址 */
    E49x_HalfSpiTransmit( address|0x80 ); //地址特殊处理 或0x80 让r/w位=1
    
    /* SDIO 切换为输入  */
    Ebyte_Port_SdioIoControl(2);
    
    /* 读数据 */
    result = E49x_HalfSpiReceive();
    
    /* 释放总线 */
    Ebyte_Port_SlckIoControl(0);   
    E49x_SpiDelay(20);   
    Ebyte_Port_CsbIoControl(1);
    
    return result;
}

/* !
 * @brief 写内部数据缓存区 FIFO
 * 
 * @param data 指向待写入数据首地址的指针
 * @param size 写入数据长度
 * @note 一般支持32字节。如果开启了合并发送和输入的独立FIFO，则支持64字节
 */
static void E49x_SetFIFO( uint8e_t *data , uint16e_t size )
{
    uint8e_t i;
  
    /* CS 引脚复位 */
    Ebyte_Port_FcsbIoControl(1);
    Ebyte_Port_CsbIoControl(1);
    
    /* SLCK 引脚 */
    Ebyte_Port_SlckIoControl(0);
    
    /* SDIO 切换为输出  */
    Ebyte_Port_SdioIoControl(3);
    
    /* 循环写入  */
    for( i=0 ; i<size ; i++)
    {
        /* CS二选一 FCSB输出低电平表示选择内部FIFO */
        Ebyte_Port_FcsbIoControl(0); 
        
        E49x_SpiDelay(20);
        
        /* 写数据 */
        E49x_HalfSpiTransmit( data[i] );
        
        Ebyte_Port_SlckIoControl(0);
        
        E49x_SpiDelay(30);
        
        Ebyte_Port_FcsbIoControl(1);
        
        E49x_SpiDelay(60);
    }
    
    Ebyte_Port_SlckIoControl(0);
}

/* !
 * @brief 读内部FIFO
 * 
 * @param data 指向存放数据首地址的指针
 * @param size 读取的数据长度
 * @return 0:正常 1:FIFO有数据但长度表示位不正确
 * @note 一般FIFO最大为32字节(有1字节被长度表示位占用)。如果开启了合并发送和输入的独立FIFO，则支持64字节
 */
static uint8e_t E49x_GetFIFO( uint8e_t *data , uint8e_t *size )
{
    uint8e_t i,length;
    
    /* CS 引脚复位 */
    Ebyte_Port_FcsbIoControl(1);
    Ebyte_Port_CsbIoControl(1);
    
    /* SLCK 引脚 */
    Ebyte_Port_SlckIoControl(0);
  
    /* SDIO 切换为输入  */
    Ebyte_Port_SdioIoControl(2);
    
    /* 先读取第一字节长度表示位 
       因为启用了可变长数据体，该位由E49发送方硬件自动生成  
    */
    Ebyte_Port_FcsbIoControl(0);   
    E49x_SpiDelay(20);    
    length = E49x_HalfSpiReceive();    
    Ebyte_Port_SlckIoControl(0);    
    E49x_SpiDelay(30);    
    Ebyte_Port_FcsbIoControl(1);    
    E49x_SpiDelay(60);    
    
    /* 判断长度表示是否正确 */
    if( length > 64 ) return 1;
    length -= 1;//去掉1长度表示位 才是数据长度
    
    /* 循环读取  */
    for( i=0 ; i<length ; i++)
    {
        /* CS二选一 FCSB输出低电平表示选择内部FIFO */
        Ebyte_Port_FcsbIoControl(0);
        
        E49x_SpiDelay(20);
        
        /* 读数据 */
        data[i] = E49x_HalfSpiReceive();
        
        Ebyte_Port_SlckIoControl(0);
        
        E49x_SpiDelay(30);
        
        Ebyte_Port_FcsbIoControl(1);
        
        E49x_SpiDelay(60);
    } 
    
    /* 填充长度 */
    *size = length;
    
    return 0;
}

/* !
 * @brief 清除FIFO内容
 * @param mode 模式
 *           @arg FIFO_WRITE ：写模式
 *           @arg FIFO_READ  ：读模式
 *
 */
static void E49x_ClearFIFO( uint8e_t mode)
{
    if( mode == FIFO_WRITE )
    {    
        /* 清除发送FIFO 寄存器地址:0x6C bit0:0x01  */
        E49x_SetRegister(0x6C, 0x01);//bit0  0：无效，1：清零 TX FIFO 将这个位设成 1 之后，无需将它再设回 0， 内部会自动设回为0
    }
    else
    {
        /* 清除接收FIFO 寄存器地址:0x6C bit1:0x02 */
        E49x_SetRegister(0x6C, 0x02);//bit1  0：无效，1：清零 RX FIFO 将这个位设成 1 之后，无需将它再设回 0， 内部会自动设回为0。     
    } 
}

/* !
 * @brief 控制FIFO进入读或写模式 
 * @param mode 模式
 *           @arg FIFO_WRITE ：写模式
 *           @arg FIFO_READ  ：读模式
 *
 */
static void E49x_GoFIFO( uint8e_t mode)
{
    uint8e_t config;
    
    /* 寄存器地址:0x69  bit2:0x04  bit0:0x01 */
    config = E49x_GetRegister(0x69);
    
    if( mode == FIFO_WRITE )
    {    
        config |= 0x04;//bit2 当作为一个 64-byte FIFO 时可使用。0：用作 RX FIFO，1：用作 TX FIFO。
        config |= 0x01;//bit0 必须在访问 FIFO 前设置好。0：SPI 的操作是读 FIFO，1：SPI 的操作是写 FIFO。
    }
    else
    {
        config &= ~0x04;//bit2 
        config &= ~0x01;//bit0
    }
    
    E49x_SetRegister(0x69, config);  
}

/* !
 * @brief 复位 
 * @note 模块内部已配置的参数会丢失
 */
void E49x_Reset(void)
{
    /* 软件指令复位 */
    E49x_SetRegister(0x7F, 0xFF);
}

/* !
 * @brief 写寄存器 配置为待机模式 
 *
 * @return 0:正常 1:异常
 * @note 待机模式下才能配置寄存器
 */
static uint8e_t E49x_SetStby(void)
{
    uint8e_t retry  =10;
    uint8e_t result =1 ;
    
    
    /* 状态机:准备状态切换 */
    E49x_Status = GO_BUSY;
    
    /* 写指令进入待机模式 寄存器地址:0x60  bit1:0x02 */
    E49x_SetRegister(0x60,0x02);
    

    while( retry > 0)
    {
        Ebyte_Port_DelayUs(100);
        
        /* 读取寄存器进行确认 寄存器地址:0x61 bit3至bit0=0x02 */
        result = E49x_GetRegister(0x61) & 0x0F;
        if( result == 0x02 )
        {
            /* 状态变更记录 */
            E49x_Status = GO_STBY;
            
            /* 正常返回 */
            result = 0;
            break;
        }    
        
        Ebyte_Port_DelayUs(500);
        retry--;
    }
    
    /* 结果检查 */
    if( result!=0 ) 
    {
        /* 状态机:模块异常 */
        E49x_Status = GO_ERROR;        
    }
    
    return result;
}



/* !
 * @brief 写寄存器 配置为接收模式 
 *
 * @return 0:正常 1:异常
 */
static uint8e_t E49x_SetReceive(void)
{
    uint8e_t retry  =10;
    uint8e_t result =1 ;
    
    /* 状态机:准备状态切换 */
    E49x_Status = GO_BUSY;    
    
    /* 写指令进入接收模式 寄存器地址:0x60  bit3:0x08 */
    E49x_SetRegister(0x60,0x08);
    

    while( retry > 0)
    {
        Ebyte_Port_DelayUs(100);
        
        /* 读取寄存器进行确认 寄存器地址:0x61 bit3至bit0=0x05 */
        result = E49x_GetRegister(0x61) & 0x0F;
        if( result == 0x05 )
        {
            /* 状态变更记录 */
            E49x_Status = GO_RECEIVE;
            
            /* 正常返回 */
            result = 0;
            break;
        }    
        
        Ebyte_Port_DelayUs(100);
        
        /* 读取寄存器进行确认 寄存器地址:0x6D 低1 bit为有效位*/
        result = E49x_GetRegister(0x6D) & 0x01;
        if( result == 0x01 )
        {
            /* 状态变更记录 */
            E49x_Status = GO_RECEIVE;
            
            /* 正常返回 */
            result = 0;
            break;
        }
        
        Ebyte_Port_DelayUs(500);
        retry--;
    }
    

    /* 结果检查 */
    if( result!=0 ) 
    {
        /* 状态机:模块异常 */
        E49x_Status = GO_ERROR;        
    }
    
    return result;
}

/* !
 * @brief 写寄存器 配置为发送模式 
 *
 * @return 0:正常 1:异常
 * @note 将会自动的对FIFO中的数据进行无线传输
 */
static uint8e_t E49x_SetTransmit(void)
{
    uint8e_t retry  =10;
    uint8e_t result =1 ;
    
    /* 状态机:准备状态切换 */
    E49x_Status = GO_BUSY;
    
    /* 写指令进入发送模式 寄存器地址:0x60  bit6:0x40 */
    E49x_SetRegister(0x60,0x40);
    
    while( retry > 0)
    {
        Ebyte_Port_DelayUs(100);
        
        /* 读取寄存器进行确认 寄存器地址:0x61 bit3至bit0=0x06*/
        result = E49x_GetRegister(0x61) & 0x0F;
        if( result == 0x06 )
        {
            /* 状态变更记录 */
            E49x_Status = GO_TRANSMIT;
            
            /* 正常返回 */
            result = 0;
            break;
        }    
        
        Ebyte_Port_DelayUs(100);
        
        /* 读取寄存器进行确认 寄存器地址:0x6A  bit3=0x08 */
        result = E49x_GetRegister(0x6A) & 0x08;
        if( result == 0x08 )
        {
            /* 状态变更记录 */
            E49x_Status = GO_TRANSMIT;
            
            /* 正常返回 */
            result = 0;
            break;
        }  

        Ebyte_Port_DelayUs(500); 
        retry--;
    }
    
    /* 结果检查 */
    if( result!=0 ) 
    {
        /* 状态机:模块异常 */
        E49x_Status = GO_ERROR;        
    }
    
    return result;
}

/* !
 * @brief 写寄存器 配置为休眠模式 
 *
 * @return 0:正常 1:异常
 */
static uint8e_t E49x_SetSleep(void)
{
    uint8e_t retry  =10;
    uint8e_t result =1 ;
    
    /* 状态机:准备状态切换 */
    E49x_Status = GO_BUSY;
    
    /* 写指令进入休眠模式 寄存器地址:0x60  bit4:0x10 */
    E49x_SetRegister(0x60,0x10);
    
    while( retry > 0)
    {
        Ebyte_Port_DelayUs(100);
        
        /* 读取寄存器进行确认 寄存器地址:0x61  bi3至bit1=0x01*/
        result = E49x_GetRegister(0x61) & 0x0F;
        if( result == 0x01 )
        {
            /* 状态变更记录 */
            E49x_Status = GO_SLEEP;
            
            /* 正常返回 */
            result = 0;
            break;
        }    
        
        Ebyte_Port_DelayUs(500);
        retry--;
    }
    
    /* 结果检查 */
    if( result!=0 ) 
    {
        /* 状态机:模块异常 */
        E49x_Status = GO_ERROR;        
    }
    
    return result;
}


/* !
 * @brief 配置实际发送消息体的长度
 * 
 * @param size 计算后的长度
 *
 * @note 如果改动了NODEID 请参考AN143手册填入正确的消息长度
 *        例如: 假设寄存器0x47 bit3:2 中NODEID_SIZE为1 即NODEID长度为2，
 *              那么应填入的消息体长度为 表示位1+NODEID长度2+用户数据长度 = 用户数据长度+3
 */
static void E49x_SetPayloadSize( uint16e_t size )
{
    uint8e_t config; 
      
    /* 寄存器地址:0x45 bit6至bit4:0x70  对应数据体长度10:8bit*/  
    config = E49x_GetRegister(0x45);

    config &= ~0x70;
    config |= (size >> 4) & 0x70;
    E49x_SetRegister(0x45, config);
    
    /* 寄存器地址:0x46 bit7至bit0:0xFF 对应数据体长度7:0bit*/
    config = size & 0xFF;
    E49x_SetRegister(0x46, config);      
}

/* !
 * @brief 配置通用IO的使用方式
 * 
 * @param config 见宏定义  GPIO1_xxx | GPIO2_xxx | GPIO3_xxx
 *
 * @note 一般都是使用IO作为中断源 通知MCU发送/接收完成
 */
static void E49x_SetGPIO( uint8e_t config )
{
    /* 寄存器地址:0x65 */
    E49x_SetRegister(0x65, config);
}

/* !
 * @brief 配置中断类型
 * 
 * @param configIrq1 配置中断1的类型 见宏定义 
 * @param configIrq2 配置中断2的类型 见宏定义 
 *
 * @note 一般都是配置发送/接收完成中断，配合E49x_SetGPIO函数映射到IO上
 */
static void E49x_SetIRQ(uint8e_t configIrq1, uint8e_t configIrq2)
{
    
    /* 中断1映射 占用bit4至bit0即0x1F 寄存器地址:0x66 */
    configIrq1 &= 0x1F;
    configIrq1 |= (~0x1F) & E49x_GetRegister(0x66);
    E49x_SetRegister(0x66, configIrq1);

    /* 中断2映射 占用bit4至bit0即0x1F 寄存器地址:0x67 */
    configIrq2 &= 0x1F;
    configIrq2 |= (~0x1F) & E49x_GetRegister(0x67);
    E49x_SetRegister(0x67, configIrq2);
}



/* !
 * @brief 获取中断标志
 *
 * @return  中断置位相关bit
 *  
 *           0x2000:SL_TMO 睡眠超时中断标志
 *           0x1000:RX_TMO 接收超时中断标志
 *           0x0800:TX_DONE 发射完成中断标志   
 *           0x0080:LBD 有效（成功检测到低电压）中断标志
 *           0x0040:COL_ERR 中断标志
 *           0x0020:PKT_ERR 中断标志
 *           0x0010:PREAM_OK 前导码检测成功中断标志
 *           0x0008:SYNC_OK 同步字检测成功中断标志
 *           0x0004:NODE_OK 节点ID检测成功中断标志
 *           0x0002:CRC_OK  CRC检测成功中断标志
 *           0x0001:PKT_OK  数据包接收完成（不管对错）中断使能中断标志
 *
 */
static uint16e_t E49x_GetIRQ(void)
{
    uint8e_t irqPolar,irqSetFlag6D,irqSetFlag6A;
    
    /* 获取中断寄存器数据 寄存器地址:0x6D + 0x6A */
    irqSetFlag6D = E49x_GetRegister(0x6D);
    irqSetFlag6A = E49x_GetRegister(0x6A);
    
    /* 获取中断极性(0:表示高电平是有效中断 1:表示低电平是有效中断) bit1:0x20 寄存器地址:0x66 */
    irqPolar = (E49x_GetRegister(0x66) & 0x20)? 1:0 ;
    
    /* 低电平为有效中断时 需要位反处理  */
    if( irqPolar )
    {
        irqSetFlag6D = ~irqSetFlag6D;
        irqSetFlag6A = ~irqSetFlag6A;
    }    
    
    return  (irqSetFlag6A<<8) | irqSetFlag6D;
}

/* !
 * @brief 清除所有中断标志
 *
 * @note  中断置位相关bit
 *           寄存器地址 0x6D
 *           bit7:LBD 有效（成功检测到低电压）中断标志
 *           bit6:COL_ERR 中断标志
 *           bit5:PKT_ERR 中断标志
 *           bit4:PREAM_OK 中断标志
 *           bit3:SYNC_OK 中断标志
 *           bit2:NODE_OK 中断标志
 *           bit1:CRC_OK 中断标志
 *           bit0:PKT_OK 中断标志
 *
 *           寄存器地址 0x6A
 *           bit5:SL_TMO 中断标志
 *           bit4:RX_TMO 中断标志
 *           bit3:TX_DONE 中断标志
 *
 * @note  中断清除相关bit
 *           寄存器地址 0x6A
 *           bit2:TX_DONE 中断清零
 *           bit1:SL_TMO 中断清零
 *           bit0:RX_TMO 中断清零
 *
 *           寄存器地址 0x6B
 *           bit5:LBD 有效（成功检测到低电压）中断清零
 *           bit4:PREAM_OK 中断清零
 *           bit3:SYNC_OK 中断清零
 *           bit2:NODE_OK 中断清零
 *           bit1:CRC_OK 中断清零
 *           bit0:PKT_DONE 中断清零
 */
void E49x_ClearIRQ(void)
{
    uint8e_t irqPolar,irqSetFlag6D,irqSetFlag6A;
    uint8e_t irqClearFlag6A = 0;
    uint8e_t irqClearFlag6B = 0;
    
    /* 获取中断寄存器数据 寄存器地址:0x6D + 0x6A */
    irqSetFlag6D = E49x_GetRegister(0x6D);
    irqSetFlag6A = E49x_GetRegister(0x6A);
    
    /* 获取中断极性(0:表示高电平是有效中断 1:表示低电平是有效中断) bit1:0x20 寄存器地址:0x66 */
    irqPolar = (E49x_GetRegister(0x66) & 0x20)? 1:0 ;
    
    /* 低电平为有效中断时 需要位反处理  */
    if( irqPolar )
    {
        irqSetFlag6D = ~irqSetFlag6D;
        irqSetFlag6A = ~irqSetFlag6A;
    }
    
    /* 中断置位标识位与清除标识位不一致 需要逐位标记 */
    if( irqSetFlag6D & 0x80 ) 
    {
        irqClearFlag6B |= 0x20;  //寄存器0x6D-LBD置位bit7 对应 寄存器0x6B-LBD清零bit5
    }  
    if( irqSetFlag6D & 0x40 ) 
    {
        irqClearFlag6B |= 0x01;  //寄存器COL_ERR置位bit6  对应 寄存器0x6B-PKT_DONE清零bit0
    }        
    if( irqSetFlag6D & 0x20 ) 
    {
        irqClearFlag6B |= 0x01;  //寄存器0x6D-PKT_ERR置位bit5 对应 寄存器0x6B-PKT_DONE清零bit0
    }    
    if( irqSetFlag6D & 0x10 ) 
    {
        irqClearFlag6B |= 0x10;  //寄存器0x6D-PREAM_OK置位bit4 对应 寄存器0x6B-PREAM_OK清零bit4
    }  
    if( irqSetFlag6D & 0x08 ) 
    {
        irqClearFlag6B |= 0x08;  //寄存器0x6D-SYNC_OK置位bit3 对应 寄存器0x6B-SYNC_OK清零bit3
    }  
    if( irqSetFlag6D & 0x04 ) 
    {
        irqClearFlag6B |= 0x04;  //寄存器0x6D-NODE_OK置位bit2 对应 寄存器0x6B-NODE_OK清零bit2
    }  
    if( irqSetFlag6D & 0x02 ) 
    {
        irqClearFlag6B |= 0x02;  //寄存器0x6D-CRC_OK置位bit1 对应 寄存器0x6B-CRC_OK清零bit1
    }  
    if( irqSetFlag6D & 0x01 ) 
    {
        irqClearFlag6B |= 0x01;  //寄存器0x6D-PKT_OK置位bit0 对应 寄存器0x6B-PKT_DONE清零bit0
    }      
    
    if( irqSetFlag6A & 0x20 ) 
    {
        irqClearFlag6A |= 0x02;  //寄存器0x6A-SL_TMO置位bit5 对应 寄存器0x6A-SL_TMO清零bit1
    }  
    if( irqSetFlag6A & 0x10 ) 
    {
        irqClearFlag6A |= 0x01;  //寄存器0x6A-RX_TMO置位bit4 对应 寄存器0x6A-PKT_DONE清零bit0
    }  
    if( irqSetFlag6A & 0x08 ) 
    {
        irqClearFlag6A |= 0x04;  //寄存器0x6A-TX_DONE置位bit3 对应 寄存器0x6A-TX_DONE清零bit2
    }      
    
    /* 将清除标识写回寄存器 */
    E49x_SetRegister(0x6A, irqClearFlag6A);
    E49x_SetRegister(0x6B, irqClearFlag6B);
}

/* !
 * @brief 开启中断
 * 
 * @param enableIrq 允许中断的bit位
 * @note 一般都是配置映射对应的中断位
 */
static void E49x_GoIRQ( uint8e_t  enableIrq)
{
    /* 开启中断 寄存器地址:0x68 */
    E49x_SetRegister(0x68, enableIrq);
}

/* !
 * @brief 开始监听数据
 * 
 * @note 一旦接收到数据 会触发GPIO中断
 */
uint8e_t E49x_GoReceive(void)
{
    uint8e_t result = 0;
    uint8e_t irqGpio = GPIO1_IRQ2;
    
    /* 进入待机配置模式 */
    result = E49x_SetStby();
    if( result!=0  ) return 1;
    
    /* 中断通道IRQ2 */
    switch( RECEIVE_IRQ_GPIO )
    {
        case 1:irqGpio=GPIO1_IRQ2;break;
        case 2:irqGpio=GPIO2_IRQ2;break;
        case 3:irqGpio=GPIO3_IRQ2;break;
        default: break;
    }
    
    /* 配置GPIO中断映射 */                  
    E49x_SetGPIO(  irqGpio  );    
    
    /* 配置中断类型映射  这里将接收完成中断映射到IRQ2  IRQ_TX_DONE占位 */
    E49x_SetIRQ (  IRQ_TX_DONE,  IRQ_PKT_OK );   
   
    /* 配置参数生效 */
    result = E49x_SetSleep();    
    if( result!=0  ) return 1;   
       
    /* 进入待机配置模式 */
    result = E49x_SetStby();
    if( result!=0  ) return 1;
    
    /* 清除所有中断标识 */
    E49x_ClearIRQ();
      
    /* 允许中断(内部寄存器标识将置位) */
    E49x_GoIRQ( PREAM_OK_IRQ_EN | SYNC_OK_IRQ_EN | PKT_DONE_IRQ_EN );
    
    /* 允许读取内部FIFO */
    E49x_GoFIFO( FIFO_READ );
    
    /* 清除接收FIFO */
    E49x_ClearFIFO( FIFO_READ );
    
    /* 进入接收模式 */
    result = E49x_SetReceive();
    if( result!=0  ) return 1;
    
    /* 状态机:等待接收完成 */
    E49x_Status = GO_WAIT_RECEIVE;
     
    
    /* 正常返回 0 */
    return result;
  
}

/* !
 * @brief 开始发送数据
 * 
 * @param data 指向待发送数据的指针
 * @param size 待发送长度
 * @return 0:正常 1:发送失败
 * @note 注意size 如果合并了发送接收FIFO 则支持64字节，否则为32字节
 *       如果FIFO 是空的，进入发送状态后，那么就会一直发射设定prefix内容(例如前导码)，直到FIFO被填入数据
 *       建议每次发送完后手动切换为接收模式
 */
uint8e_t E49x_GoTransmit( uint8e_t *data, uint8e_t size )
{
    uint8e_t result = 0;
    uint8e_t irqGpio = GPIO1_IRQ2;
    
    /* 进入待机配置模式 */
    result = E49x_SetStby();
    if( result!=0  ) return 1;
    
    /* 中断通道IRQ2 */
    switch( TRANSMIT_IRQ_GPIO )
    {
        case 1:irqGpio=GPIO1_IRQ2;break;
        case 2:irqGpio=GPIO2_IRQ2;break;
        case 3:irqGpio=GPIO3_IRQ2;break;
        default: break;
    }    
    
    /* 配置GPIO中断映射到IRQ2 */                  
    E49x_SetGPIO(  irqGpio );    
    
    /* 配置中断类型映射  这里将发送完成中断映射到IRQ2 IRQ_PKT_OK占位 */
    E49x_SetIRQ (  IRQ_PKT_OK , IRQ_TX_DONE  );   
    
    /* 配置参数生效 */
    result = E49x_SetSleep();    
    if( result!=0  ) return 1;   
       
    /* 进入待机配置模式 */
    result = E49x_SetStby();
    if( result!=0  ) return 1;
    
    /* 清除所有中断标识 */
    E49x_ClearIRQ();
      
    /* 允许中断 */
    E49x_GoIRQ( TX_DONE_IRQ_EN  );
    
    /* 允许写入内部FIFO */
    E49x_GoFIFO( FIFO_WRITE );
    
    /* 清除发送FIFO */
    E49x_ClearFIFO( FIFO_WRITE );
    
    /* 待发送数据写入FIFO */
    E49x_SetFIFO( data , size);
    
    /* 默认为 可变长数据体+无NODEID=数据长度size+长度表示位1  参考文档AN143 */
    E49x_SetPayloadSize( size+1 );
      
    /* 进入发送模式 自动传输FIFO内容*/
    result = E49x_SetTransmit();
    if( result!=0  ) return 1;
    
    /* 状态机:等待发送完成 */
    E49x_Status = GO_WAIT_TRANSMIT;
    
    /* 如果开启了阻塞模式 就在此检查到发送完成才返回*/    
#if EBYTE_TRANSMIT_MODE_BLOCKED
    uint16e_t irqSet = 0;
    do
    {
        irqSet = E49x_GetIRQ();
        Ebyte_Port_DelayUs(500);
    }
    while( !(irqSet & GET_IRQ_TX_DONE ) );
    
    E49x_ClearIRQ();
    
    /* 状态机:发送完成 进入待机状态 */
    E49x_SetStby();
    
    /* 发送完成 回调用户接口函数 */
    Ebyte_Port_TransmitCallback( irqSet );
#endif    
    /* 正常返回 0 */
    return result;    
}

/* !
 * @brief 检查模块是否可以正常读写
 *
 * @return 0:正常 1:读写不正常,请检查硬件
 */
static uint8e_t E49x_IsAlive(void)
{
    uint8e_t back, getData ;
    uint8e_t testRegister = 0x48;
    uint8e_t testData = 0xAA;
    uint8e_t result = 1;
      
    /* 尝试向寄存器0x48写入数据 test */
    back = E49x_GetRegister(testRegister);
    E49x_SetRegister(testRegister, testData);

    /* 读取确认 恢复原数据 */
    getData = E49x_GetRegister(testRegister);
    E49x_SetRegister(testRegister, back);  
    
    if( getData == testData )
    {
        /* 正常 */
        result = 0;
    }

    return result;
}

/* !
 * @brief 基础调制参数配置 
 */
static void E49x_Config(void)
{
    uint8e_t config = 0;  
    uint8e_t i,start;
    
    /* 开启配置保持 bit4:0x10, 关闭复位引脚 bit5:0x20 寄存器地址:0x61 */
    config  = E49x_GetRegister(0x61);
    config |= 0x10;       
    config &= ~0x20;      
    E49x_SetRegister(0x61, config);

    /* 开启锁相环频率锁定 (不开有一定概率频率不正确，导致无法接收数据) bit5:0x20 寄存器地址:0x62 */
    config  = E49x_GetRegister(0x62);
    config |= 0x20;        
    E49x_SetRegister(0x62, config);
    
    /* 关闭LFOSC矫正模块 （LFOSC主要用于驱动 Sleep Timer，关闭后降低了休眠功耗以及节约5ms的矫正时间）bit7至bit5=0 寄存器地址:0x0D  */
    config  = E49x_GetRegister(0x0D);
    config &= ~0xE0;
    
    /* 合并发送/接收各自独立的32字节FIFO 构成64字节FIFO bit1:0x02  寄存器地址:0x69 */
    config  = E49x_GetRegister(0x69);
    config |= 0x02; 
    E49x_SetRegister(0x69, config);
    
    /* 配置软件生成的参数 写入到寄存器 */
    /* CMT区  起始地址:0x00 长度:12 */
    start = 0;
    for( i=0 ; i<12; i++ )
    {
        E49x_SetRegister( start+i, E49x_Config_CMT[i] );
    }
    /* 系统区   起始地址:0x0C 长度:12 */
    start = 0x0C;
    for( i=0 ; i<12; i++ )
    {
        E49x_SetRegister( start+i, E49x_Config_System[i] );
    }    
    /* 频率区   起始地址:0x18 长度:8 */
    start = 0x18;
    for( i=0 ; i<8; i++ )
    {
        E49x_SetRegister( start+i, E49x_Config_Frequency[i] );
    }       
    /* 数据率区 起始地址:0x20 长度:24 */
    start = 0x20;
    for( i=0 ; i<24; i++ )
    {
        E49x_SetRegister( start+i, E49x_Config_DataRate[i] );
    }   
    /* 基带区   起始地址:0x38 长度:29 */
    start = 0x38;
    for( i=0 ; i<29; i++ )
    {
        E49x_SetRegister( start+i, E49x_Config_Baseband[i] );
    } 
    /* 发射区   起始地址:0x55 长度:11 */
    start = 0x55;
    for( i=0 ; i<11; i++ )
    {
        E49x_SetRegister( start+i, E49x_Config_TX[i] );
    } 
       
    /* 原厂BUG需要兼容 1.4版本后的配置软件可以忽略此问题 */
    config = (~0x07) & E49x_GetRegister(0x09);
    E49x_SetRegister(0x09, config|0x02);   
    
    /* 配置频率步进基准 寄存器地址:0x64 计算公式为 2.5KHz*n */
    E49x_SetRegister(0x64, EBYTE_E49x_FREQUENCY_STEP );
    
    /* 配置频率步进累加 寄存器地址:0x63 计算公式为 步进基准*n */
    E49x_SetRegister(0x63, EBYTE_E49x_FREQUENCY_CHANNEL );
    
    /* 配置参数通过模式切换生效 */
    E49x_SetSleep();
    
}

/* !
 * @brief 模块进入休眠模式
 *
 * @return 0:正常 1:进入休眠失败
 */
uint8e_t E49x_GoSleep(void)
{
    return E49x_SetSleep();
}

/* !
 * @brief 初始化
 *
 * @return 0:正常 1:初始化失败
 */
uint8e_t E49x_Init(void)
{
    uint8e_t i;
    uint8e_t result = 0 ;
  
    /* 复位 参数会丢失 */
    E49x_Reset();
    
    /* 至少等待20ms */
    for (i=0;i<20;i++)
    {
        Ebyte_Port_DelayUs(1000);
    }    
    
    /* 进入待机配置模式 */
    result = E49x_SetStby();
    if( result!=0  ) return 1;
    
    /* 寄存器读写检查 */
    result = E49x_IsAlive();
    if( result!=0  ) return 1;
    
    /* 配置调制参数 */
    E49x_Config();    
    
    /* 默认开始监听 */
    result = E49x_GoReceive();
    if( result!=0  ) return 1;
    
    return result;
}

/* !
 * @brief 用户周期调用函数 辅助完成接收回调
 *
 * @return 0:正常 1:接收数据内容不正确
 */
uint8e_t E49x_TaskForPoll(void)
{
     uint16e_t irqSet = 0;
     uint8e_t getSize = 0;
     
    /* 接收状态 等待中断信号 */
    if( E49x_Status == GO_WAIT_RECEIVE && E49x_IRQ_Trigger )
    {       
        /* 获取状态 */
        irqSet = E49x_GetIRQ();

        /* 是否有数据接收(中断标识置位) */
        if( irqSet & GET_IRQ_PKT_OK )
        {          
            /* 模块待机 */
            E49x_SetStby();
            
            /* 读取数据 */
            E49x_GetFIFO( E49x_ReceiveBuffer, &getSize );
            
            /* 清除中断 */
            E49x_ClearIRQ();
            
            /* 回调接收完成函数 */
            Ebyte_Port_ReceiveCallback( GET_IRQ_PKT_OK , E49x_ReceiveBuffer ,getSize);
        }
        else
        {
            //to-do 如果开启了其他中断 需由用户自己处理
//            Ebyte_Port_ReceiveCallback( irqSet, 0 , 0 );
        }
        
        E49x_ClearIRQ();
        
        /* 复位 */
        E49x_IRQ_Trigger = 0;        
    }
    
    /* 非阻塞模式下 发送状态 等待中断信号 */
#if !(EBYTE_TRANSMIT_MODE_BLOCKED)    
    if( E49x_Status == GO_WAIT_TRANSMIT && E49x_IRQ_Trigger  )
    {
        /* 获取状态 */
        irqSet = E49x_GetIRQ();

        /* 是否发送完成了(中断标识置位) */
        if( irqSet & GET_IRQ_TX_DONE )
        {
            /* 模块待机 */
            E49x_SetStby();       
            
            /* 清除中断 */
            E49x_ClearIRQ();
            
            /* 发送完成 回调用户接口函数 */
            Ebyte_Port_TransmitCallback( irqSet );
        } 
        
        /* 复位 */
        E49x_IRQ_Trigger = 0;
    }
#endif    
    
    return 0;
}

/* !
 * @brief 中断触发器 
 * @note  一般来说都是IO中断中调用 主循环中TaskForPoll检测该信号
 */
void E49x_TaskForIRQ(void)
{
    E49x_IRQ_Trigger = 1;
}

/*!
 * @brief 获取模块名字
 * 
 * @return 指向名字字符串的指针 
 * @note 标准字符串 末尾含有结束符 '\0'
 */
uint8e_t* E49x_GetName(void)
{
    return E49x_NameString;
}

/*!
 * @brief 获取模块状态
 * 
 * @return 状态编码
 *        未初始化     GO_INIT          =0x00        
 *        正在切换状态 GO_BUSY          =0x01   
 *        待机/空闲    GO_STBY          =0x02   
 *        准备接收环境 GO_RECEIVE       =0x03   
 *        正在监听数据 GO_WAIT_RECEIVE  =0x04          
 *        准备发送环境 GO_TRANSMIT      =0x05   
 *        等待发送完成 GO_WAIT_TRANSMIT =0x06   
 *        休眠         GO_SLEEP         =0x07   
 *        内部错误     GO_ERROR         =0x08 
 */
uint8e_t E49x_GetStatus(void)
{
  return (uint8e_t)E49x_Status;;
}

/* !
 * @brief 获取模块程序版本
 * 
 * @return 8位的编码 
 * @note 例如0x10 代表V1.0
 */
uint8e_t E49x_GetDriverVersion(void)
{
  return  EBYTE_E49_PROGRAM_TYPE;
}
