/*
 * Copyright (c) 2022-2023, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors:  Zequan Liang <zequan.liang@artinchip.com>
 */

#ifndef _AIC_A_86BOX_DEMO_COMP_NAVIG_ICON_UI_H
#define _AIC_A_86BOX_DEMO_COMP_NAVIG_ICON_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"
#include "aic_ui.h"

lv_obj_t *com_imgbtn_switch_comp(lv_obj_t *parent, void *img_src_0, void *img_src_1);
lv_obj_t *com_imgbtn_comp(lv_obj_t *parent, void *img_src, void *pressed_img_src);
lv_obj_t *ft_label_comp(lv_obj_t *parent, uint16_t weight, uint16_t style, const char *path);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
