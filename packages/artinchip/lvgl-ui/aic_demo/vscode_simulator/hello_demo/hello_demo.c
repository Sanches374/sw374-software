/*
 * Copyright (c) 2022-2023, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors:  Zequan Liang <zequan.liang@artinchip.com>
 */

#include "hello_demo.h"
#include "lvgl.h"

void hello_ui_init()
{
    lv_obj_t *hello_world = lv_img_create(lv_scr_act());
    lv_img_set_src(hello_world, LVGL_PATH(hello_demo/hello_world.png));
}
