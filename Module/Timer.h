/**
@brief 定时器模块
@version 3.0
*/

#ifndef _Timer_
#define _Timer_

#include "stm32f10x.h"

extern u8 timer_flag;//定时器标志位

extern void Timer1_Initialize(void);
extern void Timer2_Initialize(void);
extern void Timer3_Initialize(void);
extern void Timer4_Initialize(void);

#endif /* _Timer_ */
