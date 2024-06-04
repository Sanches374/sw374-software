/*
 * Copyright (c) 2023-2024, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors:  ArtInChip
 */

#ifndef _AIC_TEST_DEMO_COMP_NAVIG_ICON_UI_H
#define _AIC_TEST_DEMO_COMP_NAVIG_ICON_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"
#include "aic_ui.h"

lv_obj_t * imgbtn_icon_ui_create(lv_obj_t *parent, const char *img_release_path);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
