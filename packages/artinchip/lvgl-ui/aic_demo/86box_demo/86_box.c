/*
 * Copyright (c) 2022-2023, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors:  Zequan Liang <zequan.liang@artinchip.com>
 */

#include "aic_ui.h"
#include "lvgl.h"

#include "./components/common_comp.h"

#define HOURS_TO_SECONDS(hours)   ((hours) * 60 * 60)
#define MINUTES_TO_SECONDS(minutes) ((minutes) * 60)
#define GET_HOURS(seconds)   ((seconds) / (60 * 60))
#define GET_MINUTES(seconds) (((seconds) % (60 * 60)) / 60)

static lv_obj_t *a_86_box_ui_home_create(lv_obj_t *parent);
static lv_obj_t *a_86_box_ui_back_home_create(lv_obj_t *parent);
static lv_obj_t *a_86_box_ui_leave_home_create(lv_obj_t *parent);

static void ui_86_box_cb(lv_event_t *e);
static void switch_to_home_ui(lv_event_t *e);
static void switch_to_back_home_ui(lv_event_t *e);
static void switch_to_leave_home_ui(lv_event_t *e);

static void ui_back_home_temperature_add_cb(lv_event_t *e);
static void ui_back_home_temperature_delete_cb(lv_event_t *e);
static void ui_back_home_updata_seconds(lv_event_t *e);

static char *num_to_imgnum_path(int num);
static int is_lvgl_thread_stack_size_enough(void);

static lv_obj_t *ui_86_box;

/* ui home variable */
static lv_obj_t *ui_home;
static lv_obj_t *ui_home_time_img_container;
static lv_obj_t *ui_home_temperature_label;
static lv_obj_t *ui_home_airquality_label;
static lv_obj_t *ui_home_humidity_label;
static long ui_home_seconds = HOURS_TO_SECONDS(8) + MINUTES_TO_SECONDS(0);
static lv_timer_t *ui_home_timer;

/* ui back home variable */
static lv_obj_t *ui_back_home;
static int ui_back_home_temperature = 24;
static int ui_back_home_seconds = 0;

/* ui leave home variable */
static lv_obj_t *ui_leave_home;

void a_86_box_ui_init()
{
    if (is_lvgl_thread_stack_size_enough() == -1) {
        printf("lvgl thread stack is too small, it is recommended to set it to 98306.\n");
        printf("86box demo is has already exited!");
        return;
    }

    ui_86_box = lv_obj_create(NULL);
    lv_obj_set_size(ui_86_box, LV_HOR_RES, LV_VER_RES);
    lv_obj_clear_flag(ui_86_box, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *bg = lv_img_create(ui_86_box);
    lv_img_set_src(bg, LVGL_PATH(86box/bg_page.jpg));

    ui_home = a_86_box_ui_home_create(ui_86_box);

    lv_obj_add_event_cb(ui_86_box, ui_86_box_cb, LV_EVENT_ALL, NULL);
    lv_scr_load(ui_86_box);
}

static void ui_86_box_cb(lv_event_t *e)
{
    lv_event_code_t code = (lv_event_code_t)lv_event_get_code(e);
    if (code == LV_EVENT_SCREEN_UNLOAD_START) {
        lv_img_cache_set_size(4);
        lv_timer_del(ui_home_timer);
        ui_leave_home = NULL;
        ui_back_home = NULL;
        ui_home = NULL;
    }

    if (code == LV_EVENT_DELETE) {;}
    if (code == LV_EVENT_SCREEN_LOADED) {
        lv_img_cache_set_size(5);
    }
}

static int get_random_value(int min, int max)
{
    return (rand()) % (max - min + 1) + min;
}

void random_update_cb(lv_timer_t * timer)
{
    lv_obj_t *hours_num_0 = lv_obj_get_child(ui_home_time_img_container, 0);
    lv_obj_t *hours_num_1 = lv_obj_get_child(ui_home_time_img_container, 1);
    lv_obj_t *colon = lv_obj_get_child(ui_home_time_img_container, 2);
    lv_obj_t *minute_num_0 = lv_obj_get_child(ui_home_time_img_container, 3);
    lv_obj_t *minute_num_1 = lv_obj_get_child(ui_home_time_img_container, 4);

    ui_home_seconds++;
    if (ui_home_seconds % 2) {
        lv_obj_add_flag(colon, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_clear_flag(colon, LV_OBJ_FLAG_HIDDEN);
    }

    if (ui_home_seconds % 60) {
        lv_img_set_src(hours_num_0, num_to_imgnum_path(GET_HOURS(ui_home_seconds) / 10));
        lv_img_set_src(hours_num_1, num_to_imgnum_path(GET_HOURS(ui_home_seconds) % 10));
        lv_img_set_src(minute_num_0, num_to_imgnum_path(GET_MINUTES(ui_home_seconds) / 10));
        lv_img_set_src(minute_num_1, num_to_imgnum_path(GET_MINUTES(ui_home_seconds) % 10));
    }

    lv_label_set_text_fmt(ui_home_temperature_label, "温度 %02d°C", get_random_value(0, 99));
    lv_label_set_text_fmt(ui_home_airquality_label, "空气质量 %02d", get_random_value(0, 99));
    lv_label_set_text_fmt(ui_home_humidity_label, "湿度 %02d", get_random_value(0, 99));
}

static lv_obj_t *a_86_box_ui_home_create(lv_obj_t *parent)
{
    lv_obj_t *ui_home = lv_obj_create(parent);
    lv_obj_remove_style_all(ui_home);
    lv_obj_set_size(ui_home, lv_pct(100), lv_pct(100));

    lv_obj_t *weather = lv_img_create(ui_home);
    lv_obj_set_pos(weather, 800, 80);
    lv_img_set_src(weather, LVGL_PATH(metar.png));

    /* optimize display speed by switching from freetype font to img */
    ui_home_time_img_container = lv_obj_create(ui_home);
    lv_obj_remove_style_all(ui_home_time_img_container);
    lv_obj_set_size(ui_home_time_img_container, 500, 100);
    lv_obj_set_pos(ui_home_time_img_container, 80, 110);
    lv_obj_t *hours_num_0 = lv_img_create(ui_home_time_img_container);
    lv_img_set_src(hours_num_0, num_to_imgnum_path(GET_HOURS(ui_home_seconds) / 10));
    lv_obj_set_pos(hours_num_0, 0, 0);
    lv_obj_t *hours_num_1 = lv_img_create(ui_home_time_img_container);
    lv_img_set_src(hours_num_1, num_to_imgnum_path(GET_HOURS(ui_home_seconds) % 10));
    lv_obj_set_pos(hours_num_1, 50, 0);
    lv_obj_t *colon = lv_img_create(ui_home_time_img_container);
    lv_img_set_src(colon, LVGL_PATH(colon.png));
    lv_obj_set_pos(colon, 100, 10);
    lv_obj_t *minute_num_0 = lv_img_create(ui_home_time_img_container);
    lv_img_set_src(minute_num_0, num_to_imgnum_path(GET_MINUTES(ui_home_seconds) / 10));
    lv_obj_set_pos(minute_num_0, 120, 0);
    lv_obj_t *minute_num_1 = lv_img_create(ui_home_time_img_container);
    lv_img_set_src(minute_num_1, num_to_imgnum_path(GET_MINUTES(ui_home_seconds) % 10));
    lv_obj_set_pos(minute_num_1, 170, 0);

    lv_obj_t *data_label = ft_label_comp(ui_home, 24, FT_FONT_STYLE_NORMAL, LVGL_PATH_ORI(NotoSansSChinese-Regular_3700.ttf));
    lv_label_set_text(data_label, "2024/03/11 星期一");
    lv_obj_set_pos(data_label, 85, 180);

    lv_obj_t *temperature_icon = lv_img_create(ui_home);
    lv_obj_set_pos(temperature_icon, 72, 300);
    lv_img_set_src(temperature_icon, LVGL_PATH(temperature.png));
    ui_home_temperature_label = ft_label_comp(ui_home, 24, FT_FONT_STYLE_NORMAL, LVGL_PATH_ORI(NotoSansSChinese-Regular_3700.ttf));
    lv_label_set_text_fmt(ui_home_temperature_label, "温度 %02d°C", get_random_value(0, 99));
    lv_obj_set_pos(ui_home_temperature_label, 110, 300);

    lv_obj_t *airquality_icon = lv_img_create(ui_home);
    lv_obj_set_pos(airquality_icon, 230, 300);
    lv_img_set_src(airquality_icon, LVGL_PATH(airquality.png));
    ui_home_airquality_label = ft_label_comp(ui_home, 24, FT_FONT_STYLE_NORMAL, LVGL_PATH_ORI(NotoSansSChinese-Regular_3700.ttf));
    lv_label_set_text_fmt(ui_home_airquality_label, "空气质量 %02d", get_random_value(0, 99));
    lv_obj_set_pos(ui_home_airquality_label, 270, 300);

    lv_obj_t *humidity_icon = lv_img_create(ui_home);
    lv_obj_set_pos(humidity_icon, 420, 300);
    lv_img_set_src(humidity_icon, LVGL_PATH(humidity.png));
    ui_home_humidity_label = ft_label_comp(ui_home, 24, FT_FONT_STYLE_NORMAL, LVGL_PATH_ORI(NotoSansSChinese-Regular_3700.ttf));
    lv_label_set_text_fmt(ui_home_humidity_label, "湿度 %02d", get_random_value(0, 99));
    lv_obj_set_pos(ui_home_humidity_label, 460, 300);

    lv_obj_t *weather_icon = lv_img_create(ui_home);
    lv_obj_set_pos(weather_icon, 820, 210);
    lv_img_set_src(weather_icon, LVGL_PATH(weather.png));
    lv_obj_t *weather_label = ft_label_comp(ui_home, 24, FT_FONT_STYLE_NORMAL, LVGL_PATH_ORI(NotoSansSChinese-Regular_3700.ttf));
    lv_label_set_text(weather_label, "多云");
    lv_obj_set_pos(weather_label, 874, 205);

    lv_obj_t *back_home_btn = com_imgbtn_comp(ui_home, LVGL_PATH(home_mode.png), NULL);
    lv_obj_set_pos(back_home_btn, 65, 415);
    lv_obj_t *leave_home_btn = com_imgbtn_comp(ui_home, LVGL_PATH(offine_mode.png), NULL);
    lv_obj_set_pos(leave_home_btn, 394, 415);
    lv_obj_t *more_btn = com_imgbtn_comp(ui_home, LVGL_PATH(more_mode.png), NULL);
    lv_obj_set_pos(more_btn, 724, 415);

    ui_home_timer = lv_timer_create(random_update_cb, 1000, NULL);
    lv_obj_add_event_cb(back_home_btn, switch_to_back_home_ui, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(leave_home_btn, switch_to_leave_home_ui, LV_EVENT_ALL, NULL);

    return ui_home;
}

static lv_obj_t *a_86_box_ui_back_home_create(lv_obj_t *parent)
{
    lv_obj_t *ui_back_home = lv_obj_create(parent);
    lv_obj_remove_style_all(ui_back_home);
    lv_obj_set_size(ui_back_home, lv_pct(100), lv_pct(100));

    lv_obj_t *group;
    lv_obj_t *grop_label;
    group = com_imgbtn_switch_comp(ui_back_home, LVGL_PATH(getup_group.png), LVGL_PATH(getup_group_on.png));
    lv_obj_set_pos(group, 65, 108);
    grop_label = ft_label_comp(group, 24, FT_FONT_STYLE_NORMAL, LVGL_PATH_ORI(NotoSansSChinese-Regular_3700.ttf));
    lv_obj_align(grop_label, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_label_set_text(grop_label, "起床模式");

    group = com_imgbtn_switch_comp(ui_back_home, LVGL_PATH(sleep_group.png), LVGL_PATH(sleep_group_on.png));
    lv_obj_set_pos(group, 294, 108);
    grop_label = ft_label_comp(group, 24, FT_FONT_STYLE_NORMAL, LVGL_PATH_ORI(NotoSansSChinese-Regular_3700.ttf));
    lv_obj_align(grop_label, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_label_set_text(grop_label, "睡眠模式");

    group = com_imgbtn_switch_comp(ui_back_home, LVGL_PATH(leave_home_group.png), LVGL_PATH(leave_home_group_on.png));
    lv_obj_set_pos(group, 519, 108);
    grop_label = ft_label_comp(group, 24, FT_FONT_STYLE_NORMAL, LVGL_PATH_ORI(NotoSansSChinese-Regular_3700.ttf));
    lv_obj_align(grop_label, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_label_set_text(grop_label, "离家模式");

    group = com_imgbtn_switch_comp(ui_back_home, LVGL_PATH(romantic_group.png), LVGL_PATH(romantic_group_on.png));
    lv_obj_set_pos(group, 746, 108);
    grop_label = ft_label_comp(group, 24, FT_FONT_STYLE_NORMAL, LVGL_PATH_ORI(NotoSansSChinese-Regular_3700.ttf));
    lv_obj_align(grop_label, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_label_set_text(grop_label, "浪漫模式");

    lv_obj_t *setting_bg = lv_img_create(ui_back_home);
    lv_obj_set_pos(setting_bg, 65, 320);
    lv_img_set_src(setting_bg, LVGL_PATH(setting_bg_page02.png));
    lv_obj_t *setting_icon = lv_img_create(ui_back_home);
    lv_obj_set_pos(setting_icon, 109, 328);
    lv_img_set_src(setting_icon, LVGL_PATH(airconditioning.png));
    lv_obj_t *setting_label = ft_label_comp(ui_back_home, 24, FT_FONT_STYLE_NORMAL, LVGL_PATH_ORI(NotoSansSChinese-Regular_3700.ttf));
    lv_obj_set_pos(setting_label, 218, 336);
    lv_label_set_text(setting_label, "空调");
    lv_obj_t *setting_label_detail = ft_label_comp(ui_back_home, 20, FT_FONT_STYLE_NORMAL, LVGL_PATH_ORI(NotoSansSChinese-Regular_3700.ttf));
    lv_obj_set_pos(setting_label_detail, 218, 371);
    lv_label_set_text(setting_label_detail, "制冷");
    lv_obj_set_style_text_color(setting_label_detail, lv_color_hex(0xF0B785), 0);
    lv_obj_t *air_setting_btn_delete = com_imgbtn_comp(ui_back_home, LVGL_PATH(delete.png), LVGL_PATH(delete_over.png));
    lv_obj_set_pos(air_setting_btn_delete, 131, 464);
    lv_obj_t *air_setting_btn_add = com_imgbtn_comp(ui_back_home, LVGL_PATH(add.png), LVGL_PATH(add_over.png));
    lv_obj_set_pos(air_setting_btn_add, 398, 464);
    lv_obj_t *air_setting_temperature = ft_label_comp(ui_back_home, 36, FT_FONT_STYLE_NORMAL, LVGL_PATH_ORI(NotoSansSChinese-Regular_3700.ttf));
    lv_obj_set_pos(air_setting_temperature, 249, 446);
    lv_label_set_text_fmt(air_setting_temperature, "%02d °C", ui_back_home_temperature);

    setting_bg = lv_img_create(ui_back_home);
    lv_obj_set_pos(setting_bg, 521, 320);
    lv_img_set_src(setting_bg, LVGL_PATH(setting_bg_page02.png));
    setting_icon = lv_img_create(ui_back_home);
    lv_obj_set_pos(setting_icon, 562, 328);
    lv_img_set_src(setting_icon, LVGL_PATH(curtain.png));
    setting_label = ft_label_comp(ui_back_home, 24, FT_FONT_STYLE_NORMAL, LVGL_PATH_ORI(NotoSansSChinese-Regular_3700.ttf));
    lv_obj_set_pos(setting_label, 671, 336);
    lv_label_set_text(setting_label, "窗帘");
    lv_obj_t *curtain_setting_btn_open = com_imgbtn_comp(ui_back_home, LVGL_PATH(open.png), LVGL_PATH(open_over.png));
    lv_obj_set_pos(curtain_setting_btn_open, 585, 464);
    lv_obj_t *curtain_setting_btn_pause = com_imgbtn_comp(ui_back_home, LVGL_PATH(pause.png), LVGL_PATH(pause_over.png));
    lv_obj_set_pos(curtain_setting_btn_pause, 719, 464);
    lv_obj_t *curtain_setting_btn_close = com_imgbtn_comp(ui_back_home, LVGL_PATH(close.png), LVGL_PATH(close_over.png));
    lv_obj_set_pos(curtain_setting_btn_close, 853, 464);

    lv_obj_t *back_home = com_imgbtn_comp(ui_back_home, LVGL_PATH(back.png), NULL);
    lv_obj_set_pos(back_home, 40, 40);

    lv_obj_add_event_cb(back_home, switch_to_home_ui, LV_EVENT_ALL, ui_back_home);
    lv_obj_add_event_cb(air_setting_btn_add, ui_back_home_temperature_add_cb, LV_EVENT_ALL, air_setting_temperature);
    lv_obj_add_event_cb(air_setting_btn_delete, ui_back_home_temperature_delete_cb, LV_EVENT_ALL, air_setting_temperature);

    return ui_back_home;
}

static lv_obj_t *a_86_box_ui_leave_home_slide_create(lv_obj_t *parent)
{
    lv_obj_t *slide = lv_slider_create(parent);
    lv_slider_set_range(slide, 0, 100);
    lv_slider_set_mode(slide, LV_SLIDER_MODE_NORMAL);
    lv_slider_set_value(slide, ui_back_home_seconds, LV_ANIM_OFF);
    lv_obj_set_style_border_width(slide, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(slide, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_width(slide, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_size(slide, 380, 10);

    lv_obj_set_style_bg_opa(slide, 60, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(slide, lv_color_hex(0x453B64), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(slide, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(slide, 50, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(slide, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(slide, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    lv_obj_set_style_bg_opa(slide, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(slide, lv_color_hex(0xDB6895), LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(slide, LV_GRAD_DIR_HOR, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_color(slide, lv_color_hex(0x4840EB), LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_main_stop(slide, 0, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_stop(slide, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(slide, 50, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    lv_obj_set_style_bg_opa(slide, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(slide, lv_color_hex(0xDB6895), LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(slide, LV_GRAD_DIR_VER, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_color(slide, lv_color_hex(0x4840EB), LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_main_stop(slide, 0, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_stop(slide, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(slide, 50, LV_PART_KNOB|LV_STATE_DEFAULT);

    return slide;
}

static lv_obj_t *a_86_box_ui_leave_home_create(lv_obj_t *parent)
{
    lv_obj_t *ui_leave_home = lv_obj_create(parent);
    lv_obj_remove_style_all(ui_leave_home);
    lv_obj_set_size(ui_leave_home, lv_pct(100), lv_pct(100));

    /* setting */
    lv_obj_t *setting_bg = lv_img_create(ui_leave_home);
    lv_obj_set_pos(setting_bg, 67, 109);
    lv_img_set_src(setting_bg, LVGL_PATH(settingbg1_page03.png));
    lv_obj_t *setting_icon = lv_img_create(setting_bg);
    lv_img_set_src(setting_icon, LVGL_PATH(airconditioning_page03.png));
    lv_obj_align(setting_icon, LV_ALIGN_LEFT_MID, 20, 0);
    lv_obj_t *setting_label = ft_label_comp(setting_bg, 24, FT_FONT_STYLE_NORMAL, LVGL_PATH_ORI(NotoSansSChinese-Regular_3700.ttf));
    lv_label_set_text(setting_label, "空调");
    lv_obj_align(setting_label, LV_ALIGN_LEFT_MID, 120, -10);
    lv_obj_t *setting_switch = com_imgbtn_switch_comp(setting_bg, LVGL_PATH(switch_off.png), LVGL_PATH(switch_on.png));
    lv_obj_align(setting_switch, LV_ALIGN_RIGHT_MID, -20, 0);

    setting_bg = lv_img_create(ui_leave_home);
    lv_obj_set_pos(setting_bg, 521, 109);
    lv_img_set_src(setting_bg, LVGL_PATH(settingbg1_page03.png));
    setting_icon = lv_img_create(setting_bg);
    lv_img_set_src(setting_icon, LVGL_PATH(curtain_page03.png));
    lv_obj_align(setting_icon, LV_ALIGN_LEFT_MID, 20, 0);
    setting_label = ft_label_comp(setting_bg, 24, FT_FONT_STYLE_NORMAL, LVGL_PATH_ORI(NotoSansSChinese-Regular_3700.ttf));
    lv_label_set_text(setting_label, "窗帘");
    lv_obj_align(setting_label, LV_ALIGN_LEFT_MID, 120, -10);
    setting_switch = com_imgbtn_switch_comp(setting_bg, LVGL_PATH(switch_off.png), LVGL_PATH(switch_on.png));
    lv_obj_align(setting_switch, LV_ALIGN_RIGHT_MID, -20, 0);

    setting_bg = lv_img_create(ui_leave_home);
    lv_obj_set_pos(setting_bg, 67, 235);
    lv_img_set_src(setting_bg, LVGL_PATH(settingbg1_page03.png));
    setting_icon = lv_img_create(setting_bg);
    lv_img_set_src(setting_icon, LVGL_PATH(bedroom_page03.png));
    lv_obj_align(setting_icon, LV_ALIGN_LEFT_MID, 20, 0);
    setting_label = ft_label_comp(setting_bg, 24, FT_FONT_STYLE_NORMAL, LVGL_PATH_ORI(NotoSansSChinese-Regular_3700.ttf));
    lv_label_set_text(setting_label, "卧室");
    lv_obj_align(setting_label, LV_ALIGN_LEFT_MID, 120, -10);
    setting_switch = com_imgbtn_switch_comp(setting_bg, LVGL_PATH(switch_off.png), LVGL_PATH(switch_on.png));
    lv_obj_align(setting_switch, LV_ALIGN_RIGHT_MID, -20, 0);

    setting_bg = lv_img_create(ui_leave_home);
    lv_obj_set_pos(setting_bg, 521, 235);
    lv_img_set_src(setting_bg, LVGL_PATH(settingbg1_page03.png));
    setting_icon = lv_img_create(setting_bg);
    lv_img_set_src(setting_icon, LVGL_PATH(corridorlamp_page03.png));
    lv_obj_align(setting_icon, LV_ALIGN_LEFT_MID, 20, 0);
    setting_label = ft_label_comp(setting_bg, 24, FT_FONT_STYLE_NORMAL, LVGL_PATH_ORI(NotoSansSChinese-Regular_3700.ttf));
    lv_label_set_text(setting_label, "廊灯");
    lv_obj_align(setting_label, LV_ALIGN_LEFT_MID, 120, -10);
    setting_switch = com_imgbtn_switch_comp(setting_bg, LVGL_PATH(switch_off.png), LVGL_PATH(switch_on.png));
    lv_obj_align(setting_switch, LV_ALIGN_RIGHT_MID, -20, 0);

    /* setting2 left */
    lv_obj_t *setting2_bg = lv_img_create(ui_leave_home);
    lv_obj_set_pos(setting2_bg, 67, 358);
    lv_img_set_src(setting2_bg, LVGL_PATH(settingbg2_page03.png));

    lv_obj_t *song_cover = lv_img_create(ui_leave_home);
    lv_obj_set_pos(song_cover, 97, 383);
    lv_img_set_src(song_cover, LVGL_PATH(song_cover.png));

    lv_obj_t *song_name = ft_label_comp(ui_leave_home, 24, FT_FONT_STYLE_NORMAL, LVGL_PATH_ORI(NotoSansSChinese-Regular_3700.ttf));
    lv_obj_set_pos(song_name, 210, 376);
    lv_label_set_text(song_name, "日落");

    lv_obj_t *singer_name = ft_label_comp(ui_leave_home, 18, FT_FONT_STYLE_NORMAL, LVGL_PATH_ORI(NotoSansSChinese-Regular_3700.ttf));
    lv_obj_set_pos(singer_name, 210, 411);
    lv_label_set_text(singer_name, "Known");
    lv_obj_set_style_text_color(singer_name, lv_color_hex(0x919090), 0);

    lv_obj_t *setting2_btn_play = com_imgbtn_switch_comp(ui_leave_home, LVGL_PATH(play.png), LVGL_PATH(pause_over.png));
    lv_obj_set_pos(setting2_btn_play, 399, 383);
    lv_obj_t *setting2_btn_last = com_imgbtn_comp(ui_leave_home, LVGL_PATH(play_last.png), NULL);
    lv_obj_set_pos(setting2_btn_last, 365, 394);
    lv_obj_t *setting2_btn_next = com_imgbtn_comp(ui_leave_home, LVGL_PATH(play_next.png), NULL);
    lv_obj_set_pos(setting2_btn_next, 452, 394);

    lv_obj_t *progress_label_start = ft_label_comp(ui_leave_home, 18, FT_FONT_STYLE_NORMAL, LVGL_PATH_ORI(NotoSansSChinese-Regular_3700.ttf));
    lv_obj_set_pos(progress_label_start, 97, 520);
    lv_label_set_text_fmt(progress_label_start, "%02d:%02d", GET_MINUTES(ui_back_home_seconds), ui_back_home_seconds % 60);
    lv_obj_set_style_text_color(progress_label_start, lv_color_hex(0x919090), 0);
    lv_obj_t *progress_label_end = ft_label_comp(ui_leave_home, 18, FT_FONT_STYLE_NORMAL, LVGL_PATH_ORI(NotoSansSChinese-Regular_3700.ttf));
    lv_obj_set_pos(progress_label_end, 448, 520);
    lv_label_set_text_fmt(progress_label_end, "%02d:%02d", GET_MINUTES(100), 100 % 60);
    lv_obj_set_style_text_color(progress_label_end, lv_color_hex(0x919090), 0);

    lv_obj_t *slide = a_86_box_ui_leave_home_slide_create(ui_leave_home);
    lv_obj_set_pos(slide, 103, 505);

    /* setting2 right */
    setting2_bg = lv_img_create(ui_leave_home);
    lv_obj_set_pos(setting2_bg, 520, 358);
    lv_img_set_src(setting2_bg, LVGL_PATH(settingbg2_page03.png));

    lv_obj_t *language_icon = lv_img_create(ui_leave_home);
    lv_obj_set_pos(language_icon, 555, 377);
    lv_img_set_src(language_icon, LVGL_PATH(language_page03.png));

    lv_obj_t *language_label = ft_label_comp(ui_leave_home, 18, FT_FONT_STYLE_NORMAL, LVGL_PATH_ORI(NotoSansSChinese-Regular_3700.ttf));
    lv_obj_set_pos(language_label, 867, 391);
    lv_label_set_text(language_label, "中文 >");
    lv_obj_set_style_text_color(language_label, lv_color_hex(0x919090), 0);

    static lv_point_t line_points[] = {{555, 446}, {926, 446}};
    lv_obj_t *line = lv_line_create(ui_leave_home);
    lv_obj_set_style_line_color(line, lv_color_hex(0x433BD2), 0);
    lv_obj_set_style_line_width(line, 2, 0);
    lv_line_set_points(line, line_points, 2);

    lv_obj_t *wifi_switch = com_imgbtn_switch_comp(ui_leave_home, LVGL_PATH(wifi_off.png), LVGL_PATH(wifi_on.png));
    lv_obj_set_pos(wifi_switch, 547, 465);
    lv_obj_t *bluetooth_switch = com_imgbtn_switch_comp(ui_leave_home, LVGL_PATH(bluetooth_off.png), LVGL_PATH(bluetooth_on.png));
    lv_obj_set_pos(bluetooth_switch, 653, 465);
    lv_obj_t *wallpaper_switch = com_imgbtn_switch_comp(ui_leave_home, LVGL_PATH(wallpaper_off.png), LVGL_PATH(wallpaper_on.png));
    lv_obj_set_pos(wallpaper_switch, 754, 465);

    lv_obj_t *back_home = com_imgbtn_comp(ui_leave_home, LVGL_PATH(back.png), NULL);
    lv_obj_set_pos(back_home, 40, 40);

    lv_obj_add_event_cb(slide, ui_back_home_updata_seconds, LV_EVENT_ALL, progress_label_start);
    lv_obj_add_event_cb(back_home, switch_to_home_ui, LV_EVENT_ALL, ui_leave_home);

    return ui_leave_home;
}

static void switch_to_home_ui(lv_event_t *e)
{
    lv_event_code_t code = (lv_event_code_t)lv_event_get_code(e);
    lv_obj_t *ui = lv_event_get_user_data(e);

    if (code == LV_EVENT_CLICKED)
    {
        lv_obj_clear_flag(ui_home, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui, LV_OBJ_FLAG_HIDDEN);
        lv_timer_resume(ui_home_timer);
    }
}

static void switch_to_back_home_ui(lv_event_t *e)
{
    lv_event_code_t code = (lv_event_code_t)lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED)
    {
        if (ui_back_home == NULL)
            ui_back_home = a_86_box_ui_back_home_create(ui_86_box);
        lv_obj_clear_flag(ui_back_home, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_home, LV_OBJ_FLAG_HIDDEN);
        lv_timer_pause(ui_home_timer);
    }
}

static void switch_to_leave_home_ui(lv_event_t *e) {
    lv_event_code_t code = (lv_event_code_t)lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED)
    {
        if (ui_leave_home == NULL)
            ui_leave_home = a_86_box_ui_leave_home_create(ui_86_box);
        lv_obj_clear_flag(ui_leave_home, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_home, LV_OBJ_FLAG_HIDDEN);
        lv_timer_pause(ui_home_timer);
    }
}

static void ui_back_home_temperature_add_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *label = lv_event_get_user_data(e);

    if (code == LV_EVENT_CLICKED || code == LV_EVENT_LONG_PRESSED_REPEAT) {
        ui_back_home_temperature++;
        lv_label_set_text_fmt(label, "%02d °C", ui_back_home_temperature);
    }
}

static void ui_back_home_temperature_delete_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *label = lv_event_get_user_data(e);

    if (code == LV_EVENT_CLICKED || code == LV_EVENT_LONG_PRESSED_REPEAT) {
        ui_back_home_temperature--;
        lv_label_set_text_fmt(label, "%02d °C", ui_back_home_temperature);
    }
}

static void ui_back_home_updata_seconds(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *slide = lv_event_get_target(e);
    lv_obj_t *label = lv_event_get_user_data(e);

    if (code == LV_EVENT_VALUE_CHANGED) {
        ui_back_home_seconds = lv_slider_get_value(slide);
        lv_label_set_text_fmt(label, "%02d:%02d", GET_MINUTES(ui_back_home_seconds), ui_back_home_seconds % 60);
    }
}

/* 0 ~ 9 */
static char *num_to_imgnum_path(int num)
{
    static char *path_list[] = {
        LVGL_PATH(num_0.png),
        LVGL_PATH(num_1.png),
        LVGL_PATH(num_2.png),
        LVGL_PATH(num_3.png),
        LVGL_PATH(num_4.png),
        LVGL_PATH(num_5.png),
        LVGL_PATH(num_6.png),
        LVGL_PATH(num_7.png),
        LVGL_PATH(num_8.png),
        LVGL_PATH(num_9.png)
    };

    return path_list[num];
}

static int is_lvgl_thread_stack_size_enough(void)
{
#if LPKG_LVGL_THREAD_STACK_SIZE
    if (LPKG_LVGL_THREAD_STACK_SIZE < (1024 * 96 * 0.85))
        return -1;
#endif
    return 0;
}
