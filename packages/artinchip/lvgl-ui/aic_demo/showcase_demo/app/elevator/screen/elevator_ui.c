/*
 * Copyright (C) 2023-2024 ArtinChip Technology Co., Ltd.
 * Authors:  Keliang Liu <keliang.liu@artinchip.com>
 *           Huahui Mai <huahui.mai@artinchip.com>
 */

#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "lvgl.h"
#include "aic_ui.h"
#include "aic_lv_ffmpeg.h"
#include "mpp_fb.h"

#include "elevator_ui.h"
#include "elevator_uart.h"

typedef enum {
    DISP_SMALL,
    DISP_MEDIUM,
    DISP_LARGE,
} disp_size_t;

static void ui_elevator_cb(lv_event_t *e);

static char time_str[64];
static int main_level;
static bool main_is_up;
static bool last_is_up;

#if AIC_ELEVATOR_INSIDE
#if LV_ELEVATOR_UART_COMMAND
extern rt_device_t elevator_uart;
static lv_timer_t *elevator_cmd_timer;
static lv_timer_t *elevator_inside_timer;
#endif

static lv_obj_t *player;
static lv_obj_t *title_date;
static lv_obj_t *inside_up_down;
static lv_obj_t *inside_level_1;
static lv_obj_t *inside_level_2;

static lv_obj_t *inside_up_down;
static lv_obj_t *inside_level_1;
static lv_obj_t *inside_level_2;
static lv_obj_t *inside_repari;
#else
static lv_obj_t *outside_up_down;
static lv_obj_t *outside_level_1;
static lv_obj_t *outside_level_2;
static lv_timer_t *elevator_outsize_timer;
#endif
static disp_size_t disp_size;

static int pos_width_conversion(int width)
{
    float scr_width = LV_HOR_RES;
    int out = (int)((scr_width / 1024.0) * width);
    return out;
}

static int pos_height_conversion(int height)
{
    float scr_height = LV_VER_RES;
    return (int)((scr_height / 600.0) * (float)height);
}

static int check_lvgl_de_config(void)
{
#if defined(AICFB_ARGB8888)
    #if LV_COLOR_DEPTH == 32
    return 0;
    #endif
#endif

#if defined(AICFB_RGB565)
    #if LV_COLOR_DEPTH == 16
    return 0;
    #endif
#endif

    return -1;
}

static void fb_dev_layer_set(void)
{
    struct mpp_fb *fb = mpp_fb_open();

    struct aicfb_alpha_config alpha = {0};
    alpha.layer_id = AICFB_LAYER_TYPE_UI;
    alpha.enable = 1;

#if defined(AICFB_RGB565)
    /* due to rgb565 not supporting pixel mixing,
     * the video part needs to be set to cover the ui part.
     */
    alpha.mode = 1;
    alpha.value = 0;
#else
    alpha.mode = 0;
    alpha.value = 255;
#endif

    mpp_fb_ioctl(fb, AICFB_UPDATE_ALPHA_CONFIG, &alpha);
    mpp_fb_close(fb);
}

static void fb_dev_layer_reset(void)
{
#if defined(AICFB_RGB565)
    struct mpp_fb *fb = mpp_fb_open();

    struct aicfb_alpha_config alpha = {0};
    alpha.layer_id = AICFB_LAYER_TYPE_UI;
    alpha.enable = 1;

    alpha.mode = 0;
    alpha.value = 255;

    mpp_fb_ioctl(fb, AICFB_UPDATE_ALPHA_CONFIG, &alpha);
    mpp_fb_close(fb);
#endif
}

#if AIC_ELEVATOR_INSIDE
static char *get_time_string()
{
    static const char *week_day[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
    static time_t unix_time;
    static struct tm *time_info;

    unix_time = time(NULL);
    time_info = localtime(&unix_time);

    int year = time_info->tm_year+1900;
    int month = time_info->tm_mon + 1;
    int day = time_info->tm_mday;
    int weekday = time_info->tm_wday;
    int hour = time_info->tm_hour;
    int minutes = time_info->tm_min;
    int second = time_info->tm_sec;

    snprintf(time_str, sizeof(time_str), "%04d-%02d-%02d %s %02d:%02d:%02d", year, month, day, week_day[weekday], hour, minutes, second);
    return time_str;
}

static void create_data_time(lv_obj_t *parament_obj)
{
    title_date = lv_label_create(parament_obj);
    lv_obj_align(title_date, LV_ALIGN_CENTER, 0, 0);
    lv_label_set_text(title_date, get_time_string());
    lv_obj_center(title_date);
    lv_obj_set_style_text_color(title_date, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_opa(title_date, 255, 0);
}

static void update_data_time(lv_obj_t *inside_level_1)
{
    lv_label_set_text(title_date, get_time_string());
}

static void timer_inside_callback(lv_timer_t *tmr)
{
    char image_str[64];

    if (main_is_up)
        main_level ++;
    else
        main_level --;

    if (main_level > ELEVATOR_MAX_LEVEL) {
        main_is_up = false;
        main_level = ELEVATOR_MAX_LEVEL;
    }
    if (main_level < 1) {
        main_is_up = true;
        main_level = 1;
    }

    if (main_is_up != last_is_up) {
        if(main_is_up)
            lv_img_set_src(inside_up_down, LVGL_PATH(elevator/inside/arrow_up_Image.png));
        else
            lv_img_set_src(inside_up_down, LVGL_PATH(elevator/inside/arrow_down_Image.png));

        last_is_up = main_is_up;
    }

    if (main_level == 12) {
        lv_img_set_src(inside_level_1, LVGL_PATH(elevator/inside/floor_3_12A_Image.png));
        lv_obj_add_flag(inside_level_2, LV_OBJ_FLAG_HIDDEN);
    } else if (main_level == 13) {
        lv_img_set_src(inside_level_1, LVGL_PATH(elevator/inside/floor_3_12B_Image.png));
        lv_obj_add_flag(inside_level_2, LV_OBJ_FLAG_HIDDEN);
    } else if (main_level == 14) {
        lv_img_set_src(inside_level_1, LVGL_PATH(elevator/inside/floor_3_15A_Image.png));
        lv_obj_add_flag(inside_level_2, LV_OBJ_FLAG_HIDDEN);
    } else if (main_level == 15) {
        lv_img_set_src(inside_level_1, LVGL_PATH(elevator/inside/floor_3_15B_Image.png));
        lv_obj_add_flag(inside_level_2, LV_OBJ_FLAG_HIDDEN);
    } else if (main_level == 24) {
        lv_img_set_src(inside_level_1, LVGL_PATH(elevator/inside/floor_3_23A_Image.png));
        lv_obj_add_flag(inside_level_2, LV_OBJ_FLAG_HIDDEN);
    } else if (main_level == 34) {
        lv_img_set_src(inside_level_1, LVGL_PATH(elevator/inside/floor_3_33A_Image.png));
        lv_obj_add_flag(inside_level_2, LV_OBJ_FLAG_HIDDEN);
    } else if (main_level > 0 && main_level < 10) {
        snprintf(image_str, sizeof(image_str), LVGL_PATH(elevator/inside/floor_l_%d_Image.png), main_level);
        lv_img_set_src(inside_level_1, image_str);
        lv_obj_set_pos(inside_level_1, pos_width_conversion(50 + 30), pos_height_conversion(80 + 50 + 121));
        lv_obj_add_flag(inside_level_2, LV_OBJ_FLAG_HIDDEN);
    } else if (main_level > 9 && main_level < 20) {
        lv_img_set_src(inside_level_1, LVGL_PATH(elevator/inside/floor_l_1_Image.png));
        lv_obj_set_pos(inside_level_1, pos_width_conversion(50), pos_height_conversion(80 + 50 + 121));
        snprintf(image_str, sizeof(image_str), LVGL_PATH(elevator/inside/floor_l_%d_Image.png), main_level % 10);
        lv_obj_clear_flag(inside_level_2, LV_OBJ_FLAG_HIDDEN);
        lv_img_set_src(inside_level_2, image_str);
    } else if (main_level > 19 && main_level < 30) {
        lv_img_set_src(inside_level_1, LVGL_PATH(elevator/inside/floor_l_2_Image.png));
        lv_obj_set_pos(inside_level_1, pos_width_conversion(50), pos_height_conversion(80 + 50 + 121));
        snprintf(image_str, sizeof(image_str), LVGL_PATH(elevator/inside/floor_l_%d_Image.png), main_level % 20);
        lv_obj_clear_flag(inside_level_2, LV_OBJ_FLAG_HIDDEN);
        lv_img_set_src(inside_level_2, image_str);
    } else if (main_level > 29 && main_level < 40) {
        lv_img_set_src(inside_level_1, LVGL_PATH(elevator/inside/floor_l_3_Image.png));
        lv_obj_set_pos(inside_level_1, pos_width_conversion(50), pos_height_conversion(80 + 50 + 121));
        snprintf(image_str, sizeof(image_str), LVGL_PATH(elevator/inside/floor_l_%d_Image.png), main_level % 30);
        lv_obj_clear_flag(inside_level_2, LV_OBJ_FLAG_HIDDEN);
        lv_img_set_src(inside_level_2, image_str);
    }

    if(main_level % 2 == 0)
        lv_img_set_src(inside_repari, LVGL_PATH(elevator/inside/repair_Image.png));
    else
        lv_img_set_src(inside_repari, LVGL_PATH(elevator/inside/siji_Image.png));

    update_data_time(title_date);
    return;
}

static void create_player(lv_obj_t * parent)
{
    player = lv_ffmpeg_player_create(parent);
    if (disp_size == DISP_SMALL) {
        lv_obj_set_pos(player, pos_width_conversion(222), pos_height_conversion(80) + 1);
        lv_obj_set_size(player, 480 - pos_width_conversion(222), 278 - (pos_height_conversion(80) * 2));
    } else if (disp_size == DISP_LARGE) {
        lv_obj_set_pos(player, 222, 80);
        lv_obj_set_size(player, 800, 460);
    }
    lv_obj_set_align(player, LV_ALIGN_TOP_LEFT);
    lv_ffmpeg_player_set_auto_restart(player, true);
    lv_ffmpeg_player_set_src(player, "rodata/lvgl_data/common/elevator_mjpg.mp4");
    lv_ffmpeg_player_set_cmd_ex(player, LV_FFMPEG_PLAYER_CMD_START, NULL);
}
#endif

#ifdef LV_ELEVATOR_UART_COMMAND
int inside_up_down_ioctl(void *data, unsigned int data_len)
{
    char *str_rx = data;
    int enable;

    enable = atoi(str_rx);

    if (enable)
        lv_img_set_src(inside_up_down, LVGL_PATH(elevator/inside/arrow_down_Image.png));
    else
        lv_img_set_src(inside_up_down, LVGL_PATH(elevator/inside/arrow_up_Image.png));

    return 0;
}

int inside_level_ioctl(void *data, unsigned int data_len)
{
    char *str_rx = data;
    int main_level = atoi(str_rx);
    char send_srt[16] = { 0 };
    char image_str[64];

    lv_snprintf(send_srt, sizeof(send_srt), "level: %d", main_level);
    rt_device_write(elevator_uart, 0, send_srt, strlen(send_srt));

    if (main_level == 12) {
        lv_img_set_src(inside_level_1, LVGL_PATH(elevator/inside/floor_3_12A_Image.png));
        lv_obj_add_flag(inside_level_2, LV_OBJ_FLAG_HIDDEN);
    } else if (main_level == 13) {
        lv_img_set_src(inside_level_1, LVGL_PATH(elevator/inside/floor_3_12B_Image.png));
        lv_obj_add_flag(inside_level_2, LV_OBJ_FLAG_HIDDEN);
    } else if (main_level == 14) {
        lv_img_set_src(inside_level_1, LVGL_PATH(elevator/inside/floor_3_15A_Image.png));
        lv_obj_add_flag(inside_level_2, LV_OBJ_FLAG_HIDDEN);
    } else if (main_level == 15) {
        lv_img_set_src(inside_level_1, LVGL_PATH(elevator/inside/floor_3_15B_Image.png));
        lv_obj_add_flag(inside_level_2, LV_OBJ_FLAG_HIDDEN);
    } else if (main_level == 24) {
        lv_img_set_src(inside_level_1, LVGL_PATH(elevator/inside/floor_3_23A_Image.png));
        lv_obj_add_flag(inside_level_2, LV_OBJ_FLAG_HIDDEN);
    } else if (main_level == 34) {
        lv_img_set_src(inside_level_1, LVGL_PATH(elevator/inside/floor_3_33A_Image.png));
        lv_obj_add_flag(inside_level_2, LV_OBJ_FLAG_HIDDEN);
    } else if (main_level > 0 && main_level < 10) {
        snprintf(image_str, sizeof(image_str), LVGL_PATH(elevator/inside/floor_l_%d_Image.png), main_level);
        lv_img_set_src(inside_level_1, image_str);
        lv_obj_set_pos(inside_level_1, 50 + 30, 80 + 50 + 121);
        lv_obj_add_flag(inside_level_2, LV_OBJ_FLAG_HIDDEN);
    } else if(main_level > 9 && main_level < 20) {
        lv_img_set_src(inside_level_1, LVGL_PATH(elevator/inside/floor_l_1_Image.png));
        lv_obj_set_pos(inside_level_1, 50, 80 + 50 + 121);
        snprintf(image_str, sizeof(image_str), LVGL_PATH(elevator/inside/floor_l_%d_Image.png), main_level % 10);
        lv_obj_clear_flag(inside_level_2, LV_OBJ_FLAG_HIDDEN);
        lv_img_set_src(inside_level_2, image_str);
    } else if (main_level > 19 && main_level < 30) {
        lv_img_set_src(inside_level_1, LVGL_PATH(elevator/inside/floor_l_2_Image.png));
        lv_obj_set_pos(inside_level_1, 50, 80 + 50 + 121);
        snprintf(image_str,sizeof(image_str),  LVGL_PATH(elevator/inside/floor_l_%d_Image.png), main_level % 20);
        lv_obj_clear_flag(inside_level_2, LV_OBJ_FLAG_HIDDEN);
        lv_img_set_src(inside_level_2, image_str);
    } else if (main_level > 29 && main_level < 40) {
        lv_img_set_src(inside_level_1, LVGL_PATH(elevator/inside/floor_l_3_Image.png));
        lv_obj_set_pos(inside_level_1, 50, 80 + 50 + 121);
        snprintf(image_str, sizeof(image_str), LVGL_PATH(elevator/inside/floor_l_%d_Image.png), main_level % 30);
        lv_obj_clear_flag(inside_level_2, LV_OBJ_FLAG_HIDDEN);
        lv_img_set_src(inside_level_2, image_str);
    }

    return 0;
}

static int play_end(void)
{
    u64 play_time;
    struct av_media_info info;

    lv_ffmpeg_player_set_cmd_ex(player, LV_FFMPEG_PLAYER_CMD_GET_PLAY_TIME_EX, &play_time);
    lv_ffmpeg_player_set_cmd_ex(player, LV_FFMPEG_PLAYER_CMD_GET_MEDIA_INFO_EX, &info);

    if (play_time > info.duration) {
        return true;
    }

    return false;
}

int replayer_ioctl(void *data, unsigned int data_len)
{
#if AIC_ELEVATOR_INSIDE
    char *str[] = { "replayer", "wait player" };

    if(play_end() == true)
    {
        lv_ffmpeg_player_set_cmd_ex(player, LV_FFMPEG_PLAYER_CMD_RESUME, NULL);
        rt_device_write(elevator_uart, 0, str[0], strlen(str[0]));
    }
    else
    {
        rt_device_write(elevator_uart, 0, str[1], strlen(str[1]));
    }
#endif
    return 0;
}
#endif

#if AIC_ELEVATOR_INSIDE
void aic_elevator_create_main(lv_obj_t *tab)
{
    //220 x 80
    lv_obj_t * image_otistitle = lv_img_create(tab);
    lv_img_set_src(image_otistitle, LVGL_PATH(elevator/inside/otistitle.png));
    lv_obj_set_pos(image_otistitle, 0, 0);

    //860 x 80
    lv_obj_t * image_datetime = lv_img_create(tab);
    lv_img_set_src(image_datetime, LVGL_PATH(elevator/inside/datetime.png));
    lv_obj_set_pos(image_datetime, pos_width_conversion(222), 0);

    create_data_time(image_datetime);

    //200 x 520
    lv_obj_t * image_panel = lv_img_create(tab);
    lv_img_set_src(image_panel, LVGL_PATH(elevator/inside/panel.png));
    lv_obj_set_pos(image_panel, pos_width_conversion(0) , pos_height_conversion(82));

    //800 x 60
    lv_obj_t * image_bottom = lv_img_create(tab);
    lv_img_set_src(image_bottom, LVGL_PATH(elevator/inside/bottom.png));
    lv_obj_set_pos(image_bottom, pos_width_conversion(222), pos_height_conversion(540));

    lv_obj_t *title_wel= lv_label_create(image_bottom);
    lv_obj_align(title_wel, LV_ALIGN_CENTER, 0, 0);
    lv_label_set_text(title_wel, "Welcom to OTIS electric");
    lv_obj_center(title_wel);
    lv_obj_set_style_text_color(title_wel, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_opa(title_wel, 255, 0);

    //117 x 121
    inside_up_down = lv_img_create(tab);
    lv_img_set_src(inside_up_down, LVGL_PATH(elevator/inside/arrow_up_Image.png));
    lv_obj_set_pos(inside_up_down, pos_width_conversion(50), pos_height_conversion(80 + 30));

    //60 x 95
    inside_level_1 = lv_img_create(tab);
    lv_img_set_src(inside_level_1, LVGL_PATH(elevator/inside/floor_l_1_Image.png));
    lv_obj_set_pos(inside_level_1, pos_width_conversion(50), pos_height_conversion(80 + 50 + 121));
    inside_level_2 = lv_img_create(tab);
    lv_img_set_src(inside_level_2, LVGL_PATH(elevator/inside/floor_l_1_Image.png));
    lv_obj_set_pos(inside_level_2, pos_width_conversion(120), pos_height_conversion(80 + 50 + 121));
    lv_obj_add_flag(inside_level_2, LV_OBJ_FLAG_HIDDEN);

    //116 x 32
    inside_repari = lv_img_create(tab);
    lv_img_set_src(inside_repari, LVGL_PATH(elevator/inside/repair_Image.png));
    lv_obj_set_pos(inside_repari, pos_width_conversion(50), pos_height_conversion(80 + 80 + 121 + 95 + 80));
}

#else
static void timer_outside_callback(lv_timer_t *tmr)
{
    int x_margin = 20;
    int y_margin = 20;
    char image_str[64];

    if(main_is_up)
        main_level ++;
    else
        main_level --;

    if (main_level > ELEVATOR_MAX_LEVEL) {
        main_is_up = false;
        main_level = ELEVATOR_MAX_LEVEL;
    }
    if (main_level < 1) {
        main_is_up = true;
        main_level = 1;
    }

    if (main_is_up != last_is_up) {
        if (main_is_up)
            lv_img_set_src(outside_up_down, LVGL_PATH(elevator/outside/arrow_up_Image.png));
        else
            lv_img_set_src(outside_up_down, LVGL_PATH(elevator/outside/arrow_down_Image.png));

        last_is_up = main_is_up;
    }

    if (main_level == 12) {
        lv_img_set_src(outside_level_1, LVGL_PATH(elevator/outside/floor_3_12A_Image.png));
        lv_obj_add_flag(outside_level_2, LV_OBJ_FLAG_HIDDEN);
    } else if (main_level == 13) {
        lv_img_set_src(outside_level_1, LVGL_PATH(elevator/outside/floor_3_12B_Image.png));
        lv_obj_add_flag(outside_level_2, LV_OBJ_FLAG_HIDDEN);
    } else if (main_level == 14) {
        lv_img_set_src(outside_level_1, LVGL_PATH(elevator/outside/floor_3_15A_Image.png));
        lv_obj_add_flag(outside_level_2, LV_OBJ_FLAG_HIDDEN);
    } else if (main_level == 15) {
        lv_img_set_src(outside_level_1, LVGL_PATH(elevator/outside/floor_3_15B_Image.png));
        lv_obj_add_flag(outside_level_2, LV_OBJ_FLAG_HIDDEN);
    } else if (main_level == 24) {
        lv_img_set_src(outside_level_1, LVGL_PATH(elevator/outside/floor_3_23A_Image.png));
        lv_obj_add_flag(outside_level_2, LV_OBJ_FLAG_HIDDEN);
    } else if (main_level == 34) {
        lv_img_set_src(outside_level_1, LVGL_PATH(elevator/outside/floor_3_33A_Image.png));
        lv_obj_add_flag(outside_level_2, LV_OBJ_FLAG_HIDDEN);
    } else if(main_level > 0 && main_level < 10) {
        snprintf(image_str, sizeof(image_str), LVGL_PATH(elevator/outside/floor_l_%d_Image.png), main_level);
        lv_img_set_src(outside_level_1, image_str);
        lv_obj_set_pos(outside_level_1, pos_width_conversion((1024 / 2 + x_margin)), pos_height_conversion((600 / 2 - 200 - (228 - 200) /2 + y_margin)));
        lv_obj_add_flag(outside_level_2, LV_OBJ_FLAG_HIDDEN);
    } else if(main_level > 9 && main_level < 20) {
        lv_img_set_src(outside_level_1, LVGL_PATH(elevator/outside/floor_l_1_Image.png));
        snprintf(image_str, sizeof(image_str), LVGL_PATH(elevator/outside/floor_l_%d_Image.png), main_level % 10);
        lv_obj_set_pos(outside_level_1, pos_width_conversion((1024 / 2 - x_margin)), pos_height_conversion((600 / 2 - 200 - (228 - 200) /2 + y_margin)));
        lv_obj_clear_flag(outside_level_2, LV_OBJ_FLAG_HIDDEN);
        lv_img_set_src(outside_level_2, image_str);
    } else if(main_level > 19 && main_level < 30) {
        lv_img_set_src(outside_level_1, LVGL_PATH(elevator/outside/floor_l_2_Image.png));
        snprintf(image_str, sizeof(image_str), LVGL_PATH(elevator/outside/floor_l_%d_Image.png), main_level % 20);
        lv_obj_set_pos(outside_level_1, pos_width_conversion((1024 / 2 - x_margin)), pos_height_conversion((600 / 2 - 200 - (228 - 200) /2 + y_margin)));
        lv_obj_clear_flag(outside_level_2, LV_OBJ_FLAG_HIDDEN);
        lv_img_set_src(outside_level_2, image_str);
    } else if(main_level > 29 && main_level < 40) {
        lv_img_set_src(outside_level_1, LVGL_PATH(elevator/outside/floor_l_3_Image.png));
        snprintf(image_str, sizeof(image_str), LVGL_PATH(elevator/outside/floor_l_%d_Image.png),main_level % 30);
        lv_obj_set_pos(outside_level_1, pos_width_conversion((1024 / 2 - x_margin)), pos_height_conversion((600 / 2 - 200 - (228 - 200) /2 + y_margin)));
        lv_obj_clear_flag(outside_level_2, LV_OBJ_FLAG_HIDDEN);
        lv_img_set_src(outside_level_2, image_str);
    }

    return;
}

void aic_elevator_create_outside(lv_obj_t *tab)
{
    int x_margin = 20;
    int y_margin = 20;

    //220 x 228
    outside_up_down = lv_img_create(tab);
    lv_img_set_src(outside_up_down, LVGL_PATH(elevator/outside/arrow_up_Image.png));
    //lv_obj_align(outside_up_down, LV_ALIGN_CENTER, -70, -30);
    lv_obj_set_pos(outside_up_down, pos_width_conversion((1024 / 2 - 220 - x_margin)), pos_height_conversion((600 / 2 - 228 + y_margin)));

    //126 x 200
    outside_level_1 = lv_img_create(tab);
    lv_img_set_src(outside_level_1, LVGL_PATH(elevator/outside/floor_l_1_Image.png));
    //lv_obj_align(outside_level_1, LV_ALIGN_CENTER, 10, -30);
    lv_obj_set_pos(outside_level_1, pos_width_conversion((1024 / 2 - x_margin)), pos_height_conversion((600 / 2 - 200 - (228 - 200) /2 + y_margin)));

    //126 x 200
    outside_level_2 = lv_img_create(tab);
    lv_img_set_src(outside_level_2, LVGL_PATH(elevator/outside/floor_l_7_Image.png));
    //lv_obj_align(outside_level_2, LV_ALIGN_CENTER, 10 + 60, -30);
    lv_obj_set_pos(outside_level_2, pos_width_conversion((1024 / 2  + 126 - x_margin)), pos_height_conversion((600 / 2 - 200 - (228 - 200) /2 + y_margin)));
    lv_obj_add_flag(outside_level_2, LV_OBJ_FLAG_HIDDEN);

    //118 x 143
    lv_obj_t *overload_image = lv_img_create(tab);
    lv_img_set_src(overload_image, LVGL_PATH(elevator/outside/overload_Image.png));
    lv_obj_align(overload_image, LV_ALIGN_CENTER, pos_width_conversion((- (10 + 118))), pos_height_conversion(100));

    //118 x 143
    lv_obj_t *nosmoke_image = lv_img_create(tab);
    lv_img_set_src(nosmoke_image, LVGL_PATH(elevator/outside/nosmoke_Image.png));
    lv_obj_align(nosmoke_image, LV_ALIGN_CENTER, 0, pos_height_conversion(100));

    //118 x 143
    lv_obj_t *fire_img = lv_img_create(tab);
    lv_img_set_src(fire_img, LVGL_PATH(elevator/outside/fire_Image.png));
    lv_obj_align(fire_img, LV_ALIGN_CENTER, pos_width_conversion((10 + 118)), pos_height_conversion(100));
}
#endif

lv_obj_t * elevator_ui_init()
{
    if(LV_HOR_RES <= 480) disp_size = DISP_SMALL;
    else if(LV_HOR_RES <= 800) disp_size = DISP_MEDIUM;
    else disp_size = DISP_LARGE;

    if (check_lvgl_de_config() == -1) {
        printf("Please check defined AICFB_ARGB8888 and LV_COLOR_DEPTH is 32 or\n");
        printf("check defined AICFB_RGB565 and LV_COLOR_DEPTH is 16\n");
    }

    fb_dev_layer_set();

    main_level = 1;
    main_is_up = true; //0: down, 1: up
    last_is_up = main_is_up;

    lv_obj_t *elevator_ui = lv_obj_create(NULL);
    lv_obj_clear_flag(elevator_ui, LV_OBJ_FLAG_SCROLLABLE);

#if AIC_ELEVATOR_INSIDE
    aic_elevator_create_main(elevator_ui);
    create_player(elevator_ui);
    #ifdef LV_ELEVATOR_UART_COMMAND
        static int elevator_uart_init = 0;
        if (elevator_uart_init == 0) {
            elevator_uart_main();
            elevator_uart_init++;
        }
        elevator_cmd_timer = lv_timer_create(elevator_cmd_cb, 150, 0);
        elevator_inside_timer = lv_timer_create(timer_inside_callback, 500, 0);
    #endif /*LV_ELEVATOR_UART_COMMAND */
#else
    aic_elevator_create_outside(elevator_ui);
    elevator_outsize_timer = lv_timer_create(timer_outside_callback, 500, 0);
#endif /*AIC_ELEVATOR_INSIDE */

    lv_obj_add_event_cb(elevator_ui, ui_elevator_cb, LV_EVENT_ALL, NULL);
    return elevator_ui;
}

static void ui_elevator_cb(lv_event_t *e)
{
    lv_event_code_t code = (lv_event_code_t)lv_event_get_code(e);
    if (code == LV_EVENT_SCREEN_UNLOAD_START) {
#if AIC_ELEVATOR_INSIDE
    #ifdef LV_ELEVATOR_UART_COMMAND
        lv_timer_del(elevator_cmd_timer);
        lv_timer_del(elevator_inside_timer);
    #endif /*LV_ELEVATOR_UART_COMMAND */
#else
        lv_timer_del(elevator_outsize_timer);
#endif /*AIC_ELEVATOR_INSIDE */
        fb_dev_layer_reset();
    }

    if (code == LV_EVENT_DELETE) {;}

    if (code == LV_EVENT_SCREEN_LOADED) {;}
}
