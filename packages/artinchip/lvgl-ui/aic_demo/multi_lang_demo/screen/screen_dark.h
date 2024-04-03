/*__cplusplus
 * Copyright (C) 2022-2023 ArtinChip Technology Co., Ltd.
 * Authors:  Ning Fang <ning.fang@artinchip.com>
 */

#ifndef SCREEN_DARK_H
#define SCREEN_DARK_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

extern lv_obj_t *screen_dark;

lv_obj_t *init_screen_dark(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif // SCREEN_DARK_H
