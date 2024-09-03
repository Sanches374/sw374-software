/*
 * Copyright (c) 2022-2023, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */
#include "aic_core.h"
#include "console.h"
#include "hal_wdt.h"
#include <wdt.h>
#include "hal_mailbox.h"
#include "rpmsg_aic.h"

extern void aic_se_suspend_and_resume(void);

static char g_name[8] = "sess";
static u32 g_mbox_regs = SE_MBOX_SE2CS_BASE;
static volatile u32 check_idle = 0;
static volatile u32 standby_request = 0;
static u32 g_send_data = 0;

static void pm_mbox_recv_cb(u32 *data, u32 len)
{
    struct aic_rpmsg *msg = (struct aic_rpmsg *)data;

    data[len] = 0;
    printf("\n%s: Recv cmd 0x%x, seq %d, data len %d Word\n",
           g_name, msg->cmd, msg->seq, msg->len);

    if (msg->cmd == RPMSG_CMD_IS_IDLE)
        check_idle = 1;
    if (msg->cmd == RPMSG_CMD_PRE_STANDBY)
        standby_request = 1;
}

static void pm_mbox_send_msg(u16 cmd)
{
    struct aic_rpmsg msg;

    memset(&msg, 0, sizeof(struct aic_rpmsg));
    msg.cmd = cmd;
    memcpy(&g_send_data, &msg, 4);

    printf("%s: Resp cmd 0x%x, seq %d, data len %d Word",
           g_name, msg.cmd, msg.seq, msg.len);

    hal_mbox_wr_cmp(g_mbox_regs, g_send_data);
}

static int cmd_pm_mbox(int argc, char **argv)
{
    aicos_request_irq(MBOX_SE2CS_IRQn, hal_mbox_isr, 0, NULL, &g_mbox_regs);
    hal_mbox_set_cb(pm_mbox_recv_cb);

    hal_mbox_int_all_en(g_mbox_regs, 1);

    printf("\nWait for msg from master ...\n");

    while (!check_idle)
    {}

    pm_mbox_send_msg(RPMSG_CMD_ACK);

    while (!standby_request)
    {}

    check_idle = 0;
    standby_request = 0;

    //Init watchdog
    wdt_init();
    //Only reset SE CPU
    hal_wdt_rst_type_set(1);
    //SE watchdog reset SESS after 5 seconds
    wdt_start(5000);

    //enter suspend
    aic_se_suspend_and_resume();

    printf("sess resumed\n");
    wdt_deinit();
    return 0;
}
CONSOLE_CMD(pm_mbox, cmd_pm_mbox, "SE suspend/resume test case");
