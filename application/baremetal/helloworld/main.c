/*
 * Copyright (c) 2022, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors: weilin.peng@artinchip.com
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <board.h>
#include <aic_core.h>
#include <aic_drv_bare.h>
#include <artinchip_fb.h>
#include "aic_hal_ve.h"
#include "aic_reboot_reason.h"
#ifdef AIC_DMA_DRV
#include "drv_dma.h"
#endif

#ifdef LPKG_USING_DFS
#include <dfs.h>
#include <dfs_fs.h>
#ifdef LPKG_USING_DFS_ELMFAT
#include <dfs_elm.h>
#endif
#endif

#ifdef AIC_SDMC_DRV
#include "mmc.h"
#endif

void show_banner(void)
{
    printf("%s\n", BANNER);
    printf("Welcome to ArtInChip Luban-Lite %d.%d [Baremetal - Built on %s %s]\n",
               LL_VERSION, LL_SUBVERSION, __DATE__, __TIME__);
}

static int board_init(void)
{
    int cons_uart;

    aic_hw_board_init();
    aicos_local_irq_enable();

    cons_uart = AIC_BAREMETAL_CONSOLE_UART;
    uart_init(cons_uart);
    stdio_set_uart(cons_uart);

    show_banner();

    return 0;
}

int main(void)
{
    board_init();

#ifdef AIC_DMA_DRV
    drv_dma_init();
#endif

#ifdef TLSF_MEM_HEAP
    aic_tlsf_heap_test();
#endif

#ifdef LPKG_USING_DFS
    dfs_init();
#ifdef LPKG_USING_DFS_ELMFAT
    elm_init();
#endif
#endif

#ifdef AIC_SDMC_DRV
    mmc_init(1);
#endif
#if defined(LPKG_USING_DFS_ELMFAT) && defined(AIC_SDMC_DRV)
    if (dfs_mount("sdmc", "/", "elm", 0, 0) < 0)
        pr_err("Failed to mount sdmc with FatFS\n");
#endif

#ifdef AIC_DISPLAY_DRV
    aicfb_probe();
#endif

#ifdef AIC_VE_DRV
    hal_ve_probe();
#endif
    aic_get_reboot_reason();
    aic_show_startup_time();

    #ifdef AIC_VE_TEST
    extern int  do_pic_dec_test(int argc, char **argv);
    /* Main loop */
    aicos_mdelay(2000);
    while (1) {
        do_pic_dec_test(0,NULL);
        //break;
    }
    #else
    while (1) {
        break;
    }
    #endif

#ifdef AIC_CONSOLE_BARE_DRV
    /* Console shell loop */
    console_init();
    console_loop();
#endif

    return 0;
}
