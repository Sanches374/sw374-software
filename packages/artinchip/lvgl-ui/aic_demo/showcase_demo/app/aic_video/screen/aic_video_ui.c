/*
 * Copyright (c) 2023-2024, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors:  ArtInChip
 */

#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "lvgl.h"
#include "aic_ui.h"
#include "lv_port_disp.h"
#include "mpp_fb.h"
#include "aic_lv_ffmpeg.h"

static void fb_dev_layer_set(void);
static int check_lvgl_de_config(void);

static void ui_aic_video_cb(lv_event_t *e);
static void get_play_timer_cb(lv_timer_t *timer);

static lv_timer_t *play_timer;
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
#if defined(AICFB_RGB565)
    struct mpp_fb *fb = mpp_fb_open();
    struct aicfb_alpha_config alpha = {0};
    alpha.layer_id = 1;
    alpha.enable = 1;
    alpha.mode = 1;
    alpha.value = 255 / 2;
    mpp_fb_ioctl(fb, AICFB_UPDATE_ALPHA_CONFIG, &alpha);
    mpp_fb_close(fb);
#endif
}

static void fb_dev_layer_reset(void)
{
#if defined(AICFB_RGB565)
    struct mpp_fb *fb = mpp_fb_open();

    struct aicfb_alpha_config alpha = {0};
    alpha.layer_id = 1;
    alpha.enable = 1;

    alpha.mode = 0;
    alpha.value = 255;

    mpp_fb_ioctl(fb, AICFB_UPDATE_ALPHA_CONFIG, &alpha);
    mpp_fb_close(fb);
#endif
}

lv_obj_t *aic_video_ui_init(void)
{
    static lv_obj_t *aic_video_ui;

    aic_video_ui = lv_obj_create(NULL);

    if (check_lvgl_de_config() == -1) {
        printf("Please check defined AICFB_ARGB8888 and LV_COLOR_DEPTH is 32 or\n");
        printf("check defined AICFB_RGB565 and LV_COLOR_DEPTH is 16\n");
        return aic_video_ui;
    }

    fb_dev_layer_set();

    lv_obj_t *player = lv_ffmpeg_player_create(aic_video_ui);
    lv_obj_set_size(player, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_pos(player, 0, 0);
    lv_obj_set_align(player, LV_ALIGN_CENTER);
    lv_ffmpeg_player_set_auto_restart(player, true);
    lv_ffmpeg_player_set_src(player, "rodata/lvgl_data/common/elevator_mjpg.mp4");
    lv_ffmpeg_player_set_cmd(player, LV_FFMPEG_PLAYER_CMD_START);
    // lv_ffmpeg_player_set_cmd_ex(player, LV_FFMPEG_PLAYER_CMD_START, NULL);

    lv_obj_t *label = lv_label_create(aic_video_ui);
    lv_obj_set_align(label, LV_ALIGN_BOTTOM_LEFT);
    lv_obj_set_size(label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_color(label, lv_color_hex(0x0), 0);
    lv_obj_set_style_bg_opa(label, LV_OPA_100, 0);
    lv_label_set_text(label, "time ms: 00");

    play_timer = lv_timer_create(get_play_timer_cb, 500, aic_video_ui);

    lv_obj_add_event_cb(aic_video_ui, ui_aic_video_cb, LV_EVENT_ALL, NULL);

    return aic_video_ui;
}

static void ui_aic_video_cb(lv_event_t *e)
{
    lv_event_code_t code = (lv_event_code_t)lv_event_get_code(e);
    if (code == LV_EVENT_SCREEN_UNLOAD_START) {
        fb_dev_layer_reset();
        lv_timer_del(play_timer);
    }
    if (code == LV_EVENT_DELETE) {;}
    if (code == LV_EVENT_SCREEN_LOADED) {;}
}

static void get_play_timer_cb(lv_timer_t *timer)
{
    u64 seek;
    lv_obj_t *scr = timer->user_data;
    lv_obj_t *player = lv_obj_get_child(scr, 0);
    lv_obj_t *label = lv_obj_get_child(scr, 1);

    lv_ffmpeg_player_set_cmd_ex(player, LV_FFMPEG_PLAYER_CMD_GET_PLAY_TIME_EX, &seek);
    lv_label_set_text_fmt(label, "time ms: %d", (int)(seek / 1000));
}
