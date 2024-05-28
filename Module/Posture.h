/**
@brief 姿态传感器模块
@version 3.0
*/

#ifndef _Posture_
#define _Posture_

#include "SoftIIC.h"

extern u8 posture_identity;  //校验数
extern s16 posture_acce_y;   //y轴加速度
extern s16 posture_acce_z;   //z轴加速度
extern float posture_roll;   //角度(°)

extern void Posture_Initialize(void);
extern void Posture_Refresh(void);

#endif
