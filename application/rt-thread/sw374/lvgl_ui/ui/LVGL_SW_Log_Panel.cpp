#include "LVGL_SW_Log_Panel.h"
#define Log_box_height 29
#define Log_box_width 790
#define Log_begin_position_y Log_box_height + 8
#define Log_begin_position_x 5

LVGL_SW_Log_Panel::LVGL_SW_Log_Panel() : 
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
    // Init_LVGL_SW_text_style();
    for (int i = 0; i < 16 ;i++)
    {
        this->logText[i].setFont(&F1_Display_Regular_24px_4bpp);
        this->logText[i].setTextalignLeft();
        this->logMessages[i] = new char[MAX_LOG_MESSAGE_LENGTH];
        this->logMessages[i][0] = '\0'; // 初始化为空字符串
    }
}

LVGL_SW_Log_Panel::~LVGL_SW_Log_Panel() {
    for (int i = 0; i < 16; i++) {
        delete[] this->logMessages[i];
    }
}

void LVGL_SW_Log_Panel::updateLogDisplay(char * logmessage) {
    if (this->logIndex < 16) {
        // 总输出 16 行以下时，接续在下方输出
        strncpy(this->logMessages[logIndex], logmessage, MAX_LOG_MESSAGE_LENGTH - 1);
        this->logMessages[logIndex][MAX_LOG_MESSAGE_LENGTH - 1] = '\0'; // 确保字符串以空字符结尾
        this->logText[this->logIndex].setLabelText(this->logMessages[this->logIndex]);
        this->logIndex++;
    } else {
        // 16 行以上时，全部上移一行，在最后一行输出 log
        for (int i = 0; i < 15; i++) {
            strcpy(this->logMessages[i], this->logMessages[i + 1]);
            this->logText[i].setLabelText(this->logMessages[i]);
        }
        strncpy(this->logMessages[15], logmessage, MAX_LOG_MESSAGE_LENGTH - 1);
        this->logMessages[15][MAX_LOG_MESSAGE_LENGTH - 1] = '\0'; // 确保字符串以空字符结尾
        this->logText[15].setLabelText(this->logMessages[15]);
    }
}
