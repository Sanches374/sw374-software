/*
 * Copyright (c) 2022-2023, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors:  Zequan Liang <zequan.liang@artinchip.com>
 */

#include "qc_demo.h"
#include "lvgl.h"

extern void lv_qc_test_init(void);

void qc_test_init()
{
    lv_qc_test_init();
}
