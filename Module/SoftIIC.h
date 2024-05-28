/**
@brief 软件IIC通信模块
@version 2.5
*/

#ifndef _SoftIIC_
#define _SoftIIC_

#include "Common.h"

typedef struct { 
     IO_Config clock;//配置时钟线IO端口
     IO_Config data; //配置数据线IO端口
} SoftIIC_Config;

extern void SoftIIC_Initialize(const SoftIIC_Config * const config);
extern void SoftIIC_InitializeAndRemap(const SoftIIC_Config * const config, const u32 remap);
extern void SoftIIC_Start(const SoftIIC_Config * const config);
extern u8   SoftIIC_Send(const SoftIIC_Config * const config, const uint8_t data);
extern u8   SoftIIC_Accept(const SoftIIC_Config * const config, const uint8_t ack);
extern void SoftIIC_End(const SoftIIC_Config * const config);

#endif /* _SoftIIC_ */
