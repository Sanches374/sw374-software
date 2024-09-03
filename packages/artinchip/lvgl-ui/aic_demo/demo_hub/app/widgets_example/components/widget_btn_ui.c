/*
 * Copyright (c) 2023-2024, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors:  ArtInChip
 */

#include "widget_btn_ui.h"
#include "demo_hub.h"

lv_obj_t * widget_btn_ui_create(lv_obj_t *parent)
{
    lv_obj_t *btn = lv_btn_create(parent);
    lv_obj_set_size(btn, LV_HOR_RES * 0.4, LV_VER_RES * 0.12);
    lv_obj_set_style_outline_opa(btn, LV_OPA_100, 0);

    int out_line_width = lv_obj_get_height(btn) * 0.1;
    out_line_width = out_line_width == 0 ? 1 : out_line_width;

    lv_obj_set_style_outline_width(btn, out_line_width, 0);
    lv_obj_set_style_outline_color(btn, lv_theme_get_color_emphasize(NULL), 0);
    lv_obj_set_style_bg_color(btn, lv_theme_get_color_secondary(NULL), 0);
    lv_obj_set_style_bg_opa(btn, LV_OPA_100, 0);
    lv_obj_set_style_radius(btn, 8, 0);
    return btn;
}

