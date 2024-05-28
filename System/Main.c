/**
@brief 主程序
@version 3.0
*/

#include <stdbool.h>
#include <stdio.h>
#include "Common.h"
#include "Lamp.h"
#include "Timer.h"
#include "Monitor.h"
#include "Gui.h"
#include "Posture.h"
#include "Motor.h"
#include "Pid.h"
#include "Usart.h"

#define Fall_Roll_Min 45.0F //倒地判定最小角度
#define Pick_Roll_Max 10.0F //拿起判定最大角度
#define Pick_Acce_Min 20000 //拿起判定最小加速度

float matlab[6];//输出数据

int main(void) {
     /* 设置中断优先级 */
     NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//配置中断优先级
     
     /* 初始化信号指示灯 */
     Lamp_Initialize();//初始化信号灯驱动端口位
     Lamp_Light();     //点亮信号灯
     
     /* 图形界面布局 */
     Monitor_Initialize();//初始化显示屏和驱动端口
     Gui_Component Matlab = {
          {{64, 0}, {64, 0}},//左上角
          {{0, 0}, {0, 0}},    //尺寸
          0,                   //分支
          "",                  //文本
          {1, 0, 0, 0, 2},     //样式
          Text,                //类型
     };
     Gui_Component Roll = {
          {{0, 0}, {0, 0}},//左上角
          {{0, 0}, {0, 0}},    //尺寸
          0,                   //分支
          "",                  //文本
          {1, 0, 0, 0, 2},     //样式
          Text,                //类型
     };
     Gui_Component * rootBranch[] = {&Roll, &Matlab, 0};//分支
     Gui_Component gui_root = {
          {{0, 0}, {0, 0}},      //左上角
          {{128, 64}, {128, 64}},//尺寸
          &rootBranch,           //分支
          "",                    //文本
          {0, 0, 0, 0, 0},       //样式
          Rectangle,             //类型
     };
     
     /* 初始化姿态传感器 */
     Posture_Initialize();
     
     /* 初始化电机 */
     Motor_Initialize();
     
     /* 初始化串口 */
     Usart_Initialize();
     
     /* 配置定时器 */
     Timer1_Initialize();
     Timer2_Initialize();
     Timer3_Initialize();
     Timer4_Initialize();
     
     while(1) {
          /* 受定时器中断影响 | 执行频率约50Hz */
          if(timer_flag & 0x01) {
               timer_flag &= 0xFE;//清除定时器标志位
               
               Monitor_DumpBuffer();  //清空显示屏上一帧的缓冲数据
               Gui_Execute(&gui_root);//用Gui的绘图数据在显示屏的缓冲数据上渲染图形界面
               Monitor_LoadBuffer();  //将渲染后的缓冲数据输送到显示屏
          }
          
          /* 受定时器中断影响 | 执行频率约100Hz */
          if(timer_flag & 0x02) {
               timer_flag &= 0xFD;//清除定时器标志位

               static bool fall;//是否倒地
               static bool pick;//是否拿起
               static u16  move;//是否移动
               
               if(fall || pick) {
                    /* 驱动电机 */
                    Motor_DriveA(0);
                    Motor_DriveB(0);
                    Motor_Stop();//倒地保护 & 拿起保护
                    
                    /* 上传数据 */
                    matlab[0] = posture_roll;
                    matlab[1] = 0;
                    matlab[2] = motorA_speed;
                    matlab[3] = motorB_speed;
                    matlab[4] = 0;
                    matlab[5] = 0;
               } else {
                    if(posture_roll < -Fall_Roll_Min || Fall_Roll_Min < posture_roll) {
                         fall = true; //倒地标志
                         Motor_Stop();//倒地保护
                    } else if(__fabs(posture_roll) < Pick_Roll_Max && posture_acce_z > Pick_Acce_Min) {
                         pick = true; //拿起标志
                         Motor_Stop();//拿起保护
                    } else {
                         if(move == 1) {
                              /* 串级Pid控制系统 */
                              Pid_Roll_System(matlab[0] = posture_roll, Pid_Move_Roll);            //速度控制系
                              Pid_Speed_System(matlab[2], matlab[3], matlab[1] = pid_target_speed);//输出控制系统
                         } else if(move == 2) {
                              /* 串级Pid控制系统 */
                              Pid_Roll_System(matlab[0] = posture_roll, -Pid_Move_Roll);           //速度控制系
                              Pid_Speed_System(matlab[2] = motorA_speed, matlab[3] = motorB_speed, matlab[1] = pid_target_speed);//输出控制系统
                         } else {
                              /* 串级Pid控制系统 */
                              Pid_Loca_System(matlab[2] = motorA_speed, matlab[3] = motorB_speed); //位置控制系统
                              Pid_Roll_System(matlab[0] = posture_roll, pid_target_roll);          //速度控制系
                              Pid_Speed_System(matlab[2], matlab[3], matlab[1] = pid_target_speed);//输出控制系统
                         }
                         
                         /* 驱动电机 */
                         Motor_DriveA(matlab[4] = pid_a_target_power);
                         Motor_DriveB(matlab[5] = pid_b_target_power);
                    }
               }
               
               /* 显示调试信息 */
               sprintf(Roll.text, "%d", (s16)posture_roll);//显示当前角度
               sprintf(Matlab.text, "%d", move = Usart_ReceiveData());//显示接收到的Matlab数据
          }
          
          /* 发送输出数据 */
          Usart_SendFloats(matlab, 6);//通过串口让蓝牙发送发送数据
     }
}
