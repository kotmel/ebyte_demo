#include <stdarg.h>
#include "stm8l15x_usart.h"
#include "board_mini_printf.h"

/* !
 * @brief 目标硬件串口通信接口
 * 
 * @param data 写入的数据 1 Byte
 */
static void send_uart_data(uint8_t data)
{
  while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
  USART_SendData8(USART1,data);
}
 
/*
  功能：将int型数据转为2，8，10，16进制字符串
  参数：value --- 输入的int整型数
        str --- 存储转换的字符串
        radix --- 进制类型选择
  注意：8位单片机int字节只占2个字节
*/
static char *sky_itoa(int value, char *str, unsigned int radix)
{
  char list[] = "0123456789ABCDEF";
  unsigned int tmp_value;
  int i = 0, j, k = 0;
	char tmp;
//  if (NULL == str) {
  if (0 == str) {
//    return NULL;
    return 0;
  }
  if (2 != radix && 8 != radix && 10 != radix && 16 != radix) {
//    return NULL;
    return 0;
  }
  if (radix == 10 && value < 0) {
    //十进制且为负数
    tmp_value = (unsigned int)(0 - value);
    str[i++] = '-';
    k = 1;
  } else {
    tmp_value = (unsigned int)value;
  }
  //数据转换为字符串，逆序存储
  do {
    str[i ++] = list[tmp_value%radix];
    tmp_value /= radix;
  } while(tmp_value);
  str[i] = '\0';
  //将逆序字符串转换为正序
  
  for (j = k; j < (i+k)/2; j++) {
    tmp = str[j];
    str[j] = str[i-j-1+k];
    str[i-j-1+k] = tmp;
  }
  return str;
}
 
/*
  功能：将double型数据转为字符串
  参数：value --- 输入的double浮点数
        str --- 存储转换的字符串
        eps --- 保留小数位选择，至少保留一个小数位,至多保留4个小数位
  注意：8位单片机int字节只占2个字节
*/
static void sky_ftoa(double value, char *str, unsigned int eps)
{
  unsigned int integer;
  double decimal;
  char list[] = "0123456789";
  int i = 0, j, k = 0;
	char tmp;
	double pp = 0.1;
	int tmp_decimal;
  //将整数及小数部分提取出来
  if (value < 0) {
    decimal = (double)(((int)value) - value);
    integer = (unsigned int)(0 - value);
    str[i ++] = '-';
    k = 1;
  } else {
    integer = (unsigned int)(value);
    decimal = (double)(value - integer);
  }
  //整数部分数据转换为字符串，逆序存储
  do {
    str[i ++] = list[integer%10];
    integer /= 10;
  } while(integer);
  str[i] = '\0';
  //将逆序字符串转换为正序
  
  for (j = k; j < (i+k)/2; j++) {
    tmp = str[j];
    str[j] = str[i-j-1+k];
    str[i-j-1+k] = tmp;
  }
  //处理小数部分
  if (eps < 1 || eps > 4) {
    eps = 4;
  }
  
  //精度问题，防止输入1.2输出1.19等情况
  
  for (j = 0; j <= eps; j++) {
    pp *= 0.1;
  }
  decimal += pp;
  while (eps) {
    decimal *= 10;
    eps --;
  }
  tmp_decimal = (int)decimal;
  str[i ++] = '.';
  k = i;
  //整数部分数据转换为字符串，逆序存储
  do {
    str[i ++] = list[tmp_decimal%10];
    tmp_decimal /= 10;
  } while(tmp_decimal);
  str[i] = '\0';
  //将逆序字符串转换为正序
  for (j = k; j < (i+k)/2; j++) {
    tmp = str[j];
    str[j] = str[i-j-1+k];
    str[i-j-1+k] = tmp;
  }
  str[i] = '\0';
}
 
 
void mprintf(char * Data, ...)
{
  const char *s;
  int d;   
  char buf[16];
  uint8_t txdata;
  va_list ap;
  va_start(ap, Data);
  while ( * Data != 0 ) {				                          
    if ( * Data == 0x5c )  {									  
      switch ( *++Data ) {
        case 'r':							          
          txdata = 0x0d;
          send_uart_data(txdata);
          Data ++;
          break;
        case 'n':							          
          txdata = 0x0a;
          send_uart_data(txdata);
          Data ++;
          break;
        default:
          Data ++;
          break;
      }			 
    } else if ( * Data == '%') {									  
      switch ( *++Data ) {				
      case 's':										 
        s = va_arg(ap, const char *);
        for ( ; *s; s++) {
          send_uart_data(*((uint8_t *)s));
        }				
        Data++;				
        break;
      case 'd':			
        d = va_arg(ap, int);					
        sky_itoa(d, buf, 10);					
        for (s = buf; *s; s++) {
          send_uart_data(*((uint8_t *)s));
        }					
        Data++;				
        break;
      case 'x': {
        d = va_arg(ap, int);					
        sky_itoa(d, buf, 16);					
        for (s = buf; *s; s++) {
          send_uart_data(*((uint8_t *)s));
        }					
        Data++;			
        break;
      }
      case 'f': {
        double num = va_arg(ap, double);					
        sky_ftoa(num, buf, 4);
        for (s = buf; *s; s++) {
          send_uart_data(*((uint8_t *)s));
        }					
        Data++;			
        break;
      }
      default:
        Data++;				
        break;				
      }		 
    } else {
        send_uart_data(*((uint8_t *)Data));
        Data++;
    }
  }
}