/*
 * Copyright (c) 2023-2024, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors:  ArtInChip
 */

#ifndef _AIC_LV_SHOWCASE_DEMO_H
#define _AIC_LV_SHOWCASE_DEMO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"
#include "aic_ui.h"

typedef struct _layer_sys_ui {
    lv_obj_t *home_btn;
} layer_sys_ui;

/* expand an interface for obtaining colors */
lv_color_t lv_theme_get_color_emphasize(lv_obj_t * obj);

void layer_sys_ui_visible(int flag);
void layer_sys_ui_invisible(int flag);

void showcase_demo_init(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
