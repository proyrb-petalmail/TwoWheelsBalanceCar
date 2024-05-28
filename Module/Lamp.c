/**
@brief 开发板指示灯
@version 3.2
*/

#include "Lamp.h"

#define Lamp_IO_Config const IO_Config lamp_config = {RCC_APB2Periph_GPIOC, GPIOC, GPIO_Pin_13, GPIO_Mode_Out_PP};

/**
@brief 初始化IO端口
*/
void Lamp_Initialize(void) {
     Lamp_IO_Config              //定义配置IO端口的结构体
     IO_Initialize(&lamp_config);//用结构体初始化IO端口
}

/**
@brief 点亮指示灯
*/
void Lamp_Light(void) {
     Lamp_IO_Config            //定义配置IO端口的结构体
     IO_Write(&lamp_config, 0);//向该端口位置0
}

/**
@brief 熄灭指示灯
*/
void Lamp_Dark(void) {
     Lamp_IO_Config            //定义配置IO端口的结构体
     IO_Write(&lamp_config, 1);//向该端口位置1
}
