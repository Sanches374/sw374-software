/*
 * Copyright (c) 2024, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors:  Huahui Mai <huahui.mai@artinchip.com>
 */

#ifndef LV_DEMO_USB_VIDEO_H
#define LV_DEMO_USB_VIDEO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"
#include "aic_ui.h"

lv_obj_t * lv_video_screen_creat(void);

void lv_video_play(void);

void lv_video_stop(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_DEMO_USB_VIDEO_H*/
