/*
 * Copyright (c) 2023, Artinchip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Wu Dehuang <dehuang.wu@artinchip.com>
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <image.h>
#include <boot.h>
#include <aic_core.h>
#include <aic_time.h>
#include <boot_time.h>
#include <console.h>

void boot_app(void *app)
{
    int ret;
    void (*ep)(void);

    ret = console_get_ctrlc();
    if (ret > 0)
        return;

    ep = image_get_entry_point(app);
    if (!ep) {
        printf("Entry point is null.\n");
        while(1);
    }
    boot_time_trace("Run APP");
    boot_time_show();
    aicos_dcache_clean();
    ep();
}
