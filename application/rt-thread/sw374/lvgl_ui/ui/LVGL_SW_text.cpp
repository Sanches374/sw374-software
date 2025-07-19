#include "LVGL_SW_text.h"


lv_style_t LVGL_SW_text_style;
lv_style_t LVGL_SW_windows_style;
// 初始化样式
void Init_LVGL_SW_text_style() {
    // 初始化文本样式
    lv_style_init(&LVGL_SW_text_style);
    lv_style_set_border_width(&LVGL_SW_text_style, 0);
    lv_style_set_bg_opa(&LVGL_SW_text_style, LV_OPA_TRANSP);
    lv_style_set_pad_all(&LVGL_SW_text_style, 0);  // 无内边距
}


void Init_LVGL_SW_windows_style()
{
    // 初始化弹出框样式
    lv_style_init(&LVGL_SW_windows_style);
    lv_style_set_border_width(&LVGL_SW_windows_style, 0);
    lv_style_set_bg_opa(&LVGL_SW_windows_style, LV_OPA_TRANSP);
    lv_style_set_pad_all(&LVGL_SW_windows_style, 0);  // 无内边距
    lv_style_set_border_color(&LVGL_SW_windows_style, lv_color_hex(0xFFFFFF));
    lv_style_set_border_opa(&LVGL_SW_windows_style, LV_OPA_COVER);
    lv_style_set_bg_opa(&LVGL_SW_windows_style, LV_OPA_COVER);
    lv_style_set_radius(&LVGL_SW_windows_style, 0);
    lv_style_set_pad_all(&LVGL_SW_windows_style, 0);  // 无内边距
}

LVGL_SW_text::LVGL_SW_text(double pos_x_Val = 0.0, double pos_y_Val = 0.0, double size_x_Val = 100.0, double size_y_Val = 100.0, lv_obj_t* mainscr_Val = lv_scr_act())
    : LVGL_SW_box(pos_x_Val, pos_y_Val, size_x_Val, size_y_Val, mainscr_Val), label(lv_label_create(rect))
{
    this->pstyle = &LVGL_SW_text_style;
    // 应用样式到矩形对象
    lv_obj_add_style(rect, this->pstyle, 0);

    // 初始化标签文本为空
    lv_label_set_text(label, "");
    // 将标签相对于父对象(矩形)居中对齐
    lv_obj_set_align(label, LV_ALIGN_CENTER);

    // 启用文本自动换行
    lv_label_set_long_mode(label, LV_LABEL_LONG_CLIP);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0); // 设置文本颜色为白色
    lv_obj_set_style_text_font(label, &MAIN_FONT, 0);
}

void LVGL_SW_text::setLabelText(std::string str)
{
    // 设置标签显示的文本内容
    lv_label_set_text(label, str.c_str());
}

void LVGL_SW_text::setLabelText(char* str)
{
    // 设置标签显示的文本内容
    lv_label_set_text(label, str);
}

void LVGL_SW_text::setFont(const lv_font_t* font_Val)
{
    // 设置标签字体
    lv_obj_set_style_text_font(label, font_Val, 0);
}

void LVGL_SW_text::setTextColor(lv_color_t lvcolor)
{
    lv_obj_set_style_text_color(label, lvcolor, 0);
}

void LVGL_SW_text::setTextColor(uint32_t RGBcolor)
{
    lv_obj_set_style_text_color(label, lv_color_hex(RGBcolor), 0);
}

void LVGL_SW_text::setTextalignLeft(void)
{
    lv_obj_set_align(label, LV_ALIGN_LEFT_MID);
}