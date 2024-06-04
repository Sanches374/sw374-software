/*
 * Copyright (c) 2023-2024, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors:  ArtInChip
 */

#include "navigator_ui.h"
#include "showcase_demo.h"
#include "icon_ui.h"
#include "footer_ui.h"
#include "com_label_ui.h"
#include "sys_met_hub.h"

#include "aic_core.h"

#define DRAW_DOWN           1
// #define MONITOR_PERFORMANCE 1

#define DRAW_DOWN_MENU_DOWN 0
#define DRAW_DOWN_MENU_UP   1

typedef enum {
    DISP_SMALL,
    DISP_MEDIUM,
    DISP_LARGE,
} disp_size_t;

typedef struct _icon_app_list {
    const char *icon_path;
    void (*event_cb)(lv_event_t * e);
} icon_app_list;

static disp_size_t disp_size;
static sys_met_hub *sys_info;

extern lv_obj_t* lv_demo_keypad_encoder(void);
extern lv_obj_t *lv_demo_stress(void);
extern lv_obj_t *lv_demo_benchmark(void);
extern lv_obj_t *widgets_ui_init(void);
extern lv_obj_t *gif_ui_init(void);
extern lv_obj_t *layout_list_ui_init(void);
extern lv_obj_t *layout_table_ui_init(void);
extern lv_obj_t *cookbook_ui_init(void);
extern lv_obj_t *dashboard_ui_init(void);
extern lv_obj_t *elevator_ui_init(void);
extern lv_obj_t *aic_video_ui_init(void);
extern lv_obj_t *aic_audio_ui_init(void);
extern lv_obj_t *camera_ui_init(void);
extern lv_obj_t *coffee_ui_init(void);

extern char cur_screen[64];

static void obj_anim_set_y(lv_anim_t* a, int32_t v);
static void obj_anima_set_opa(lv_anim_t* a, int32_t v);

static int last_tabview_tab = 0;
#ifdef PROMOTE_APP
static void promote_set_opa(lv_anim_t* a, int32_t v)
{
    lv_obj_t *obj = (lv_obj_t *)a->user_data;
    if (v % 2)
        lv_obj_set_style_opa(obj, LV_OPA_100, 0);
    else
        lv_obj_set_style_opa(obj, LV_OPA_0, 0);
}

static void to_be_written_cb(lv_event_t * e)
{
    lv_anim_t text_opa;
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * to_be_written = lv_event_get_user_data(e);

    lv_anim_init(&text_opa);
    lv_anim_set_time(&text_opa, 3000);
    lv_anim_set_user_data(&text_opa, to_be_written);
    lv_anim_set_custom_exec_cb(&text_opa, promote_set_opa);
    lv_anim_set_values(&text_opa, 7, 0);
    lv_anim_set_path_cb(&text_opa, lv_anim_path_linear);
    lv_anim_set_delay(&text_opa, 0);
    lv_anim_set_deleted_cb(&text_opa, NULL);
    lv_anim_set_playback_time(&text_opa, 0);
    lv_anim_set_playback_delay(&text_opa, 0);
    lv_anim_set_repeat_count(&text_opa, 0);
    lv_anim_set_repeat_delay(&text_opa, 0);
    lv_anim_set_early_apply(&text_opa, false);

    if (code == LV_EVENT_PRESSED) {
        lv_anim_del_all();
        lv_anim_start(&text_opa);
    }
}
#endif

static void lvgl_default_theme_set(lv_obj_t *obj, bool dark)
{
    lv_disp_t *disp = lv_disp_get_default();

    lv_theme_default_init(disp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), dark, LV_FONT_DEFAULT);
    lv_theme_t *theme = lv_theme_default_get();
    lv_theme_apply(obj);

    lv_disp_set_theme(disp, theme);
}

static void showcase_default_theme_set(lv_obj_t *obj, bool dark)
{
    lv_disp_t *disp = lv_disp_get_default();

    lv_theme_default_init(disp, lv_color_hex(0x1A1D26), lv_color_hex(0x272729), dark, LV_FONT_DEFAULT);

    lv_theme_t *theme = lv_theme_default_get();
    lv_theme_apply(obj);

    lv_disp_set_theme(disp, theme);
}

static void keypad_encoder_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_PRESSED) {
        lv_obj_t *obj_keypad_encoder = lv_demo_keypad_encoder();

        layer_sys_ui_visible(0);

        lvgl_default_theme_set(obj_keypad_encoder, false);

        lv_scr_load_anim(obj_keypad_encoder, LV_SCR_LOAD_ANIM_NONE, 0, 5, true);
        strncpy(cur_screen, "keypad_encoder", sizeof(cur_screen));
    }
}

static void stress_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_PRESSED) {
        lv_obj_t *obj_stress = lv_demo_stress();

        layer_sys_ui_visible(0);

        lvgl_default_theme_set(obj_stress, false);

        lv_scr_load_anim(obj_stress, LV_SCR_LOAD_ANIM_NONE, 0, 5, true);
        strncpy(cur_screen, "stress", sizeof(cur_screen));
    }
}

static void benchmark_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_PRESSED) {
        lv_img_cache_invalidate_src(NULL);

        lv_obj_t *obj_benchmark = lv_demo_benchmark();

        layer_sys_ui_visible(0);

        lvgl_default_theme_set(obj_benchmark, false);

        lv_scr_load_anim(obj_benchmark, LV_SCR_LOAD_ANIM_NONE, 0, 5, true);
        strncpy(cur_screen, "benchmark", sizeof(cur_screen));
    }
}

static void widgets_example_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_PRESSED) {
        lv_img_cache_invalidate_src(NULL);

        lv_obj_t *obj_widget_example = widgets_ui_init();

        showcase_default_theme_set(obj_widget_example, false);

        lv_scr_load_anim(obj_widget_example, LV_SCR_LOAD_ANIM_NONE, 0, 5, true);
        strncpy(cur_screen, "widget", sizeof(cur_screen));
    }
}

static void gif_example_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_PRESSED) {
        lv_img_cache_invalidate_src(NULL);

        layer_sys_ui_visible(0);

        lv_obj_t *obj_gif = gif_ui_init();

        showcase_default_theme_set(obj_gif, true);

        lv_scr_load_anim(obj_gif, LV_SCR_LOAD_ANIM_NONE, 0, 5, true);
        strncpy(cur_screen, "gif", sizeof(cur_screen));
    }
}

static void layout_list_ui_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_PRESSED) {
        lv_img_cache_invalidate_src(NULL);

        lv_obj_t *obj_layout = layout_list_ui_init();

        showcase_default_theme_set(obj_layout, true);

        lv_scr_load_anim(obj_layout, LV_SCR_LOAD_ANIM_NONE, 0, 5, true);
        strncpy(cur_screen, "layout_list", sizeof(cur_screen));
    }
}

static void layout_table_ui_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_PRESSED) {
        lv_img_cache_invalidate_src(NULL);

        layer_sys_ui_visible(0);

        lv_obj_t *obj_layout = layout_table_ui_init();

        showcase_default_theme_set(obj_layout, true);

        lv_scr_load_anim(obj_layout, LV_SCR_LOAD_ANIM_NONE, 0, 5, true);
        strncpy(cur_screen, "layout_table", sizeof(cur_screen));
    }
}

static void cookbook_ui_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_PRESSED) {
        lv_img_cache_invalidate_src(NULL);

        layer_sys_ui_visible(0);

        lv_obj_t *obj_cookbook = cookbook_ui_init();

        showcase_default_theme_set(obj_cookbook, true);

        lv_scr_load_anim(obj_cookbook, LV_SCR_LOAD_ANIM_NONE, 0, 5, true);
        strncpy(cur_screen, "cookbook", sizeof(cur_screen));
    }
}

static void dashboard_ui_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_PRESSED) {
        lv_img_cache_invalidate_src(NULL);

        layer_sys_ui_visible(0);

        lv_obj_t *obj_dashboard = dashboard_ui_init();

        showcase_default_theme_set(obj_dashboard, true);

        lv_scr_load_anim(obj_dashboard, LV_SCR_LOAD_ANIM_NONE, 0, 5, true);
        strncpy(cur_screen, "dashboard", sizeof(cur_screen));
    }
}

static void elevator_ui_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_PRESSED) {
        lv_img_cache_invalidate_src(NULL);

        layer_sys_ui_visible(0);

        // lv_obj_clean(lv_scr_act());

        lv_obj_t *obj_elevator = elevator_ui_init();

        showcase_default_theme_set(obj_elevator, true);

        lv_scr_load_anim(obj_elevator, LV_SCR_LOAD_ANIM_NONE, 0, 5, true);
        strncpy(cur_screen, "elevator", sizeof(cur_screen));
    }
}

static void aic_video_ui_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_PRESSED) {
        lv_img_cache_invalidate_src(NULL);

        layer_sys_ui_visible(0);

        lv_obj_t *obj_aic_video = aic_video_ui_init();

        showcase_default_theme_set(obj_aic_video, true);

        lv_scr_load_anim(obj_aic_video, LV_SCR_LOAD_ANIM_NONE, 0, 5, true);
        strncpy(cur_screen, "aic_video", sizeof(cur_screen));
    }
}

static void aic_audio_ui_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_PRESSED) {
        lv_img_cache_invalidate_src(NULL);

        layer_sys_ui_visible(0);

        lv_obj_t *obj_aic_audio = aic_audio_ui_init();

        showcase_default_theme_set(obj_aic_audio, true);

        lv_scr_load_anim(obj_aic_audio, LV_SCR_LOAD_ANIM_NONE, 0, 5, true);
        strncpy(cur_screen, "aic_audio", sizeof(cur_screen));
    }
}

static void camera_ui_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_PRESSED) {
        lv_img_cache_invalidate_src(NULL);

        layer_sys_ui_visible(0);

        lv_obj_t *obj_camera = camera_ui_init();

        showcase_default_theme_set(obj_camera, true);

        lv_scr_load_anim(obj_camera, LV_SCR_LOAD_ANIM_NONE, 0, 5, true);
        strncpy(cur_screen, "camera", sizeof(cur_screen));
    }
}

static void coffee_ui_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_PRESSED) {
        lv_img_cache_invalidate_src(NULL);

        layer_sys_ui_visible(0);

        lv_obj_t *obj_coffee = coffee_ui_init();

        showcase_default_theme_set(obj_coffee, true);

        lv_scr_load_anim(obj_coffee, LV_SCR_LOAD_ANIM_NONE, 0, 5, true);
        strncpy(cur_screen, "coffee", sizeof(cur_screen));
    }
}

static icon_app_list icon_list_480_272[] = {
    {LVGL_PATH(navigator/dashboard.png), dashboard_ui_cb},
    {LVGL_PATH(navigator/elevator_simulation.png), elevator_ui_cb},
    {LVGL_PATH(navigator/video.png), aic_video_ui_cb},
    {LVGL_PATH(navigator/music.png), aic_audio_ui_cb},
    {LVGL_PATH(navigator/stress.png), stress_cb},
    {LVGL_PATH(navigator/benchmark.png), benchmark_cb},
    {LVGL_PATH(navigator/keypad_encoder.png), keypad_encoder_cb},
    {LVGL_PATH(navigator/widgets.png), widgets_example_cb},

    {LVGL_PATH(navigator/cookbook.png), cookbook_ui_cb},
    {LVGL_PATH(navigator/gif.png), gif_example_cb},
    {LVGL_PATH(navigator/list.png), layout_list_ui_cb},
    {LVGL_PATH(navigator/table.png), layout_table_ui_cb},
    {LVGL_PATH(navigator/camera.png), camera_ui_cb},
    {LVGL_PATH(navigator/coffee.png), coffee_ui_cb},
    {LVGL_PATH(navigator/print.png), NULL},
    {LVGL_PATH(navigator/weather.png), NULL},

    {LVGL_PATH(navigator/alarm_clock.png), NULL},
    {LVGL_PATH(navigator/calculator.png), NULL},
    {LVGL_PATH(navigator/camera.png), NULL},
    {LVGL_PATH(navigator/clock.png), NULL},
    {LVGL_PATH(navigator/dashboard.png), NULL},
    {LVGL_PATH(navigator/elevator_simulation.png), NULL},
    {LVGL_PATH(navigator/kitchen_appliances.png), NULL},
    {LVGL_PATH(navigator/mail.png), NULL},
};

static icon_app_list icon_list_1024_600[] = {
    {LVGL_PATH(navigator/dashboard.png), dashboard_ui_cb},
    {LVGL_PATH(navigator/elevator_simulation.png), elevator_ui_cb},
    {LVGL_PATH(navigator/stress.png), stress_cb},
    {LVGL_PATH(navigator/keypad_encoder.png), keypad_encoder_cb},
    {LVGL_PATH(navigator/benchmark.png), benchmark_cb},
    {LVGL_PATH(navigator/coffee.png), coffee_ui_cb},
    {LVGL_PATH(navigator/sound_recording.png), NULL},
    {LVGL_PATH(navigator/clock.png), NULL},

    {LVGL_PATH(navigator/alarm_clock.png), NULL},
    {LVGL_PATH(navigator/calculator.png), NULL},
    {LVGL_PATH(navigator/camera.png), NULL},
    {LVGL_PATH(navigator/clock.png), NULL},
    {LVGL_PATH(navigator/mail.png), NULL},
    {LVGL_PATH(navigator/print.png), NULL},
    {LVGL_PATH(navigator/sound_recording.png), NULL},
    {LVGL_PATH(navigator/clock.png), NULL},

    {LVGL_PATH(navigator/alarm_clock.png), NULL},
    {LVGL_PATH(navigator/calculator.png), NULL},
    {LVGL_PATH(navigator/camera.png), NULL},
    {LVGL_PATH(navigator/clock.png), NULL},
    {LVGL_PATH(navigator/mail.png), NULL},
    {LVGL_PATH(navigator/print.png), NULL},
    {LVGL_PATH(navigator/sound_recording.png), NULL},
    {LVGL_PATH(navigator/clock.png), NULL}
};

static lv_obj_t *footer_container;

static lv_timer_t *timer_time;
static lv_timer_t *timer_sys;

static void obj_anim_set_y(lv_anim_t* a, int32_t v)
{
   lv_obj_t *obj = (lv_obj_t *)a->user_data;
   lv_obj_set_style_y(obj, v, 0);
}

static void obj_anima_set_opa(lv_anim_t* a, int32_t v)
{
   lv_obj_t *obj = (lv_obj_t *)a->user_data;
   lv_obj_set_style_opa(obj, v, 0);
}

static void drawn_down_anim(lv_obj_t * obj, int direction)
{
    lv_anim_t drawn_down_pos;
    lv_anim_t drawn_down_opa;

    lv_coord_t start_y = lv_obj_get_y(obj);
    lv_coord_t end_y = 0;

    if (direction == DRAW_DOWN_MENU_UP) {
        end_y = -LV_VER_RES * 0.92;
    } else if (direction == DRAW_DOWN_MENU_DOWN) {
        end_y = 0;
    }

    lv_anim_init(&drawn_down_pos);
    lv_anim_set_time(&drawn_down_pos, 200);
    lv_anim_set_user_data(&drawn_down_pos, obj);
    lv_anim_set_custom_exec_cb(&drawn_down_pos, obj_anim_set_y);
    lv_anim_set_values(&drawn_down_pos, start_y, end_y);
    lv_anim_set_path_cb(&drawn_down_pos, lv_anim_path_ease_out);
    lv_anim_set_delay(&drawn_down_pos, 0);
    lv_anim_set_deleted_cb(&drawn_down_pos, NULL);
    lv_anim_set_playback_time(&drawn_down_pos, 0);
    lv_anim_set_playback_delay(&drawn_down_pos, 0);
    lv_anim_set_repeat_count(&drawn_down_pos, 0);
    lv_anim_set_repeat_delay(&drawn_down_pos, 0);
    lv_anim_set_early_apply(&drawn_down_pos, false);

    lv_anim_start(&drawn_down_pos);

    if (direction == DRAW_DOWN_MENU_UP && disp_size != DISP_SMALL) {
        lv_anim_init(&drawn_down_opa);
        lv_anim_set_time(&drawn_down_opa, 400);
        lv_anim_set_user_data(&drawn_down_opa, obj);
        lv_anim_set_custom_exec_cb(&drawn_down_opa, obj_anima_set_opa);
        lv_anim_set_values(&drawn_down_opa, LV_OPA_100, LV_OPA_0);
        lv_anim_set_path_cb(&drawn_down_opa, lv_anim_path_linear);
        lv_anim_set_delay(&drawn_down_opa, 200);
        lv_anim_set_deleted_cb(&drawn_down_opa, NULL);
        lv_anim_set_playback_time(&drawn_down_opa, 0);
        lv_anim_set_playback_delay(&drawn_down_opa, 0);
        lv_anim_set_repeat_count(&drawn_down_opa, 0);
        lv_anim_set_repeat_delay(&drawn_down_opa, 0);
        lv_anim_set_early_apply(&drawn_down_opa, false);
        lv_anim_start(&drawn_down_opa);
    }
}

void time_info_update_cb(lv_timer_t * timer)
{
    static int hours = 12;
    static int minutes = 1;

    lv_obj_t *time_info = (lv_obj_t *)timer->user_data;
    minutes++;
    lv_label_set_text_fmt(time_info, "%2d:%2d", hours, minutes);
    if (minutes > 60)  {
        hours++;
        minutes = 0;
    }
    if (hours > 24) hours = 0;
}

void sys_info_update_cb(lv_timer_t * timer)
{
    lv_obj_t *fps_info = (lv_obj_t *)timer->user_data;

    sys_met_hub_get_info(sys_info);
    lv_label_set_text_fmt(fps_info, "fps %d cpu %d", sys_info->draw_fps, (int)sys_info->cpu_usage);
}

static void act_sub_tabview(lv_event_t * e)
{
    lv_obj_t * tapview = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    int footer_num = lv_obj_get_child_cnt(footer_container);
    int footer_now = lv_tabview_get_tab_act(tapview);

    if (code == LV_EVENT_VALUE_CHANGED) {
        for (int i = 0; i < footer_num; i++) {
            footer_ui_switch(lv_obj_get_child(footer_container, i), FOOTER_TURN_OFF);
        }
        footer_ui_switch(lv_obj_get_child(footer_container, footer_now), FOOTER_TURN_ON);
        last_tabview_tab = footer_now;
    }
}

static void draw_down_menu_move(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);

    if (code == LV_EVENT_RELEASED) {
        lv_coord_t y = lv_obj_get_y(obj);
        if (y > -LV_VER_RES * 0.6) {
            drawn_down_anim(obj, DRAW_DOWN_MENU_DOWN);
        } else {
            drawn_down_anim(obj, DRAW_DOWN_MENU_UP);
        }
        return;
    }

    /* LV_OBJ_FLAG_USER_4 flag is used to record whether there is movement and the click event will not be triggered if there is movement */
    if (LV_EVENT_PRESSED == code)  lv_obj_clear_flag(obj, LV_OBJ_FLAG_USER_4);

    if (code == LV_EVENT_PRESSING) {
        if (lv_obj_has_flag(obj, LV_OBJ_FLAG_USER_4) == false)  lv_obj_set_style_opa(obj, LV_OPA_100, 0);
        lv_indev_t *indev = lv_indev_get_act();
        if (indev == NULL) return;

        lv_indev_type_t indev_type = lv_indev_get_type(indev);
        if (indev_type != LV_INDEV_TYPE_POINTER) return;

        lv_point_t vect;
        lv_indev_get_vect(indev, &vect);
        lv_coord_t y = lv_obj_get_y(obj) + vect.y;

        /* after moving it will not trigger click */
        lv_obj_add_flag(obj, LV_OBJ_FLAG_USER_4);

        if (y >= -LV_VER_RES * 0.92 && y <= 0)  lv_obj_set_y(obj, y);
    }
}

static lv_obj_t *draw_down_menu_init(lv_obj_t *parent)
{
    lv_obj_t *draw_down_cont = lv_obj_create(parent);
    lv_obj_clear_flag(draw_down_cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(draw_down_cont, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(draw_down_cont, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    lv_obj_set_size(draw_down_cont, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_align(draw_down_cont, LV_ALIGN_TOP_LEFT);
    lv_obj_set_pos(draw_down_cont, 0, -LV_VER_RES * 0.92);
    lv_obj_set_style_bg_opa(draw_down_cont, LV_OPA_100, 0);
    lv_obj_set_style_bg_color(draw_down_cont, lv_theme_get_color_secondary(NULL), 0);
    lv_obj_set_style_radius(draw_down_cont, 25, 0);
    lv_obj_set_style_border_width(draw_down_cont, 0, 0);
    lv_obj_set_style_border_color(draw_down_cont, lv_color_hex(0x0), 0);
    lv_obj_set_style_shadow_color(draw_down_cont, lv_color_hex(0x0), 0);
    lv_obj_set_style_shadow_opa(draw_down_cont, LV_OPA_100, 0);
    lv_obj_set_style_shadow_spread(draw_down_cont, 2, 0);
    lv_obj_set_style_shadow_ofs_x(draw_down_cont, 0, 0);
    lv_obj_set_style_shadow_ofs_y(draw_down_cont, 1, 0);
    lv_obj_set_style_shadow_width(draw_down_cont, 4, 0);

    lv_obj_add_event_cb(draw_down_cont, draw_down_menu_move, LV_EVENT_ALL, NULL);

    return draw_down_cont;
}

void navigator_ui_init_impl(lv_obj_t *parent)
{
    sys_info = sys_met_hub_create();

    lv_obj_t *tv_top = lv_tabview_create(parent, LV_DIR_TOP, 0);
    lv_obj_set_size(tv_top, lv_pct(100), lv_pct(100));
    lv_obj_set_align(tv_top, LV_ALIGN_TOP_LEFT);

    icon_app_list *icon_list = NULL;
    int icon_num = 0;
    if (disp_size == DISP_SMALL) {
        icon_list = icon_list_480_272;
        icon_num = (sizeof(icon_list_480_272) / sizeof(icon_list_480_272[0]));
    } else {
        icon_list = icon_list_1024_600;
        icon_num = (sizeof(icon_list_1024_600) / sizeof(icon_list_1024_600[0]));
    }

    char tv_tab_name[64] = {0};
    int now_icon_num = 0;
    int tv_tab_num = icon_num / 8;
    if (icon_num % 8 != 0) tv_tab_num++;

    lv_obj_set_style_bg_opa(tv_top, LV_OPA_100, 0);
    lv_obj_set_style_bg_color(tv_top, lv_theme_get_color_primary(NULL), 0);

    lv_obj_t *company_info = commom_label_ui_create(parent, 16, 0);
    lv_label_set_text_fmt(company_info, "ArtInChip");
    lv_obj_set_align(company_info, LV_ALIGN_BOTTOM_RIGHT);
    lv_obj_set_style_text_color(company_info, lv_theme_get_color_emphasize(NULL), 0);

    footer_container = lv_obj_create(parent);
    lv_obj_set_style_border_width(footer_container, 0, 0);
    lv_obj_set_style_bg_color(footer_container, lv_theme_get_color_primary(NULL), 0);
    lv_obj_set_style_bg_opa(footer_container, LV_OPA_0, 0);
    lv_obj_set_size(footer_container, LV_HOR_RES * 0.2, LV_VER_RES * 0.2);
    lv_obj_set_align(footer_container, LV_ALIGN_BOTTOM_MID);
    lv_obj_set_pos(footer_container, 0, LV_VER_RES * 0.05);
    lv_obj_clear_flag(footer_container, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_flex_align(footer_container, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_SPACE_BETWEEN);

    lv_obj_t *draw_down = draw_down_menu_init(parent);
    lv_obj_t *fps_info = NULL;
    if (disp_size == DISP_SMALL) {
        fps_info = lv_label_create(draw_down);
        lv_obj_set_align(fps_info, LV_ALIGN_BOTTOM_RIGHT);
        lv_obj_set_pos(fps_info, -14, 10);

        lv_obj_set_style_opa(draw_down, LV_OPA_100, 0);
    } else {
        fps_info = lv_label_create(parent);
        lv_obj_set_align(fps_info, LV_ALIGN_BOTTOM_LEFT);
        lv_obj_set_pos(fps_info, 0, 0);
        lv_obj_set_size(fps_info, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_style_text_color(fps_info, lv_theme_get_color_emphasize(NULL), 0);
        lv_obj_set_style_opa(draw_down, LV_OPA_0, 0);
    }
    lv_label_set_text_fmt(fps_info, "fps %d cpu %d", 0, 0);

    lv_obj_t *time_info = lv_label_create(draw_down);
    lv_obj_set_align(time_info, LV_ALIGN_BOTTOM_LEFT);
    lv_obj_set_pos(time_info, 10, 12);
    lv_obj_set_size(time_info, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_label_set_text_fmt(time_info, "12:00");
    lv_obj_set_style_text_color(time_info, lv_theme_get_color_emphasize(NULL), 0);

    static lv_obj_t * promote_label = NULL;
    promote_label = lv_label_create(draw_down);
    lv_obj_set_align(promote_label, LV_ALIGN_BOTTOM_MID);
    lv_obj_set_pos(promote_label, 0, 12);
    lv_obj_set_style_opa(promote_label, LV_OPA_0, 0);
    lv_obj_set_size(promote_label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_label_set_text_fmt(promote_label, "To be written");
    lv_obj_set_style_text_color(promote_label, lv_theme_get_color_emphasize(NULL), 0);

    for (int i = 0; i < tv_tab_num; i++) {
        snprintf(tv_tab_name, sizeof(tv_tab_name), "tv_tab_num_%d", i);
        lv_obj_t *tv_tab = lv_tabview_add_tab(tv_top, tv_tab_name);

        lv_obj_t *img_btn_contain = lv_obj_create(tv_tab);
        lv_obj_clear_flag(img_btn_contain, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_align(img_btn_contain, LV_ALIGN_CENTER);
        lv_obj_set_pos(img_btn_contain, 0, 0);
        lv_obj_set_size(img_btn_contain, LV_HOR_RES * 0.92, LV_VER_RES * 0.82);
        lv_obj_set_style_border_width(img_btn_contain, 0, 0);
        lv_obj_set_style_bg_opa(img_btn_contain, LV_OPA_0, 0);
        lv_obj_set_style_bg_color(img_btn_contain, lv_theme_get_color_primary(NULL), 0);
        lv_obj_set_flex_flow(img_btn_contain, LV_FLEX_FLOW_ROW_WRAP);
        lv_obj_set_flex_align(img_btn_contain, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_SPACE_BETWEEN);

        lv_obj_t *footer = footer_ui_create(footer_container);
        if (i == 0) {
            footer_ui_switch(footer, FOOTER_TURN_ON);
        }

        for (int j = 0; j < 8; j++) {
            if (now_icon_num == icon_num) break;
            lv_obj_t *btn = imgbtn_icon_ui_create(img_btn_contain ,icon_list[now_icon_num].icon_path);
            if (icon_list[now_icon_num].event_cb != NULL) {
                lv_obj_add_event_cb(btn, icon_list[now_icon_num].event_cb, LV_EVENT_ALL, NULL);
            }
#ifdef PROMOTE_APP
            else {
                lv_obj_add_event_cb(btn, to_be_written_cb, LV_EVENT_ALL, promote_label);
            }
#endif
            now_icon_num++;
        }
    }

    if (disp_size != DISP_SMALL) {
        lv_obj_add_flag(footer_container, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(time_info, LV_OBJ_FLAG_HIDDEN);
    }

    timer_time = lv_timer_create(time_info_update_cb, 1000, time_info);
    timer_sys = lv_timer_create(sys_info_update_cb, 100, fps_info);
#ifndef MONITOR_PERFORMANCE
    lv_obj_add_flag(time_info, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(fps_info, LV_OBJ_FLAG_HIDDEN);
    lv_timer_pause(timer_time);
    lv_timer_pause(timer_sys);
#endif

#ifndef DRAW_DOWN
    lv_obj_add_flag(draw_down, LV_OBJ_FLAG_HIDDEN);
#endif
    lv_tabview_set_act(tv_top, last_tabview_tab, 0);
    lv_obj_add_event_cb(tv_top, act_sub_tabview, LV_EVENT_ALL, NULL);
}

static void ui_navigator_cb(lv_event_t *e)
{
    lv_event_code_t code = (lv_event_code_t)lv_event_get_code(e);
    if (code == LV_EVENT_SCREEN_UNLOAD_START) {
        sys_met_hub_delete(sys_info);
        lv_img_cache_set_size(4);
        lv_timer_del(timer_time);
        lv_timer_del(timer_sys);
    }

    if (code == LV_EVENT_DELETE) {;}

    if (code == LV_EVENT_SCREEN_LOADED) {;}
}

lv_obj_t *navigator_ui_init(void)
{
    if(LV_HOR_RES <= 480) disp_size = DISP_SMALL;
    else if(LV_HOR_RES <= 800) disp_size = DISP_MEDIUM;
    else disp_size = DISP_LARGE;

    lv_obj_t *ui_navigator = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_navigator, LV_OBJ_FLAG_SCROLLABLE);
    lv_img_cache_set_size(16);

    navigator_ui_init_impl(ui_navigator); /* capable of adapting to screen size */

    lv_obj_add_event_cb(ui_navigator, ui_navigator_cb, LV_EVENT_ALL, NULL);

    return ui_navigator;
}
