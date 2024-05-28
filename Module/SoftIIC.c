/**
@brief 软件IIC通信模块
@version 2.5
*/

#include "SoftIIC.h"

/**
@brief 初始化时钟线和数据线的IO端口
*/
void SoftIIC_Initialize(const SoftIIC_Config * const config) {
     IO_Initialize(&(config->clock));//用结构体初始化时钟线的IO端口
     IO_Initialize(&(config->data)); //用结构体初始化数据线的IO端口
}

/**
@brief 初始化时钟线和数据线的IO端口并重映射
*/
void SoftIIC_InitializeAndRemap(const SoftIIC_Config * const config, const u32 remap) {
     IO_InitializeAndRemap(&(config->clock), remap);//用结构体初始化时钟线的IO端口并重映射
     IO_InitializeAndRemap(&(config->data), remap); //用结构体初始化数据线的IO端口并重映射
}

/**
@brief 发送起始信号
*/
void SoftIIC_Start(const SoftIIC_Config * const config) {
     /* 将时钟线和数据线上拉至高电平 | 先数据线后时钟线 | 先时钟线后数据线可能会被认定为终止信号 */
     IO_Write(&(config->data), 1); //向数据线的端口位置1
     IO_Write(&(config->clock), 1);//向时钟线的端口位置1
     
     /* 将时钟线和数据线下拉至低电平 | 先数据线后时钟线 | 先时钟线后数据线不被认定为起始信号 */
     IO_Write(&(config->data), 0); //向数据线的端口位置0
     IO_Write(&(config->clock), 0);//向时钟线的端口位置0
}

/**
@brief 发送一个字节数据并接收回应
*/
u8   SoftIIC_Send(const SoftIIC_Config * const config, const uint8_t data) {
     u8 ack;//储存接收到的回应
     
     /* 由高位到低位 | 逐位发送 | 此时时钟线一定是低电平 | 获得数据线控制权 */
     for (u8 times = 0; times < 8; times++) {
          IO_Write(&(config->data), data & (0x80 >> times));//由高到低逐位放置数据
          
          /* 放置完数据后拉高时钟线 | 从机才可以读出该位数据 | 然后继续拉低时钟线准备放置下一位数据或发送结束开始读取回应 */
          IO_Write(&(config->clock), 1);//向时钟线的端口位置1
          IO_Write(&(config->clock), 0);//向时钟线的端口位置0
     }
     
     /* 此时时钟线一定是低电平 | 上拉数据线至高电平 | 从机获得数据线控制权 | 接收方放置回应 | 继续拉高时钟线至高电平 */
     IO_Write(&(config->data), 1); //向数据线的端口位置1
     IO_Write(&(config->clock), 1);//向时钟线的端口位置1
     
     /* 读取数据线上的数据 | 继续拉低时钟线至低电平 | 准备后续其他操作 */
     ack = IO_Read(&(config->data));//从数据线的端口位读数据作为回应储存
     IO_Write(&(config->clock), 0); //向时钟线的端口位置0
     
     return ack;//返回接收到的回应
}

/**
@brief 接收一个字节数据并发送回应
*/
u8   SoftIIC_Accept(const SoftIIC_Config * const config, const uint8_t ack) {
     u8 data = 0x00;//储存接收到的数据 | 清空数据
     
     /* 此时时钟线一定是低电平 | 上拉数据线等于释放数据线的控制权 | 从机获得数据线控制权并在此时放置最高位数据 */
     IO_Write(&(config->data), 1);//向数据线的端口位置1
     
     /* 由高位到低位 | 逐位接收 | 拉高时钟线至高电平 | 读取数据线上放置的数据 | 再拉低时钟线至低电平 | 允许从机继续放置数据 */
     for (u8 times = 0; times < 8; times++) {
          IO_Write(&(config->clock), 1);                          //向时钟线的端口位置1
          if(IO_Read(&(config->data)) == 1) data |= 0x80 >> times;//读取从机放置的数据进行储存
          IO_Write(&(config->clock), 0);                          //向时钟线的端口位置0
     }
     
     /* 此时时钟线一定是低电平 | 上拉数据线至指定电平作为回应 | 拉高时钟线至高电平允许从机读取回应 | 继续拉低时钟线至低电平准备后续操作 */
     IO_Write(&(config->data), ack);//向数据线的端口位置指定数据
     IO_Write(&(config->clock), 1); //向时钟线的端口位置1
     IO_Write(&(config->clock), 0); //向时钟线的端口位置0
     
     return data;//返回读取到的数据
}

/**
@brief 发送终止信号
*/
void SoftIIC_End(const SoftIIC_Config * const config) {
     /* 此时时钟线一定是低电平 | 先下拉数据线 | 然后拉高时钟线 | 最后下拉数据线 | 在时钟线高电平期间下拉数据线会被认定为终止信号 */
     IO_Write(&(config->data), 0); //向数据线的端口位置0
     IO_Write(&(config->clock), 1);//向时钟线的端口位置1
     IO_Write(&(config->data), 1); //向数据线的端口位置1
}
