/**
@brief 直流电机模块
@version 3.0
*/

#include "Common.h"
#include "Motor.h"

#define Motor_EncoderA_IO_Config const IO_Config encodera_config = {RCC_APB2Periph_GPIOA, GPIOA, GPIO_Pin_0, GPIO_Mode_IN_FLOATING};
#define Motor_EncoderB_IO_Config const IO_Config encoderb_config = {RCC_APB2Periph_GPIOA, GPIOA, GPIO_Pin_1 , GPIO_Mode_IN_FLOATING};
#define Motor_PwmA_IO_Config const IO_Config pwma_config = {RCC_APB2Periph_GPIOA, GPIOA, GPIO_Pin_8, GPIO_Mode_AF_PP};
#define Motor_PwmB_IO_Config const IO_Config pwmb_config = {RCC_APB2Periph_GPIOA, GPIOA, GPIO_Pin_11, GPIO_Mode_AF_PP};
static const IO_Config ain1_config = {RCC_APB2Periph_GPIOB, GPIOB, GPIO_Pin_13, GPIO_Mode_Out_PP};
static const IO_Config ain2_config = {RCC_APB2Periph_GPIOB, GPIOB, GPIO_Pin_12, GPIO_Mode_Out_PP};
static const IO_Config bin1_config = {RCC_APB2Periph_GPIOB, GPIOB, GPIO_Pin_14, GPIO_Mode_Out_PP};
static const IO_Config bin2_config = {RCC_APB2Periph_GPIOB, GPIOB, GPIO_Pin_15, GPIO_Mode_Out_PP};

s8 motorA_speed, motorB_speed;//电机转速

/**
@brief 初始化电机
*/
void Motor_Initialize(void) {
     Motor_EncoderA_IO_Config        //定义编码器A相IO端口配置结构体
     Motor_EncoderB_IO_Config        //定义编码器B相IO端口配置结构体
     IO_Initialize(&encodera_config);//用结构体初始化编码器A相端口
     IO_Initialize(&encoderb_config);//用结构体初始化编码器B相端口
     
     Motor_PwmA_IO_Config        //定义PwmA输出IO端口配置结构体
     Motor_PwmB_IO_Config        //定义PwmB输出IO端口配置结构体
     IO_Initialize(&pwma_config);//用结构体初始化PwmA输出端口
     IO_Initialize(&pwmb_config);//用结构体初始化PwmB输出端口
     
     IO_Initialize(&ain1_config);//初始化ain1端口
     IO_Initialize(&ain2_config);//初始化ain2端口
     IO_Initialize(&bin1_config);//初始化bin1端口
     IO_Initialize(&bin2_config);//初始化bin2端口
}

/**
@brief 停止电机
*/
void Motor_Stop(void) {
     /* 停止A电机 */
     IO_Write(&ain1_config, 0);//向该端口位置0
     IO_Write(&ain2_config, 0);//向该端口位置0
     
     /* 停止B电机 */
     IO_Write(&bin1_config, 0);//向该端口位置0
     IO_Write(&bin2_config, 0);//向该端口位置0
}

/**
@brief 测速
*/
void Motor_Measure(void) {
     motorA_speed = TIM_GetCounter(TIM2) - Encoder_Counter_Middle;//计算前后差值得到转速
     TIM_SetCounter(TIM2, Encoder_Counter_Middle);
     motorB_speed = TIM_GetCounter(TIM3) - Encoder_Counter_Middle;//计算前后差值得到转速
     TIM_SetCounter(TIM3, Encoder_Counter_Middle);
}

/**
@brief 调整电机
*/
void Motor_DriveA(const s16 power) {
     if(power < 0) {
          IO_Write(&ain1_config, 0);    //向该端口位置0
          IO_Write(&ain2_config, 1);    //向该端口位置1
          TIM_SetCompare1(TIM1, -power);//设置通道1的占空比即输出电压
     } else if(power > 0) {
          IO_Write(&ain1_config, 1);   //向该端口位置1
          IO_Write(&ain2_config, 0);   //向该端口位置0
          TIM_SetCompare1(TIM1, power);//设置通道1的占空比即输出电压
     }
}

/**
@brief 调整电机
*/
void Motor_DriveB(const s16 power) {
     if(power < 0) {
          IO_Write(&bin1_config, 1);    //向该端口位置1
          IO_Write(&bin2_config, 0);    //向该端口位置0
          TIM_SetCompare4(TIM1, -power);//设置通道4的占空比即输出电压
     } else if(power > 0) {
          IO_Write(&bin1_config, 0);   //向该端口位置0
          IO_Write(&bin2_config, 1);   //向该端口位置1
          TIM_SetCompare4(TIM1, power);//设置通道4的占空比即输出电压
     }
}
