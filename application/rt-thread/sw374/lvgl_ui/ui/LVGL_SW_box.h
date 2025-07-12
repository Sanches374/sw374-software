#ifndef _LVGL_SW_BOX_H_
#define _LVGL_SW_BOX_H_

#include <string>
#include "lvgl.h"

extern lv_style_t LVGL_SW_box_style;  // 矩形样式
void Init_LVGL_SW_box_style(void);
class LVGL_SW_box {
protected:
    lv_obj_t* mainscr; // LVGL基础屏幕对象
    double pos_x;  // x坐标
    double pos_y;  // y坐标
    double size_x;  // 宽度
    double size_y;  // 高度
    lv_obj_t * rect;  // 矩形对象
    lv_style_t * pstyle;  // 指向矩形样式的指针

public:
    // 构造函数
    LVGL_SW_box(double pos_x_Val, double pos_y_Val, double size_x_Val, double size_y_Val, lv_obj_t* mainscr_Val);
    // 设置坐标
    void setPosition(double newX, double newY);
    void setSize(double newWidth, double newHeight);
    void setStyle(lv_style_t& style);
    // 设置颜色
    void setBoxColor(lv_color_t lvcolor);
    void setBoxColor(uint32_t RGBcolor);
};



#endif // _LVGL_SW_BOX_H_