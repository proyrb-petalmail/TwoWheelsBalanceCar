/**
@brief IO端口
@version 3.5
*/

#ifndef _Common_
#define _Common_

#include "stm32f10x.h"

typedef struct {
     u32 clock;            //RCC_APB2Periph_GPIOx(x=A..D)
     GPIO_TypeDef * port;  //GPIOx(x=A..D)
     u16 bit;              //GPIO_Pin_x(x=0..15).
     GPIOMode_TypeDef mode;//输入输出模式GPIO_Mode_Out_PP...
} IO_Config;

extern void IO_Initialize(const IO_Config * const config);
extern void IO_InitializeAndRemap(const IO_Config * const config, const u32 remap);
extern void IO_Write(const IO_Config * const config, const u8 data);
extern u8 IO_Read(const IO_Config * const config);

#endif /* _Common_ */
