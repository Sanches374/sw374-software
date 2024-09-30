/*
 * Copyright (c) 2024-2024, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Chen Junlong <junlong.chen@artinchip.com>
 */

#include <stdio.h>
#include <string.h>
#include <console.h>
#include <aic_seupg.h>
#include <getopt.h>
#include "aic_core.h"
#include "aic_log.h"
#include "hal_mailbox.h"
#include "rpmsg_aic.h"

static u32 g_msg_inited = 0;
static u32 g_mbox_regs = SE_MBOX_SE2CS_BASE;

static void usage(char *program)
{
    printf("Artinchip tinyspl secure upgrade\n");
    printf("\n");
    printf("Usage: \n");
    printf("SESS execute command: %s\n", program);
}

__USED static int ctrlc (void)
{
    switch (getchar()) {
        case 0x03:              /* ^C - Control C */
            return 1;
        default:
            break;
    }

    return 0;
}

static void mbox_recv_cb(u32 *data, u32 len)
{
    mbox_cmd(data, len);
}

static int do_secure_upg(int argc, char *argv[])
{
    if (argc > 1) {
        usage(argv[0]);
        return -1;
    }

    if (!g_msg_inited) {
        aicos_request_irq(MBOX_SE2CS_IRQn, hal_mbox_isr, 0, NULL, &g_mbox_regs);
        hal_mbox_set_cb(mbox_recv_cb);

        hal_mbox_int_all_en(g_mbox_regs, 1);
        g_msg_inited = 1;
    } else {
        printf("Mailbox driver is already inited\n");
    }

    printf("\nWait for msg from master ...\n");
    while (1) {
        if (ctrlc()) {
           aicos_irq_disable(MBOX_SE2CS_IRQn);
           break;
        }
    }

#ifdef AIC_MAILBOX_DRV_DEBUG
    hal_mbox_dbg_mode(g_mbox_regs);
#endif

    return 0;
}

CONSOLE_CMD(aic_secure_upg, do_secure_upg, "Upgrading mode command.");
