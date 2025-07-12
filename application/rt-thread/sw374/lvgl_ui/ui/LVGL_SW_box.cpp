#include "LVGL_SW_box.h"


lv_style_t LVGL_SW_box_style;

void Init_LVGL_SW_box_style() {
    // 初始化矩形样式
    lv_style_init(&LVGL_SW_box_style);
    lv_style_set_border_width(&LVGL_SW_box_style, 2);
    lv_style_set_border_color(&LVGL_SW_box_style, lv_color_white());
    lv_style_set_border_opa(&LVGL_SW_box_style, LV_OPA_COVER);
    lv_style_set_bg_opa(&LVGL_SW_box_style, LV_OPA_COVER);
    lv_style_set_radius(&LVGL_SW_box_style, 0);
    lv_style_set_pad_all(&LVGL_SW_box_style, 0);  // 无内边距
   
}

LVGL_SW_box::LVGL_SW_box(double pos_x_Val = 0.0, double pos_y_Val = 0.0, double size_x_Val = 100.0, double size_y_Val = 100.0, lv_obj_t* mainscr_Val = lv_scr_act())
:
mainscr(mainscr_Val), pos_x(pos_x_Val), pos_y(pos_y_Val), size_x(size_x_Val), size_y(size_y_Val),rect(lv_obj_create(mainscr))
{
    this->pstyle = &LVGL_SW_box_style;
    // 应用样式到矩形对象
    lv_obj_add_style(rect, this->pstyle, 0);


    // 设置矩形位置
    lv_obj_set_pos(this->rect, this->pos_x, this->pos_y);

    // 设置矩形尺寸
    lv_obj_set_size(this->rect, this->size_x, this->size_y);
    
    // 设置背景颜色为黑色
    lv_obj_set_style_bg_color(rect, lv_color_hex(0x000000), 0); 
}

void LVGL_SW_box::setPosition(double newX, double newY)
{
    this->pos_x = newX;
    this->pos_y = newY;
    lv_obj_set_pos(this->rect, this->pos_x, this->pos_y);
}

void LVGL_SW_box::setSize(double newWidth, double newHeight)
{
    size_x = newWidth;
    size_y = newHeight;
    lv_obj_set_size(this->rect, this->size_x, this->size_y);
}

void LVGL_SW_box::setStyle(lv_style_t& style)
{
    this->pstyle = &style;
    lv_obj_add_style(rect, this->pstyle, 0);
}


void LVGL_SW_box::setBoxColor(lv_color_t lvcolor)
{
   lv_obj_set_style_bg_color(rect, lvcolor, 0);
}

void LVGL_SW_box::setBoxColor(uint32_t RGBcolor)
{
   lv_obj_set_style_bg_color(rect, lv_color_hex(RGBcolor), 0);
}