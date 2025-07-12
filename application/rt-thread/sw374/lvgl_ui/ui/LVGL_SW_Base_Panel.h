#ifndef _LVGL_SW_BASE_PANEL_H_
#define _LVGL_SW_BASE_PANEL_H_

#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>

#include "lvgl/lvgl.h"

#include "LVGL_SW_box.h"
#include "LVGL_SW_text.h"
#include "LVGL_SW_data.h"
#include "SW_conf.h"


class LVGL_SW_Base_Panel {
protected:
    lv_obj_t* mainscr; // LVGL基础屏幕对象
    lv_timer_t* update_timer;
    SW_DATA_TYPE::LVGL_SW_Pannel_Data current_data;
public:
    LVGL_SW_Base_Panel();
    virtual ~LVGL_SW_Base_Panel();
    void setCurrentData(SW_DATA_TYPE::LVGL_SW_Pannel_Data data);
    static void update_timer_callback(lv_timer_t* timer);
    virtual void TimerUpdate(void);
};


#endif  // _LVGL_SW_BASE_PANEL_H_
