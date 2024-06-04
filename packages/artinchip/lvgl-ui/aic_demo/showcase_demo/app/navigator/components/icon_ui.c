/*
 * Copyright (c) 2023-2024, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors:  ArtInChip
 */

#include "icon_ui.h"
#include "string.h"

typedef enum {
    DISP_SMALL,
    DISP_MEDIUM,
    DISP_LARGE,
} disp_size_t;

lv_obj_t * imgbtn_icon_ui_create(lv_obj_t *parent, const char *img_release_path)
{
    static lv_style_t icon_style_pressed_default;
    static disp_size_t disp_size = 0;
    int offset_size = 2;

    if(LV_HOR_RES <= 480) disp_size = DISP_SMALL;
    else if(LV_HOR_RES <= 800) disp_size = DISP_MEDIUM;
    else disp_size = DISP_LARGE;

    (void)disp_size;
    lv_obj_t *app_icon = lv_img_create(parent);
    lv_img_set_src(app_icon, img_release_path);
    lv_obj_add_flag(app_icon, LV_OBJ_FLAG_CLICKABLE);

    /* reduce the click area and minimize accidental touches */
    if (disp_size == DISP_SMALL)
        lv_obj_set_ext_click_area(app_icon, -15);

    if (icon_style_pressed_default.prop_cnt > 1) {
        lv_style_reset(&icon_style_pressed_default);
    } else {
        lv_style_init(&icon_style_pressed_default);
    }

    lv_style_set_translate_x(&icon_style_pressed_default, offset_size);
    lv_style_set_translate_y(&icon_style_pressed_default, offset_size);

    lv_obj_add_style(app_icon, &icon_style_pressed_default, LV_STATE_PRESSED);

    return app_icon;
}
