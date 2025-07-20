#ifndef _LVGL_SW_INIT_Pannel_H_
#define _LVGL_SW_INIT_Pannel_H_


#include "lvgl/lvgl.h"
#include "LVGL_SW_box.h"
#include "LVGL_SW_text.h"

#include <string>

#include "SW_conf.h"

#define MAX_LOG_MESSAGE_LENGTH 128

class LVGL_SW_Log_Panel {

private:
    lv_obj_t* mainscr; // LVGL基础屏幕对象
    int logIndex;
    LVGL_SW_box box;  // 外显示框
    LVGL_SW_text logText[16];
    std::string logMessages[16];;  // 用于保存日志信息

public:
    LVGL_SW_Log_Panel();
    ~LVGL_SW_Log_Panel();
    void updateLogDisplay(std::string logmessage);
};

#endif //_LVGL_SW_INIT_Pannel_H_