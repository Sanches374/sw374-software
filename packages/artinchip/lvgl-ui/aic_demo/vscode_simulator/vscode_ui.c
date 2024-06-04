/*
 * Copyright (c) 2022-2023, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors:  Zequan Liang <zequan.liang@artinchip.com>
 */

#include "aic_ui.h"

#define USE_HELLO_DEMO

void vscode_ui_init(void)
{
#ifdef USE_SLIDE_DEMO
    extern void slide_ui_init();
	slide_ui_init();
#endif

#ifdef USE_HELLO_DEMO
    extern void hello_ui_init();
    hello_ui_init();
#endif
}
