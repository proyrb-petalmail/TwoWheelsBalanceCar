/**
@brief 图形界面库
@version 3.2
*/

#ifndef _Gui_
#define _Gui_

#include "Monitor.h"

/**
@brief 坐标属性
*/
typedef struct {
     Monitor_Dot current;//当前坐标
     Monitor_Dot target; //目标坐标
} Gui_Coord;

/**
@brief 尺寸属性
*/
typedef struct {
     Monitor_Size current;//当前尺寸
     Monitor_Size target; //目标尺寸
} Gui_Size;

/**
@brief 样式属性
*/
typedef struct {
     unsigned light    : 1;//点亮
     unsigned fill     : 1;//填充
     unsigned hide     : 1;//隐藏
     unsigned vertical : 1;//竖直
     unsigned interval : 4;//间隔
} Gui_Style;

/**
@brief 组件类型
*/
typedef enum {
     Line = 0,
     Rectangle,
     Circle,
     Capsule,
     Text,
} Gui_Type;

/**
@brief 组件
*/
typedef struct Gui_Component_t {
     Gui_Coord corner;                    //左上角
     Gui_Size  size;                      //尺寸
     struct Gui_Component_t * (*branch)[];//分支
     char text[14];                       //文本
     Gui_Style style;                     //样式
     Gui_Type    type;                    //组件类型
} Gui_Component;

extern void Gui_Execute(const Gui_Component * const component);

#endif /* _Gui_ */
