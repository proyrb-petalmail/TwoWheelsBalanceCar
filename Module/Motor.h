/**
@brief 直流电机模块
@version 3.0
*/

#ifndef _Motor_
#define _Motor_

#include "stm32f10x.h"

#define CNT_Max 7199 //计数最大值
#define Encoder_Counter_Middle 32768
#define MotorA_Speed_Max 84 //A电机转速最大值
#define MotorB_Speed_Max 89 //B电机转速最大值

extern s8 motorA_speed, motorB_speed;//电机转速

extern void Motor_Initialize(void);
extern void Motor_Stop(void);
extern void Motor_Measure(void);
extern void Motor_DriveA(const s16 power);
extern void Motor_DriveB(const s16 power);

#endif /* _Motor_ */
