#include "LVGL_SW_Base_Panel.h"


LVGL_SW_Base_Panel::LVGL_SW_Base_Panel() : mainscr(lv_scr_act())
{
    update_timer = lv_timer_create(update_timer_callback, LVGL_SW_TIMER_PERIOD, this);
}


LVGL_SW_Base_Panel::~LVGL_SW_Base_Panel()
{
    if (update_timer) {
        lv_timer_del(update_timer);
        update_timer = nullptr;
    }
}

void LVGL_SW_Base_Panel::setCurrentData(SW_DATA_TYPE::LVGL_SW_Pannel_Data data) {
    this->current_data = data;
}

void LVGL_SW_Base_Panel::update_timer_callback(lv_timer_t* timer) {
    // 获取类实例指针
    LVGL_SW_Base_Panel* instance = static_cast<LVGL_SW_Base_Panel*>(lv_timer_get_user_data(timer));
    if (instance) {
        // 使用current_data更新屏幕
        instance->TimerUpdate();
    }
}




void LVGL_SW_Base_Panel::TimerUpdate(void)
{
    LV_LOG_USER("You Pannel needs a new Function UpdateAllData!");
}

