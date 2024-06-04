/*
 * Copyright (c) 2023-2024, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors:  ArtInChip
 */

#include "gif_ui.h"

static void gif_event(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_READY) {;}
}

static void gif_ui_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_SCREEN_UNLOAD_START) {;};
}

lv_obj_t *gif_ui_init(void)
{
    lv_obj_t *gif_ui = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(gif_ui, lv_theme_get_color_primary(NULL), 0);

    lv_obj_t *gif_label = lv_img_create(gif_ui);
    lv_img_set_src(gif_label, LVGL_PATH(gif_demo/gif_label.png));
    lv_obj_set_align(gif_label, LV_ALIGN_TOP_MID);
    lv_obj_set_y(gif_label, LV_VER_RES * 0.05);

    lv_obj_t *img_gif = lv_gif_create(gif_ui);
    lv_gif_set_src(img_gif, LVGL_PATH(gif_demo/bulb.gif));
    lv_obj_center(img_gif);

    lv_obj_add_event_cb(img_gif, gif_event, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(gif_ui, gif_ui_cb, LV_EVENT_ALL, NULL);

    return gif_ui;
}
