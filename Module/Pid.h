/**
@brief 串级Pid控制模块
@version 3.0
*/

#ifndef _Pid_
#define _Pid_

#include "stm32f10x.h"

#define Pid_Move_Roll 2.0F //Pid移动倾角

extern float pid_target_roll;   //中心环输出的目标速度
extern float pid_target_speed;  //外环输出的目标速度
extern float pid_a_target_power;//内环输出电机A的目标输出
extern float pid_b_target_power;//内环输出电机B的目标输出

extern void Pid_Loca_System(const s16 a_speed, const s16 b_speed);
extern void Pid_Roll_System(const float roll, const float target);
extern void Pid_Speed_System(const s16 a_speed, const s16 b_speed, const float target);

#endif /* _Pid_ */
