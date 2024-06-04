/*
 * Copyright (C) 2022-2023 ArtinChip Technology Co., Ltd.
 * Authors:  Ning Fang <ning.fang@artinchip.com>
 */

#include "lvgl.h"
#include "aic_ui.h"
#include "aic_osal.h"

void aic_ui_init()
{
/* qc test demo is only for aic internal qc testing, please ignore it. */

#ifdef LPKG_USING_LVGL_VSCODE
    extern void vscode_ui_init();
    vscode_ui_init();
    return;
#endif

#ifdef AIC_LVGL_BASE_DEMO
#include "base_ui.h"
    base_ui_init();
#endif

#ifdef AIC_LVGL_METER_DEMO
#include "meter_ui.h"
    meter_ui_init();
#endif

#ifdef AIC_LVGL_LAUNCHER_DEMO
    extern void launcher_ui_init();
    launcher_ui_init();
#endif

#ifdef AIC_LVGL_MUSIC_DEMO
    extern void lv_demo_music(void);
    lv_demo_music();
#endif

#ifdef AIC_LVGL_DASHBOARD_DEMO
    extern void dashboard_ui_init(void);
    dashboard_ui_init();
#endif

#ifdef AIC_LVGL_SHOWCASE_DEMO
    extern void showcase_demo_init(void);
    showcase_demo_init();
#endif

#ifdef AIC_LVGL_ELEVATOR_DEMO
#include "elevator_ui.h"
    elevator_ui_init();
#endif

#ifdef AIC_LVGL_SLIDE_DEMO
    extern void slide_ui_init(void);
    slide_ui_init();
#endif

#ifdef AIC_LVGL_SIMPLE_PLAYER_DEMO
    extern void simple_player_ui_init(void);
    simple_player_ui_init();
#endif

    return;
}
