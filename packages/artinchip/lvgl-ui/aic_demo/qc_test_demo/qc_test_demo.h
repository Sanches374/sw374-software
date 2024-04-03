/*
 * Copyright (c) 2023-2024, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors:  ArtInChip
 */

#include "lvgl.h"
#include "aic_ui.h"

#define DEBUG_QC

/* Chip model selection */
#define D13B
#define D13C
#define D13E

#define G73X
#define G73X
#define G73X

#define D12B
#define D12C
#define D12E

LV_FONT_DECLARE(lv_font_montserrat_24)
LV_FONT_DECLARE(lv_font_montserrat_48)

void qc_test_init(void);
