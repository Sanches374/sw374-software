#include "LVGL_SW_arc.h"



LVGL_SW_arc::LVGL_SW_arc(double pos_x_Val = 0.0, double pos_y_Val = 0.0, double size_x_Val = 100.0, double size_y_Val = 100.0, double width_Val = 5.0, lv_obj_t* mainscr_Val = lv_scr_act())
:
mainscr(mainscr_Val), pos_x(pos_x_Val), pos_y(pos_y_Val), size_x(size_x_Val), size_y(size_y_Val), width(width_Val), arc(lv_arc_create(mainscr))
{
    lv_obj_set_pos(this->arc, this->pos_x, this->pos_y);
    lv_obj_set_size(this->arc, this->size_x, this->size_y);
    lv_obj_set_style_arc_width(this->arc, this->width, LV_PART_INDICATOR); // 设置当前轮胎类型图标宽度为5
    lv_obj_remove_style(this->arc, NULL, LV_PART_KNOB);   /*Be sure the knob is not displayed*/
    lv_obj_remove_flag(this->arc, LV_OBJ_FLAG_CLICKABLE);  /*To not allow adjusting by click*/
    lv_arc_set_bg_angles(this->arc, 0, 360);
    lv_arc_set_range(this->arc, 0, 100);
    lv_arc_set_value(this->arc, 100);
    lv_obj_set_style_arc_opa(this->arc, LV_OPA_TRANSP, LV_PART_MAIN); // 设置当前轮胎类型图标透明度为0
}
// 设置坐标
void LVGL_SW_arc::setPosition(double newX, double newY)
{
    this->pos_x = newX;
    this->pos_y = newY;
    lv_obj_set_pos(this->arc, this->pos_x, this->pos_y);
}
void LVGL_SW_arc::setSize(double newWidth, double newHeight)
{
    this->size_x = newWidth;
    this->size_y = newHeight;
    lv_obj_set_size(this->arc, this->size_x, this->size_y);
}
void LVGL_SW_arc::setWidth(double newArcWidth)
{
    this->width = newArcWidth;
    lv_obj_set_style_arc_width(this->arc, this->width, LV_PART_INDICATOR); // 设置当前轮胎类型图标宽度为6
}
// 设置颜色
void LVGL_SW_arc::setArcColor(lv_color_t lvcolor)
{
    lv_obj_set_style_arc_color(this->arc, lvcolor, LV_PART_INDICATOR);
}
void LVGL_SW_arc::setArcColor(uint32_t RGBcolor)
{
    lv_obj_set_style_arc_color(this->arc, lv_color_hex(RGBcolor), LV_PART_INDICATOR);
}

void LVGL_SW_arc::setArcOpa(lv_opa_t opa)
{
    lv_obj_set_style_arc_opa(this->arc, opa, LV_PART_INDICATOR);
}