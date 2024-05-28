/**
@brief 图形界面库
@version 3.2
*/

#include "Gui.h"

/**
@brief 渲染图形
*/
void Gui_Execute(const Gui_Component * const component) {
     static Monitor_Line line;
     static Monitor_Rectangle_T rectangle;
     static Monitor_Circle circle;
     static Monitor_Symbol symbol;
     static Monitor_Command command;
     
     switch(component->type) {
     case Line:
          line.vertex2 = line.vertex1 = component->corner.current;
          line.vertex2.x += component->size.current.w;
          line.vertex2.y += component->size.current.h;
          command.light = component->style.light;
          Monitor_DrawLine(&line, command);
          break;
     case Rectangle:
          rectangle.corner = component->corner.current;
          rectangle.size = component->size.current;
          command.light = component->style.light;
          command.fill = component->style.fill;
          Monitor_DrawRectangle(&rectangle, command);
          break;
     case Circle:
          circle.center.x = component->corner.current.x + component->size.current.w / 2;
          circle.center.y = component->corner.current.y + component->size.current.h / 2;
          circle.radius = component->size.current.w / 2;
          command.light = component->style.light;
          command.fill = component->style.fill;
          Monitor_DrawCircle(&circle, command);
          break;
     case Capsule:
          if(component->style.vertical) {
               circle.radius = component->size.current.w / 2;
               circle.center.x = component->corner.current.x + circle.radius;
               circle.center.y = component->corner.current.y + circle.radius;
               command.light = component->style.light;
               command.fill = component->style.fill;
               Monitor_DrawCircle(&circle, command);
               rectangle.corner.x = component->corner.current.x;
               rectangle.corner.y = circle.center.y;
               rectangle.size.w = component->size.current.w;
               rectangle.size.h = component->size.current.h - component->size.current.w;
               command.light = component->style.light;
               command.fill = component->style.fill;
               Monitor_DrawRectangle(&rectangle, command);
               circle.center.y = component->corner.current.y + component->size.current.h - circle.radius;
               Monitor_DrawCircle(&circle, command);
               
          } else {
               circle.radius = component->size.current.h / 2;
               circle.center.x = component->corner.current.x + circle.radius;
               circle.center.y = component->corner.current.y + circle.radius;
               command.light = component->style.light;
               command.fill = component->style.fill;
               Monitor_DrawCircle(&circle, command);
               rectangle.corner.x = circle.center.x;
               rectangle.corner.y = component->corner.current.y;
               rectangle.size.w = component->size.current.w - component->size.current.h;
               rectangle.size.h = component->size.current.h;
               command.light = component->style.light;
               command.fill = component->style.fill;
               Monitor_DrawRectangle(&rectangle, command);
               circle.center.x = component->corner.current.x + component->size.current.w - circle.radius;
               Monitor_DrawCircle(&circle, command);
          }
          break;
     case Text:
          symbol.corner.y = component->corner.current.y;
          for(u8 index = 0; (component->text)[index] != '\0'; index++) {
               symbol.corner.x = component->corner.current.x + index * (component->style.interval + SymbolColumn);
               symbol.symbol = (component->text)[index];
               command.light = component->style.light;
               Monitor_DrawSymbol(&symbol, command);
          }
          break;
     default:
          break;
     }
     
     if(component->branch) {
          for(u8 index = 0; (*(component->branch))[index] != 0; index++) {
               Gui_Component target = *(*(component->branch))[index];
               target.corner.current.x += component->corner.current.x;
               target.corner.current.y += component->corner.current.y;
               target.corner.target.x += component->corner.target.x;
               target.corner.target.y += component->corner.target.y;
               Gui_Execute(&target);
          }
     }
}
