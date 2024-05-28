/**
@brief 串口通信模块
@version 2.0
*/

#ifndef _Usart_
#define _Usart_

#include "stm32f10x.h"

extern void Usart_Initialize(void);
extern void Usart_SendByte(const u8 byte);
extern void Usart_SendString(const char * const string);
extern void Usart_SendFloats(const float * const data, const u8 number);
extern u16  Usart_ReceiveData(void);

#endif /* _Usart_ */
