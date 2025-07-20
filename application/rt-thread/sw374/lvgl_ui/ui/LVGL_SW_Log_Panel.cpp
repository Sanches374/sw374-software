#include "LVGL_SW_Log_Panel.h"
#define Log_box_height 29
#define Log_box_width 790
#define Log_begin_position_y Log_box_height + 8
#define Log_begin_position_x 5

LVGL_SW_Log_Panel::LVGL_SW_Log_Panel()
 : 
mainscr(lv_scr_act()),
logIndex(0),
box(2, 2, 796, 476, this->mainscr),
logText{
    LVGL_SW_text(Log_begin_position_x, 0 * Log_begin_position_y, Log_box_width, Log_box_height, this->mainscr),
    LVGL_SW_text(Log_begin_position_x, 1 * Log_begin_position_y, Log_box_width, Log_box_height, this->mainscr),
    LVGL_SW_text(Log_begin_position_x, 2 * Log_begin_position_y, Log_box_width, Log_box_height, this->mainscr),
    LVGL_SW_text(Log_begin_position_x, 3 * Log_begin_position_y, Log_box_width, Log_box_height, this->mainscr),
    LVGL_SW_text(Log_begin_position_x, 4 * Log_begin_position_y, Log_box_width, Log_box_height, this->mainscr),
    LVGL_SW_text(Log_begin_position_x, 5 * Log_begin_position_y, Log_box_width, Log_box_height, this->mainscr),
    LVGL_SW_text(Log_begin_position_x, 6 * Log_begin_position_y, Log_box_width, Log_box_height, this->mainscr),
    LVGL_SW_text(Log_begin_position_x, 7 * Log_begin_position_y, Log_box_width, Log_box_height, this->mainscr),
    LVGL_SW_text(Log_begin_position_x, 8 * Log_begin_position_y, Log_box_width, Log_box_height, this->mainscr),
    LVGL_SW_text(Log_begin_position_x, 9 * Log_begin_position_y, Log_box_width, Log_box_height, this->mainscr),
    LVGL_SW_text(Log_begin_position_x, 10 * Log_begin_position_y, Log_box_width, Log_box_height, this->mainscr),
    LVGL_SW_text(Log_begin_position_x, 11 * Log_begin_position_y, Log_box_width, Log_box_height, this->mainscr),
    LVGL_SW_text(Log_begin_position_x, 12 * Log_begin_position_y, Log_box_width, Log_box_height, this->mainscr),
    LVGL_SW_text(Log_begin_position_x, 13 * Log_begin_position_y, Log_box_width, Log_box_height, this->mainscr),
    LVGL_SW_text(Log_begin_position_x, 14 * Log_begin_position_y, Log_box_width, Log_box_height, this->mainscr),
    LVGL_SW_text(Log_begin_position_x, 15 * Log_begin_position_y, Log_box_width, Log_box_height, this->mainscr)
}
{
    // Init_LVGL_SW_box_style();
    Init_LVGL_SW_text_style();
    for (int i = 0; i < 16; i++)
    {
        this->logText[i].setFont(&F1_Display_Regular_24px_4bpp);
        this->logText[i].setTextalignLeft();
        // this->logMessages[i] = new char[MAX_LOG_MESSAGE_LENGTH];
        // this->logMessages[i][0] = '\0'; // 初始化为空字符串
    }

}

LVGL_SW_Log_Panel::~LVGL_SW_Log_Panel() {
    // for (int i = 0; i < 16; i++) {
    //     delete[] this->logMessages[i];
    // }
}

void LVGL_SW_Log_Panel::updateLogDisplay(std::string logmessage) {
    if (logIndex < 16) 
    {
        logMessages[logIndex] = logmessage;
        logText[logIndex].setLabelText(logmessage);
        logIndex++;
    } 
    else 
    {
        for (int i = 0; i < 15; i++) {
            logMessages[i] = logMessages[i + 1];
            logText[i].setLabelText(logMessages[i]);
        }
        logMessages[15] = logmessage;
        logText[15].setLabelText(logmessage);
    }
}
