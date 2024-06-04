/*
 * Copyright (c) 2023-2024, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors:  ArtInChip
 */

#ifndef _AIC_TEST_DEMO_COMP_COM_LABEL_UI_H
#define _AIC_TEST_DEMO_COMP_COM_LABEL_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"
#include "aic_ui.h"

lv_obj_t *commom_label_ui_create(lv_obj_t *parent, int font_size, int font_type);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
