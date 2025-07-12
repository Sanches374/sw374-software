#ifndef _LVGL_SW_TEXT_H_
#define _LVGL_SW_TEXT_H_

#include <string>
#include "lvgl.h"
#include "LVGL_SW_box.h"
#include "SW_conf.h"

// 激活字体
LV_FONT_DECLARE(F1_Display_Regular_6px_4bpp)
LV_FONT_DECLARE(F1_Display_Regular_12px_4bpp)
LV_FONT_DECLARE(F1_Display_Regular_18px_4bpp)
LV_FONT_DECLARE(F1_Display_Regular_24px_4bpp)
LV_FONT_DECLARE(F1_Display_Regular_36px_4bpp)
LV_FONT_DECLARE(F1_Display_Regular_48px_4bpp)
LV_FONT_DECLARE(F1_Display_Regular_64px_4bpp)
LV_FONT_DECLARE(F1_Display_Regular_72px_4bpp)
LV_FONT_DECLARE(F1_Display_Regular_128px_4bpp)
LV_FONT_DECLARE(F1_Display_Regular_180px_4bpp)
LV_FONT_DECLARE(F1_Display_Regular_240px_4bpp)
#define MAIN_FONT F1_Display_Regular_48px_4bpp

extern lv_style_t LVGL_SW_text_style;  // 文本样式
extern lv_style_t LVGL_SW_windows_style;  // 文本样式
void Init_LVGL_SW_text_style(void);
void Init_LVGL_SW_windows_style(void);
class LVGL_SW_text : public LVGL_SW_box {
private:
    lv_obj_t * label;    // 标签对象，用于显示文本

public:
    LVGL_SW_text(double pos_x_Val, double pos_y_Val, double size_x_Val, double size_y_Val, lv_obj_t* mainscr_Val);
    // 设置标签文本
    void setLabelText(std::string str);
    void setLabelText(char* str);
    // 设置字体
    void setFont(const lv_font_t*);
    // 设置颜色
    void setTextColor(lv_color_t lvcolor);
    void setTextColor(uint32_t RGBcolor);
};



#endif // _LVGL_SW_TEXT_H_

