#ifndef _LVGL_SW_ARC_H_
#define _LVGL_SW_ARC_H_

#include <string>
#include "lvgl.h"



class LVGL_SW_arc {
protected:
    lv_obj_t* mainscr; // LVGL基础屏幕对象
    double pos_x;  // x坐标
    double pos_y;  // y坐标
    double size_x;  // 宽度
    double size_y;  // 高度
    double width;  // 圆弧宽度
    lv_obj_t * arc;  // 圆弧对象

public:
    // 构造函数
    LVGL_SW_arc(double pos_x_Val, double pos_y_Val, double size_x_Val, double size_y_Val, double width, lv_obj_t* mainscr_Val);
    // 设置坐标
    void setPosition(double newX, double newY);
    void setSize(double newWidth, double newHeight);
    void setWidth(double newArcWidth);
    // 设置颜色
    void setArcColor(lv_color_t lvcolor);
    void setArcColor(uint32_t RGBcolor);
    void setArcOpa(lv_opa_t opa);
};



#endif // _LVGL_SW_ARC_H_
