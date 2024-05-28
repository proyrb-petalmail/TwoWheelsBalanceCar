/**
@brief 串级Pid控制模块
@version 3.0
*/

#include <math.h>
#include "Pid.h"
#include "Motor.h"

#define Pid_Loca_Ki      0.03F //积分系数
#define Pid_Loca_Gpa_Max 20.0F //积分限幅

#define Pid_Roll_Remedy    -1.9F //角度补偿
#define Pid_Roll_Kp        1.50F //比例系数
#define Pid_Roll_Ki        0.35F //积分系数
#define Pid_Roll_Gpa_Max   40.0F //积分限幅
#define Pid_Roll_Kd        3.30F //微分系数
#define Pid_Roll_Speed_Max 85.0F //速度限幅

#define Pid_Speed_Kp        140.00F //比例系数
#define Pid_Speed_Ki        35.000F //积分系数
#define Pid_Speed_Gpa_Max   CNT_Max //积分限幅
#define Pid_Speed_Kd        0.0000F //微分系数
#define Pid_Speed_Power_Max CNT_Max //输出限幅

float pid_target_roll;   //中心环输出的目标速度
float pid_target_speed;  //直立环输出的目标速度
float pid_a_target_power;//速度环输出电机A的目标输出
float pid_b_target_power;//速度环输出电机B的目标输出

/**
@brief 根据当前角速度筛选出电机速度进行积分得到目标角度
*/
void Pid_Loca_System(const s16 a_speed, const s16 b_speed) {
     static float inte;
     
     inte += Pid_Loca_Ki * (a_speed + b_speed);
     if(inte < -Pid_Loca_Gpa_Max) {
          pid_target_roll = Pid_Move_Roll;
     } else if(Pid_Loca_Gpa_Max < inte) {
          pid_target_roll = -Pid_Move_Roll;
     } else {
          pid_target_roll = 0.0F;
     }
}

/**
@brief 根据当前角度计算目标速度
*/
void Pid_Roll_System(const float roll, const float target) {
     float error = roll - target + Pid_Roll_Remedy;//补偿角度偏差并计算误差
     
     float rate = Pid_Roll_Kp * error;//比例
     
     static float inte;                                         //上次积分
     inte += Pid_Roll_Ki * error;                               //积分
     if(inte > Pid_Roll_Gpa_Max) inte = Pid_Roll_Gpa_Max;       //限幅
     else if(inte < -Pid_Roll_Gpa_Max) inte = -Pid_Roll_Gpa_Max;//限幅
     
     static float last_error;                        //上次误差
     float diff = Pid_Roll_Kd * (error - last_error);//微分
     last_error = error;                             //更新上次误差
     
     pid_target_speed = rate + inte + diff;                                                 //求和
     if(pid_target_speed > Pid_Roll_Speed_Max) pid_target_speed = Pid_Roll_Speed_Max;       //限幅
     else if(pid_target_speed < -Pid_Roll_Speed_Max) pid_target_speed = -Pid_Roll_Speed_Max;//限幅
}

/**
@brief 根据当前速度计算目标输出
*/
void Pid_Speed_System(const s16 a_speed, const s16 b_speed, const float target) {
     /* 电机A */
     float a_error = target - a_speed;     //计算误差
     float a_rate = Pid_Speed_Kp * a_error;//比例
     
     static float a_inte;                                             //积分
     a_inte += Pid_Speed_Ki * a_error;                                //积分
     if(a_inte > Pid_Speed_Gpa_Max) a_inte = Pid_Speed_Gpa_Max;       //限幅
     else if(a_inte < -Pid_Speed_Gpa_Max) a_inte = -Pid_Speed_Gpa_Max;//限幅
     
     static float a_last_error;                             //上次误差
     float a_diff = Pid_Speed_Kd * (a_error - a_last_error);//微分
     a_last_error = a_error;                                //更新上次误差
     
     pid_a_target_power = a_rate + a_inte + a_diff;                       //求和
     if(pid_a_target_power > CNT_Max) pid_a_target_power = CNT_Max;       //限幅
     else if(pid_a_target_power < -CNT_Max) pid_a_target_power = -CNT_Max;//限幅
     
     /* 电机B */
     float b_error = target - b_speed;     //计算误差
     float b_rate = Pid_Speed_Kp * b_error;//比例
     
     static float b_inte;                                             //积分
     b_inte += Pid_Speed_Ki * b_error;                                //积分
     if(b_inte > Pid_Speed_Gpa_Max) b_inte = Pid_Speed_Gpa_Max;       //限幅
     else if(b_inte < -Pid_Speed_Gpa_Max) b_inte = -Pid_Speed_Gpa_Max;//限幅
     
     static float b_last_error;                             //上次误差
     float b_diff = Pid_Speed_Kd * (b_error - b_last_error);//微分
     b_last_error = b_error;                                //更新上次误差
     
     pid_b_target_power = b_rate + b_inte + b_diff;                                               //求和
     if(pid_b_target_power > Pid_Speed_Power_Max) pid_b_target_power = Pid_Speed_Power_Max;       //限幅
     else if(pid_b_target_power < -Pid_Speed_Power_Max) pid_b_target_power = -Pid_Speed_Power_Max;//限幅
}
