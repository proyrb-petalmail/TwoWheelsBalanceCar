/**
@brief 串口通信模块
@version 2.0
*/

#include "Common.h"
#include "Usart.h"

typedef union {
     float data;//浮点数据
     u8 byte[4];//分字节数据
} Usart_Float;

static const IO_Config tx_config = {RCC_APB2Periph_GPIOB, GPIOB, GPIO_Pin_10, GPIO_Mode_AF_PP};
static const IO_Config rx_config = {RCC_APB2Periph_GPIOB, GPIOB, GPIO_Pin_11, GPIO_Mode_IN_FLOATING};

/**
@brief 初始化串口通信端口
*/
void Usart_Initialize(void) {
     RCC->APB1ENR |= RCC_APB1Periph_USART3;//使能串口时钟
     IO_Initialize(&tx_config);            //用结构体初始化发送线端口位
     IO_Initialize(&rx_config);            //用结构体初始化接收线端口位
     
     USART_InitTypeDef usart = {
          115200, USART_WordLength_8b, USART_StopBits_1, USART_Parity_No, USART_Mode_Tx | USART_Mode_Rx, USART_HardwareFlowControl_None
     };                         //配置用来初始化串口的结构体
     USART_Init(USART3, &usart);//用结构体初始化串口
     USART_Cmd(USART3,ENABLE);  //使能串口
}

/**
@brief 发送一个字节数据
*/
void Usart_SendByte(const u8 byte) {
     u8 state;//储存寄存器状态
     
     USART3->DR = byte & (u16)0x01FF;//向数据寄存器写入数据
     do {
          if((USART3->SR & USART_FLAG_TXE) != 0) state = 1;
          else state = 0;
     } while(!state);//等待数据从数据寄存器转移至移位寄存器 | 下次向数据寄存器写入数据时自动清除该标志位
}

/**
@brief 发送一个字符串
*/
void Usart_SendString(const char * string) {
     u8 state;//储存寄存器状态
     
     while(*string != '\0') {
          Usart_SendByte(*string);//逐个发送字节
          string++;               //指向下一个字符
     }
     do {
          if((USART3->SR & USART_FLAG_TC) != 0) state = 1;
          else state = 0;
     } while(!state);//等待移位寄存器中的数据发送完毕 | 下次向数据寄存器写入数据时自动清除该标志位
}

/**
@brief 发送一个浮点数
*/
void Usart_SendFloats(const float * const data, const u8 number) {
     u8 state;//储存寄存器状态
     Usart_Float union_data;//用联合体储存数据
     for(u8 index = 0; index < number; index++) {
          union_data.data = data[index];
          for(u8 times = 0; times < 4; times++) {
               Usart_SendByte(union_data.byte[times]);//发送字节数据
          }
     }
     do {
          if((USART3->SR & USART_FLAG_TC) != 0) state = 1;
          else state = 0;
     } while(!state);//等待移位寄存器中的数据发送完毕 | 下次向数据寄存器写入数据时自动清除该标志位
}

/**
@brief 接收两个字节的数据
*/
u16  Usart_ReceiveData(void) {     
     return (u16)(USART3->DR & (u16)0x01FF);//返回接收到的数据(2字节)
}
