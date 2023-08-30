/*
 * Copyright (c) 2023, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors: matteo <duanmt@artinchip.com>
 */

#ifndef _ARTINCHIP_HAL_WRI_H_
#define _ARTINCHIP_HAL_WRI_H_

#include "aic_common.h"

enum aic_warm_reset_type {
    WRI_TYPE_POR = 0,
    WRI_TYPE_RTC,
    WRI_TYPE_EXT,
    WRI_TYPE_DM,
    WRI_TYPE_WDT,
    WRI_TYPE_TSEN,
    WRI_TYPE_CMP,
    WRI_TYPE_MAX
};

enum aic_warm_reset_type aic_wr_type_get(void);
enum aic_reboot_reason aic_judge_reboot_reason(enum aic_warm_reset_type hw,
                                               u32 sw);

#endif
