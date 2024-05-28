/**
@brief IO端口
@version 3.5
*/

#include "Common.h"

/**
@brief 初始化指定IO
*/
void IO_Initialize(const IO_Config * const config) {
     RCC->APB2ENR |= config->clock;                                        //启动指定端口的时钟
     GPIO_InitTypeDef GPIO = {config->bit, GPIO_Speed_50MHz, config->mode};//初始化结构体
     GPIO_Init(config->port, &GPIO);                                       //用结构体初始化指定端口
}

/**
@brief 初始化指定IO并重映射
*/
void IO_InitializeAndRemap(const IO_Config * const config, const u32 remap) {
     RCC->APB2ENR |= config->clock | RCC_APB2Periph_AFIO;                  //启动指定端口的时钟和复用时钟
     GPIO_PinRemapConfig(remap, ENABLE);                                   //重映射端口
     GPIO_InitTypeDef GPIO = {config->bit, GPIO_Speed_50MHz, config->mode};//初始化结构体
     GPIO_Init(config->port, &GPIO);                                       //用结构体初始化指定端口
}

/**
@brief 向指定IO写数据
*/
void IO_Write(const IO_Config * const config, const u8 data) {
     if(data != 0) config->port->BSRR = config->bit;//置该位为1
     else config->port->BRR = config->bit;          //置该位为0
}

/**
@brief 从指定IO读数据
*/
u8 IO_Read(const IO_Config * const config) {
     if((config->port->IDR & config->bit) != 0) return 1;//读该位为1
     else return 0;                                      //读该位为0
}
