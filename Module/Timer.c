/**
@brief 定时器模块
@version 3.0
*/

#include "Timer.h"
#include "Posture.h"
#include "Motor.h"

/**
@brief 定时器标志位
0000 0000
|||| ||||- Gui     刷新标志
|||| |||-- Posture 采样标志
|||| ||---
|||| |----
||||------
|||-------
||--------
|---------
*/
u8 timer_flag;

/**
@brief 初始化定时器1
*/
void Timer1_Initialize(void) {
     RCC->APB2ENR |= RCC_APB2Periph_TIM1;                                              //打开时钟
     TIM_InternalClockConfig(TIM1);                                                    //配置内部时钟信号作为从模式
     TIM_TimeBaseInitTypeDef timer = {0, TIM_CounterMode_Up, CNT_Max, TIM_CKD_DIV1, 0};//10KHz
     TIM_TimeBaseInit(TIM1, &timer);                                                   //初始化时基单元
     TIM_OCInitTypeDef timer_output;                                                   //定义输出配置结构体
     TIM_OCStructInit(&timer_output);                                                  //设置默认数值
     timer_output.TIM_OCMode = TIM_OCMode_PWM1;                                        //输出模式为PWM
     timer_output.TIM_OutputState = TIM_OutputState_Enable;                            //输出使能
     timer_output.TIM_Pulse = 0;                                                       //脉冲计数清零
     timer_output.TIM_OCPolarity = TIM_OCPolarity_High;                                //极性为高电平有效
     TIM_OC1Init(TIM1, &timer_output);                                                 //初始化通道1的输出比较
     TIM_OC1PreloadConfig(TIM1,TIM_OCPreload_Enable);                                  //使能通道1的预装载寄存器
     TIM_OC4Init(TIM1, &timer_output);                                                 //初始化通道4的输出比较
     TIM_OC4PreloadConfig(TIM1,TIM_OCPreload_Enable);                                  //使能通道4的预装载寄存器
     TIM_SetCompare1(TIM1, 0);                                                         //通道1的初始占空比为0
     TIM_SetCompare4(TIM1, 0);                                                         //通道4的初始占空比为0
     TIM_CtrlPWMOutputs(TIM1, ENABLE);                                                 //使能主PWM输出
     TIM_Cmd(TIM1, ENABLE);                                                            //启动计时
}

/**
@brief 定时器1更新事件中断函数
*/
void TIM1_UP_IRQHandler(void) {
     TIM_ClearITPendingBit(TIM1, TIM_IT_Update);//清除更新中断标志位
}

/**
@brief 初始化定时器2
*/
void Timer2_Initialize(void) {
     RCC->APB1ENR |= RCC_APB1Periph_TIM2;/* 打开时钟 */
     TIM_TimeBaseInitTypeDef timer = {0, TIM_CounterMode_Up, 65536 - 1, TIM_CKD_DIV1, 0};
     TIM_TimeBaseInit(TIM2, &timer);     /* 初始化时基单元 */
     TIM_ICInitTypeDef timer_input;
     TIM_ICStructInit(&timer_input);
     timer_input.TIM_Channel = TIM_Channel_1;
     timer_input.TIM_ICFilter = 0xF;
     TIM_ICInit(TIM2, &timer_input);
     timer_input.TIM_Channel = TIM_Channel_2;
     TIM_ICInit(TIM2, &timer_input);
     TIM_SetCounter(TIM2, Encoder_Counter_Middle);
     TIM_EncoderInterfaceConfig(TIM2, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
     TIM_Cmd(TIM2, ENABLE);              /* 启动计时 */
}

/**
@brief 定时器2中断函数
*/
void TIM2_IRQHandler(void) {
     TIM_ClearITPendingBit(TIM2, TIM_IT_Update);//清除更新中断标志位
}

/**
@brief 初始化定时器3
*/
void Timer3_Initialize(void) {
     RCC->APB1ENR |= RCC_APB1Periph_TIM3;/* 打开时钟 */
     TIM_TimeBaseInitTypeDef timer = {0, TIM_CounterMode_Up, 65536 - 1, TIM_CKD_DIV1, 0};
     TIM_TimeBaseInit(TIM3, &timer);     /* 初始化时基单元 */
     TIM_ICInitTypeDef timer_input;
     TIM_ICStructInit(&timer_input);
     timer_input.TIM_Channel = TIM_Channel_1;
     timer_input.TIM_ICFilter = 0xF;
     TIM_ICInit(TIM3, &timer_input);
     timer_input.TIM_Channel = TIM_Channel_2;
     TIM_ICInit(TIM3, &timer_input);
     TIM_SetCounter(TIM3, Encoder_Counter_Middle);
     TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
     TIM_Cmd(TIM3, ENABLE);              /* 启动计时 */
}

/**
@brief 定时器3中断函数
*/
void TIM3_IRQHandler(void) {
     TIM_ClearITPendingBit(TIM3, TIM_IT_Update);//清除更新中断标志位
}

/**
@brief 初始化定时器4
*/
void Timer4_Initialize(void) {
     NVIC_InitTypeDef nvic = {
          TIM4_IRQn,//指定中断通道
          3,        //先占优先级
          3,        //从占优先级
          ENABLE    //使能中断通道
     };
     NVIC_Init(&nvic);                                                                       //初始化指定中断
     RCC->APB1ENR |= RCC_APB1Periph_TIM4;                                                    //打开时钟
     TIM_InternalClockConfig(TIM4);                                                          //配置内部时钟信号作为从模式
     TIM_TimeBaseInitTypeDef timer = {3600 - 1, TIM_CounterMode_Up, 20 - 1, TIM_CKD_DIV1, 0};//1KHz
     TIM_TimeBaseInit(TIM4, &timer);                                                         //初始化时基单元
     TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);                                              //打开中断
     TIM_Cmd(TIM4, ENABLE);                                                                  //启动计时
}

/**
@brief 定时器4中断函数
*/
void TIM4_IRQHandler(void) {
     static u8 count;
     
     count++;//中断计数
     
     /* Gui 50Hz */
     if(count % 20 == 0) {
          timer_flag |= 0x01;//置标志位
     }
     
     /* Posture Motor 100Hz */
     if(count % 10 == 0) {
          timer_flag |= 0x02;//置标志位
          Posture_Refresh(); //刷新姿态数据
          Motor_Measure();   //刷新电机数据
     }
     
     TIM_ClearITPendingBit(TIM4, TIM_IT_Update);//清除更新中断标志位
}
