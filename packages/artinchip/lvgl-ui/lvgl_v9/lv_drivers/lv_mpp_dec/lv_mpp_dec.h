/*
 * Copyright (c) 2024, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors:  Ning Fang <ning.fang@artinchip.com>
 */

#ifndef LV_MPP_DEC_H
#define LV_MPP_DEC_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef LV_USE_MPP_DEC
#define LV_USE_MPP_DEC 1
#endif

#ifndef LV_CACHE_IMG_NUM_LIMIT
#define LV_CACHE_IMG_NUM_LIMIT 1
#endif

#ifndef LV_CACHE_IMG_NUM
#define LV_CACHE_IMG_NUM 8
#endif

void lv_mpp_dec_init(void);

void lv_mpp_dec_deinit(void);

bool lv_drop_one_cached_image();

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif //LV_MPP_DEC_H
