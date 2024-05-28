/**
@brief 显示屏模块
@version 3.0
*/

#ifndef _Monitor_
#define _Monitor_

#include "SoftIIC.h"

#define SymbolColumn 8 //字符像素宽度

/**
@brief 像素点参数
*/
typedef struct {
     s16 x;//横坐标
     s16 y;//纵坐标
} Monitor_Dot;

/**
@brief 尺寸大小参数
*/
typedef struct {
     s16 w;//宽度
     s16 h;//高度
} Monitor_Size;

/**
@brief 直线绘制参数
*/
typedef struct {
     Monitor_Dot vertex1;
     Monitor_Dot vertex2;
} Monitor_Line;

/**
@brief 折线绘制参数
*/
typedef struct {
     Monitor_Dot (*vertexs)[];//顶点数组
     u8 number;               //数组元素个数
} Monitor_FoldLine;

/**
@brief 矩形绘制参数
*/
typedef struct Monitor_Rectangle {
     Monitor_Dot  corner;//左上角
     Monitor_Size size;  //尺寸
} Monitor_Rectangle_T;

/**
@brief 多边形绘制参数
*/
typedef Monitor_FoldLine Monitor_Polygon;

/**
@brief 正圆绘制参数
*/
typedef struct {
     Monitor_Dot center;//圆心
     s16 radius;        //半径
} Monitor_Circle;

/**
@brief 符号绘制参数
*/
typedef struct {
     Monitor_Dot corner;//左上角
     s8 symbol;         //符号
} Monitor_Symbol;

/**
@brief 指令参数
*/
typedef struct {
     unsigned light : 1;//是否点亮
     unsigned fill  : 1;//是否填充
} Monitor_Command;

extern void Monitor_Initialize(void);
extern void Monitor_DumpBuffer(void);
extern void Monitor_LoadBuffer(void);
extern void Monitor_DrawDot(const Monitor_Dot * const feature, const Monitor_Command command);
extern void Monitor_DrawLine(const Monitor_Line * const feature, const Monitor_Command command);
extern void Monitor_DrawFoldLine(const Monitor_FoldLine * const feature, const Monitor_Command command);
extern void Monitor_DrawRectangle(const Monitor_Rectangle_T * const feature, const Monitor_Command command);
extern void Monitor_DrawPolygon(const Monitor_Polygon * const feature, const Monitor_Command command);
extern void Monitor_DrawCircle(const Monitor_Circle * const feature, const Monitor_Command command);
extern void Monitor_DrawSymbol(const Monitor_Symbol * const feature, const Monitor_Command command);

#endif /* _Monitor_ */
