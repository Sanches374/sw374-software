
/*
 * Copyright (c) 2023-2024, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors:  ArtInChip
 */

#include "sys_icon_ui.h"

lv_obj_t *home_icon_ui_create(lv_obj_t *parent)
{
    lv_obj_t *home_btn = lv_imgbtn_create(parent);
    lv_obj_move_foreground(home_btn);
    lv_obj_add_flag(home_btn, LV_OBJ_FLAG_FLOATING);
    lv_imgbtn_set_src(home_btn, LV_IMGBTN_STATE_RELEASED, NULL, LVGL_PATH(common/home_released.png), NULL);
    lv_imgbtn_set_src(home_btn, LV_IMGBTN_STATE_PRESSED, NULL, LVGL_PATH(common/home_pressed.png), NULL);
    lv_obj_set_align(home_btn, LV_ALIGN_TOP_LEFT);
    lv_obj_set_size(home_btn, 27, 27);

    lv_obj_set_pos(home_btn, LV_HOR_RES * 0.1, LV_VER_RES / 2);
    return home_btn;
}
