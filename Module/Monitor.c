/**
@brief 显示屏模块
@version 3.0
*/

#include <math.h>
#include "Monitor.h"

#define DeviceAddress 0x78 //设备的总线地址

#define Monitor_PageNumber 8   //页数目
#define Monitor_PageLength 128 //页长度

#define Monitor_XNumber 128 //横坐标像素数目
#define Monitor_YNumber 64  //纵坐标像素数目

#define LineAccuracy 0.000001F //直线精确程度

#define SymbolOffset 32 //符号集相对ASCII偏移量
#define SymbolHeight 16 //字符像素高度

#define Contain(dot) (((dot.x > -1)&&(dot.x < Monitor_XNumber))&&((dot.y > -1)&&(dot.y < Monitor_YNumber))) //是否位于显示器内

static const SoftIIC_Config softiic_config = {
     {RCC_APB2Periph_GPIOB, GPIOB, GPIO_Pin_9, GPIO_Mode_Out_OD},
     {RCC_APB2Periph_GPIOB, GPIOB, GPIO_Pin_8, GPIO_Mode_Out_OD}
};
static u8 buffer[Monitor_PageNumber][Monitor_PageLength];//缓冲数据
static const u16 symbols[95][8] = {
     {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,},//space
     {0x0000, 0x0000, 0x0000, 0xCFFF, 0xCFFF, 0x0000, 0x0000, 0x0000,},//!
     {0x0000, 0x0007, 0x0007, 0x0000, 0x0000, 0x0007, 0x0007, 0x0000,},//"
     {0x1008, 0xF808, 0x17E8, 0x101F, 0xF808, 0x17E8, 0x101F, 0x1008,},//#
     {0x0C30, 0x1048, 0x2084, 0xFFFF, 0xFFFF, 0x2104, 0x1208, 0x0C30,},//$
     {0xC00E, 0x3011, 0x0C11, 0x7311, 0x88CE, 0x8830, 0x880C, 0x7003,},//%
     {0x2C00, 0x431E, 0x80A1, 0x80E1, 0x8721, 0x5821, 0x601E, 0x9C00,},//&
     {0x0000, 0x0000, 0x0000, 0x0007, 0x0007, 0x0000, 0x0000, 0x0000,},//'
     {0x0000, 0x0000, 0x03C0, 0x1C38, 0x6006, 0x8001, 0x0000, 0x0000,},//(
     {0x0000, 0x0000, 0x8001, 0x6006, 0x1C38, 0x03C0, 0x0000, 0x0000,},//)
     {0x0040, 0x0C80, 0x0300, 0x01F0, 0x0300, 0x0C80, 0x0040, 0x0000,},//*
     {0x0180, 0x0180, 0x0180, 0x0FF0, 0x0FF0, 0x0180, 0x0180, 0x0180,},//+
     {0x0000, 0x0000, 0x8000, 0x7000, 0x3000, 0x0000, 0x0000, 0x0000,},//,
     {0x0180, 0x0180, 0x0180, 0x0180, 0x0180, 0x0180, 0x0180, 0x0180,},//-
     {0x0000, 0x0000, 0x0000, 0xC000, 0xC000, 0x0000, 0x0000, 0x0000,},//.
     {0xC000, 0x3000, 0x0C00, 0x0300, 0x00C0, 0x0030, 0x000C, 0x0003,},///
     {0x0FF0, 0x300C, 0x4002, 0x8001, 0x8001, 0x4002, 0x300C, 0x0FF0,},//0
     {0x0000, 0x8008, 0x8004, 0x8002, 0xFFFF, 0x8000, 0x8000, 0x0000,},//1
     {0xE00C, 0x9802, 0x8401, 0x8201, 0x8101, 0x8081, 0x8042, 0x803C,},//2
     {0x301C, 0x4002, 0x8001, 0x8081, 0x8081, 0x8081, 0x4342, 0x3C3C,},//3
     {0x1800, 0x1600, 0x1180, 0x1060, 0x1018, 0x1006, 0xFFFF, 0x1000,},//4
     {0x30FF, 0x4041, 0x8021, 0x8021, 0x8021, 0x8021, 0x4041, 0x3F81,},//5
     {0x3FFC, 0x4082, 0x8041, 0x8041, 0x8041, 0x8041, 0x4082, 0x3F04,},//6
     {0xC001, 0x3001, 0x0C01, 0x0301, 0x00C1, 0x0031, 0x000D, 0x0003,},//7
     {0x3C3C, 0x4242, 0x8241, 0x8181, 0x8181, 0x8241, 0x4242, 0x3C3C,},//8
     {0x20FC, 0x4102, 0x8201, 0x8201, 0x8201, 0x8201, 0x4102, 0x3FFC,},//9
     {0x0000, 0x0000, 0x0000, 0x0660, 0x0660, 0x0000, 0x0000, 0x0000,},//:
     {0x0000, 0x0000, 0x0800, 0x0E60, 0x0660, 0x0000, 0x0000, 0x0000,},//;
     {0x0000, 0x0080, 0x0140, 0x0220, 0x0410, 0x0808, 0x1004, 0x0000,},//<
     {0x0000, 0x0140, 0x0140, 0x0140, 0x0140, 0x0140, 0x0140, 0x0000,},//=
     {0x0000, 0x1004, 0x0808, 0x0410, 0x0220, 0x0140, 0x0080, 0x0000,},//>
     {0x001C, 0x0002, 0x0001, 0xCC01, 0xCE01, 0x0181, 0x0042, 0x003C,},//?
     {0x3FFC, 0x4002, 0x9FF9, 0xA005, 0xA005, 0x9FF9, 0x2002, 0x1FFC,},//@
     {0xF000, 0x0F00, 0x04F0, 0x040F, 0x040F, 0x04F0, 0x0F00, 0xF000,},//A
     {0xFFFF, 0x8081, 0x8081, 0x8081, 0x8081, 0x80C2, 0x413C, 0x3E00,},//B
     {0x3FFC, 0x4002, 0x8001, 0x8001, 0x8001, 0x8001, 0x4002, 0x300C,},//C
     {0xFFFF, 0x8001, 0x8001, 0x8001, 0x8001, 0x4002, 0x300C, 0x0FF0,},//D
     {0xFFFF, 0x8081, 0x8081, 0x8081, 0x8081, 0x8081, 0x8081, 0x8001,},//E
     {0xFFFF, 0x0081, 0x0081, 0x0081, 0x0081, 0x0081, 0x0081, 0x0001,},//F
     {0x3FF8, 0x4002, 0x8201, 0x8201, 0x8201, 0x8201, 0x4202, 0x3E0C,},//G
     {0xFFFF, 0x0080, 0x0080, 0x0080, 0x0080, 0x0080, 0x0080, 0xFFFF,},//H
     {0x0000, 0x8001, 0x8001, 0xFFFF, 0xFFFF, 0x8001, 0x8001, 0x0000,},//I
     {0x0000, 0x4001, 0x8001, 0x8001, 0x7FFF, 0x1FFF, 0x0001, 0x0000,},//J
     {0xFFFF, 0x0300, 0x0480, 0x0840, 0x1020, 0x2010, 0x400C, 0x8003,},//K
     {0xFFFF, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,},//L
     {0xFFFF, 0x0038, 0x01C0, 0x0E00, 0x0E00, 0x01C0, 0x0038, 0xFFFF,},//M
     {0xFFFF, 0x000C, 0x0030, 0x00C0, 0x0300, 0x0C00, 0x3000, 0xFFFF,},//N
     {0x3FF8, 0x4002, 0x8001, 0x8001, 0x8001, 0x8001, 0x4002, 0x3FF8,},//O
     {0xFFFF, 0x0101, 0x0101, 0x0101, 0x0101, 0x0101, 0x0082, 0x007C,},//P
     {0x3FFC, 0x4002, 0x8001, 0x8001, 0x8001, 0xA001, 0x4002, 0xBFFC,},//Q
     {0xFFFF, 0x0101, 0x0101, 0x0301, 0x0D01, 0x3101, 0x6082, 0x807C,},//R
     {0x203C, 0x4042, 0x8081, 0x8081, 0x8081, 0x8081, 0x4102, 0x3E04,},//S
     {0x0001, 0x0001, 0x0001, 0xFFFF, 0xFFFF, 0x0001, 0x0001, 0x0001,},//T
     {0x3FFF, 0x4000, 0x8000, 0x8000, 0x8000, 0x8000, 0x4000, 0x3FFF,},//U
     {0x000F, 0x00F0, 0x0F00, 0xF000, 0xF000, 0x0F00, 0x00F0, 0x000F,},//V
     {0x03FF, 0xFC00, 0x03E0, 0x001F, 0x001F, 0x03E0, 0xFC00, 0x03FF,},//W
     {0xC003, 0x300C, 0x0C30, 0x03C0, 0x03C0, 0x0C30, 0x300C, 0xC003,},//X
     {0x0007, 0x0038, 0x01C0, 0xFE00, 0xFE00, 0x01C0, 0x0038, 0x0007,},//Y
     {0xC001, 0xB001, 0x8C01, 0x8301, 0x80C1, 0x8031, 0x800D, 0x8003,},//Z
     {0x0000, 0x0000, 0xFFFF, 0x8001, 0x8001, 0x8001, 0x0000, 0x0000,},//[
     {0x0003, 0x000C, 0x0030, 0x00C0, 0x0300, 0x0C00, 0x3000, 0xC000,},//"\"
     {0x0000, 0x0000, 0x8001, 0x8001, 0x8001, 0xFFFF, 0x0000, 0x0000,},//]
     {0x0000, 0x0030, 0x000C, 0x0003, 0x0003, 0x000C, 0x0030, 0x0000,},//^
     {0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,},//_
     {0x0000, 0x0000, 0x0001, 0x0003, 0x0006, 0x0004, 0x0000, 0x0000,},//`
     {0x3800, 0x4480, 0x8240, 0x8240, 0x8240, 0x4480, 0x7F00, 0x8000,},//a
     {0xFFFF, 0x4080, 0x8040, 0x8040, 0x8040, 0x8040, 0x4080, 0x3F00,},//b
     {0x3F00, 0x4080, 0x8040, 0x8040, 0x8040, 0x8040, 0x4080, 0x3300,},//c
     {0x3F00, 0x4080, 0x8040, 0x8040, 0x8040, 0x8040, 0x4080, 0xFFFF,},//d
     {0x3F00, 0x4080, 0x8840, 0x8840, 0x8840, 0x8840, 0x8880, 0x4700,},//e
     {0x0040, 0x0040, 0xFFFC, 0x0042, 0x0041, 0x0041, 0x0041, 0x0041,},//f
     {0x20FC, 0x4102, 0x8201, 0x8201, 0x8201, 0x8201, 0x4102, 0x3FFF,},//g
     {0xFFFF, 0x0080, 0x0040, 0x0040, 0x0040, 0x0040, 0x0080, 0xFF00,},//h
     {0x0000, 0x0000, 0x0000, 0xFEC0, 0xFEC0, 0x0000, 0x0000, 0x0000,},//i
     {0x0000, 0x0008, 0x8008, 0x7FFB, 0x3FFB, 0x0000, 0x0000, 0x0000,},//j
     {0xFFFF, 0x0200, 0x0500, 0x0900, 0x1080, 0x2080, 0x4040, 0x8040,},//k
     {0x0000, 0x0001, 0x0003, 0x3FFE, 0x4000, 0x8000, 0x8000, 0x0000,},//l
     {0x0040, 0xFF80, 0x0040, 0x0040, 0xFF80, 0x0040, 0x0040, 0xFF80,},//m
     {0x0040, 0xFF80, 0x0080, 0x0040, 0x0040, 0x0040, 0x0080, 0xFF80,},//n
     {0x3F00, 0x4080, 0x8040, 0x8040, 0x8040, 0x8040, 0x4080, 0x3F00,},//o
     {0x0040, 0xFF80, 0x0480, 0x0840, 0x0840, 0x0840, 0x0480, 0x0300,},//p
     {0x0300, 0x0480, 0x0840, 0x0840, 0x0840, 0x0480, 0xFF80, 0x0040,},//q
     {0x0040, 0xFF80, 0x0080, 0x0040, 0x0040, 0x0040, 0x0040, 0x0080,},//r
     {0x2100, 0x4280, 0x8440, 0x8440, 0x8440, 0x8440, 0x4880, 0x3100,},//s
     {0x0040, 0x0040, 0x3FF8, 0x4040, 0x8040, 0x8040, 0x0040, 0x0040,},//t
     {0x3FC0, 0x4000, 0x8000, 0x8000, 0x8000, 0x8000, 0x4000, 0xBFC0,},//u
     {0x00C0, 0x0700, 0x3800, 0xC000, 0xC000, 0x3800, 0x0700, 0x00C0,},//v
     {0x0FC0, 0xF000, 0x0E00, 0x01C0, 0x01C0, 0x0E00, 0xF000, 0x0FC0,},//w
     {0x8040, 0x4080, 0x3300, 0x0C00, 0x0C00, 0x3300, 0x4080, 0x8040,},//x
     {0x23C0, 0x4400, 0x8800, 0x8800, 0x8800, 0x8800, 0x4400, 0x3FC0,},//y
     {0x8040, 0xA040, 0x9040, 0x8840, 0x8440, 0x8240, 0x8140, 0x80C0,},//z
     {0x0000, 0x0000, 0x0180, 0x7E7E, 0x8001, 0x8001, 0x0000, 0x0000,},//{
     {0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000,},//|
     {0x0000, 0x0000, 0x8001, 0x8001, 0x7E7E, 0x0180, 0x0000, 0x0000,},//}
     {0x0080, 0x0040, 0x0040, 0x0080, 0x0100, 0x0200, 0x0200, 0x0100,},//~
};//可显示符号集

/**
@brief 向显示器发送指令
*/
static void WriteCommand(u8 command) {
     SoftIIC_Start(&softiic_config);              //发送起始信号
     SoftIIC_Send(&softiic_config, DeviceAddress);//命令屏幕进入写模式
     SoftIIC_Send(&softiic_config, 0x00);         //从机进入收命令模式
     SoftIIC_Send(&softiic_config, command);      //从机接收命令
     SoftIIC_End(&softiic_config);                //发送结束信号
}

/**
@brief 向显示器发送数据
*/
static void WriteData(u8 data) {
     SoftIIC_Start(&softiic_config);              //发送起始信号
     SoftIIC_Send(&softiic_config, DeviceAddress);//命令屏幕进入写模式
     SoftIIC_Send(&softiic_config, 0x40);         //从机进入收数据模式
     SoftIIC_Send(&softiic_config, data);         //从机接收命令
     SoftIIC_End(&softiic_config);                //发送结束信号
}

/**
@brief 上电后首次初始化屏幕
*/
void Monitor_Initialize() {
     /* 初始化通信接口 */
     SoftIIC_Initialize(&softiic_config);
     
     /* 执行固定指令流程 */     
     WriteCommand(0xAE);//关闭显示
     
     WriteCommand(0xD5);//设置振荡器频率
     WriteCommand(0x80);
     
     WriteCommand(0xA8);//设置多路复用率
     WriteCommand(0x3F);
     
     WriteCommand(0xD3);//设置显示偏移
     WriteCommand(0x00);
     
     WriteCommand(0x40);//设置显示开始行
     
     WriteCommand(0xA1);//设置左右方向，0xA1正常 0xA0左右反置
     
     WriteCommand(0xC8);//设置上下方向，0xC8正常 0xC0上下反置
     
     WriteCommand(0xDA);//设置COM引脚硬件配置
     WriteCommand(0x12);
	
     WriteCommand(0x81);//设置对比度控制
     WriteCommand(0xFF);
     
     WriteCommand(0xD9);//设置预充电周期
     WriteCommand(0xF1);
     
     WriteCommand(0xDB);//设置VCOMH取消选择级别
     WriteCommand(0x30);
     
     WriteCommand(0xA4);//设置整个显示打开/关闭
     
     WriteCommand(0xA6);//设置正常/倒转显示
     
     //配置页设置并清屏
	for (u8 page = 0; page < 8; page++) {
          WriteCommand(0xB0 | page);//设置页地址
          WriteCommand(0x10 | 0x00);//设置列地址高4位
          WriteCommand(0x00 | 0x00);//设置列地址低4位
		for(u8 i = 0; i < 128; i++) {
			WriteData(0x00);
		}
	}
     
     WriteCommand(0x8D);//设置充电泵
     WriteCommand(0x14);
     
     WriteCommand(0xAF);//开启显示
}

/**
@brief 清空缓冲数据
*/
void Monitor_DumpBuffer(void) {     
	for (u8 page = 0; page < Monitor_PageNumber; page++) {
		for(u8 length = 0; length < Monitor_PageLength; length++) {
			buffer[page][length] = 0x00;//清空数据
		}
	}
}

/**
@brief 渲染缓冲数据
*/
void Monitor_LoadBuffer(void) {
     WriteCommand(0x20);                          //设置水平寻址
     SoftIIC_Start(&softiic_config);              //发送起始信号
     SoftIIC_Send(&softiic_config, DeviceAddress);//命令屏幕进入写模式
     SoftIIC_Send(&softiic_config, 0x40);         //从机进入收数据模式
     for(u8 page = 0; page < Monitor_PageNumber; page++) {
          for(u8 length = 0; length < Monitor_PageLength; length++) {
               SoftIIC_Send(&softiic_config, buffer[page][length]);//从机接收数据
          }
     }
     SoftIIC_End(&softiic_config);//发送结束信号
}

/**
@brief 绘制点
*/
void Monitor_DrawDot(const Monitor_Dot * const feature, const Monitor_Command command) {
     if(Contain((*feature))) {
          if(command.light) {
               buffer[feature->y / 8][feature->x] |= 0x01 << (feature->y % 8);   //点亮该点像素
          } else {
               buffer[feature->y / 8][feature->x] &= ~(0x01 << (feature->y % 8));//熄灭该点像素
          }
     }
}

/**
@brief 绘制直线
*/
void Monitor_DrawLine(const Monitor_Line * const feature, const Monitor_Command command) {
     Monitor_Dot head, tail, transfer;
     u8 mode = 1;              //默认为捺模式
     float slope, slopeTo, gap;//斜率，目标斜率，差距
     
     /* 识别撇和捺并规范数据 */
     if(feature->vertex1.y > feature->vertex2.y) {
          head = feature->vertex2;
          tail = feature->vertex1;
     } else {
          head = feature->vertex1;
          tail = feature->vertex2;
     }
     if(head.x > tail.x) {
          mode = 0;//撇模式
     }
     
     /* 设置像素坐标的起点和终点并记录直线斜率 */
     transfer = head;
     if(!(transfer.x == tail.x || transfer.y == tail.y)) {
          slopeTo = (float)(tail.y - transfer.y) / (float)(tail.x - transfer.x);
     }
     
     //还需要限制范围不能超出屏幕
     if(mode) {
          while(transfer.x != tail.x || transfer.y != tail.y) {
               Monitor_DrawDot(&transfer, command);
               if(transfer.x == tail.x) {
                    transfer.y++;
               } else if(transfer.y == tail.y) {
                    transfer.x++;
               } else {
                    slope = (float)(tail.y - transfer.y) / (float)(tail.x - transfer.x);
                    gap = slopeTo - slope;
                    if(gap < -LineAccuracy) {
                         transfer.y++;
                    } else if(gap > LineAccuracy) {
                         transfer.x++;
                    } else {
                         transfer.x++;
                         transfer.y++;
                    }
               }
          }
     } else {
          while(transfer.x != tail.x || transfer.y != tail.y) {
               Monitor_DrawDot(&transfer, command);
               if(transfer.x == tail.x) {
                    transfer.y++;
               } else if(transfer.y == tail.y) {
                    transfer.x--;
               } else {
                    slope = (float)(tail.y - transfer.y) / (float)(tail.x - transfer.x);
                    gap = slopeTo - slope;
                    if(gap > LineAccuracy) {
                         transfer.y++;
                    } else if(gap < -LineAccuracy) {
                         transfer.x--;
                    } else {
                         transfer.x--;
                         transfer.y++;
                    }
               }
          }
     }
     Monitor_DrawDot(&tail, command);
}

/**
@brief 绘制折线
*/
void Monitor_DrawFoldLine(const Monitor_FoldLine * const feature, const Monitor_Command command) {
     Monitor_Line line;
     for(u8 times = 0; times < feature->number - 1; times++) {
          line.vertex1 = (*feature->vertexs)[times];
          line.vertex2 = (*feature->vertexs)[times + 1];
          Monitor_DrawLine(&line, command);
     }
}

/**
@brief 绘制矩形
*/
void Monitor_DrawRectangle(const Monitor_Rectangle_T * const feature, const Monitor_Command command) {
     if(command.fill) {
          Monitor_Dot dot;
          for(u8 row = 0; row < feature->size.w; row++) {
               for(u8 column = 0; column < feature->size.h; column++) {
                    dot.x = feature->corner.x + row;
                    dot.y = feature->corner.y + column;
                    Monitor_DrawDot(&dot, command);
               }
          }
     } else {
          Monitor_Dot vertexs[4] = {
               feature->corner,
               {feature->corner.x + feature->size.w - 1, feature->corner.y},
               {feature->corner.x + feature->size.w - 1, feature->corner.y + feature->size.h - 1},
               {feature->corner.x, feature->corner.y + feature->size.h - 1}
          };
          Monitor_Polygon rectangle = {&vertexs, 4};
          Monitor_DrawPolygon(&rectangle, command);
     }
}

/**
@brief 绘制多边形
*/
void Monitor_DrawPolygon(const Monitor_Polygon * const feature, const Monitor_Command command) {
     Monitor_Line line = {(*feature->vertexs)[0], (*feature->vertexs)[feature->number - 1]};
     Monitor_DrawFoldLine(feature, command);
     Monitor_DrawLine(&line, command);
}

/**
@brief 绘制正圆
*/
void Monitor_DrawCircle(const Monitor_Circle * const feature, const Monitor_Command command) {
     Monitor_Dot dot;
     s16 dX = 0, dY = 0 - feature->radius;
     if(dY > 0) {
          return;
     } else if(dY == 0) {
          dot = feature->center;
          Monitor_DrawDot(&dot, command);
     } else if(dY == -1) {
          dot = feature->center;
          dot.y--;
          Monitor_DrawDot(&dot, command);
          
          dot = feature->center;
          dot.x++;
          Monitor_DrawDot(&dot, command);
          
          dot = feature->center;
          dot.y++;
          Monitor_DrawDot(&dot, command);
          
          dot = feature->center;
          dot.x--;
          Monitor_DrawDot(&dot, command);
     } else {
          s16 x, y;
          double dXSquare, stdY, rSquare = pow(feature->radius, 2);
          do {
               dot.x = x = feature->center.x + dX;
               dot.y = y = feature->center.y + dY;
               Monitor_DrawDot(&dot, command);
               
               dot.x = feature->center.x - dY;
               dot.y = feature->center.y - dX;
               Monitor_DrawDot(&dot, command);
               
               dot.x = feature->center.x - dY;
               dot.y = feature->center.y + dX;
               Monitor_DrawDot(&dot, command);
               
               dot.x = feature->center.x + dX;
               dot.y = feature->center.y - dY;
               Monitor_DrawDot(&dot, command);
               
               dot.x = feature->center.x - dX;
               dot.y = feature->center.y - dY;
               Monitor_DrawDot(&dot, command);
               
               dot.x = feature->center.x + dY;
               dot.y = feature->center.y + dX;
               Monitor_DrawDot(&dot, command);
               
               dot.x = feature->center.x + dY;
               dot.y = feature->center.y - dX;
               Monitor_DrawDot(&dot, command);
               
               dot.x = feature->center.x - dX;
               dot.y = feature->center.y + dY;
               Monitor_DrawDot(&dot, command);
               
               dXSquare = pow(dX + 1, 2);
               stdY = sqrt(rSquare - dXSquare);
               if(stdY + dY < -0.5) {dY++;}
               dX++;
          } while((float)(x - feature->center.x) / (float)(y - feature->center.y) > -1.0F);
     }
     if(command.fill) {
          static Monitor_Circle circle;
          circle = *feature;
          circle.radius--;
          Monitor_DrawCircle(&circle, command);
     }
}

/**
@brief 绘制符号
*/
void Monitor_DrawSymbol(const Monitor_Symbol * const feature, const Monitor_Command command) {
     for(u8 order = 0; order < SymbolColumn; order++) {
          for(s8 bit = 0; bit < SymbolHeight; bit++) {
               Monitor_Dot dot = {feature->corner.x + order, feature->corner.y + bit};
               if((symbols[feature->symbol - SymbolOffset][order] & (0x0001 << bit)) == (0x0001 << bit)) {
                    Monitor_DrawDot(&dot, command);
               }
          }
     }
}
