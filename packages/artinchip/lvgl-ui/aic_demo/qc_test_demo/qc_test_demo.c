/*
 * Copyright (c) 2023-2024, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors:  ArtInChip
 */
#include <rtconfig.h>

extern void lv_qc_test_init();

void qc_test_init(void)
{
#ifdef KERNEL_RTTHREAD
    lv_qc_test_init();
#endif
}
