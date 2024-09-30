/*
 * Copyright (c) 2024, ArtInChip Technology CO.,LTD. All Rights Reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Author: Chen JunLong <junlong.chen@artinchip.com>
 */

#include <aic_seupg.h>
#include "mbox_reg.h"
#include "mbox_mode.h"
#include "hal_mailbox.h"
#include "rpmsg_aic.h"

#define MBOX_OK  0
#define MBOX_ERR -1
#define MBOX_TMO -2
#define MBOX_BUSY -3
#define MBOX_UNIT_CNT(cmd) (sizeof((cmd)) / 4)
#define MBOX_CMD_DLEN(cmd) ((sizeof((cmd)) / 4) - 1)
#define MBOX_1s 1000000
#define MBOX_1ms 1000
#define MBOX_DELAY_US 1

static u32 g_mbox_regs = SE_MBOX_SE2CS_BASE;

static s32 _write_response_to_csys(u32 *g_send_data)
{
    u32 ret = 0;
    struct aic_mbox_rpmsg *msg = (struct aic_mbox_rpmsg *)g_send_data;
    if (!g_send_data) {
        pr_err("g_send_data is null\n");
        return -1;
    }

    ret = hal_mbox_wr(g_mbox_regs, g_send_data, msg->len);
    if (ret < 0) {
        pr_err("Failed to send response\n");
        return -1;
    }

    hal_mbox_wr_cmp(g_mbox_regs, g_send_data[msg->len]);
    return 0;
}


int mbox_cmd(u32 *data, u32 len)
{
    struct aic_mbox_cmd cmd;
    struct aic_mbox_cmd_write_data *wr;
    struct aic_mbox_cmd_exec *exec;
    struct aic_mbox_cmd_verify_data *ver;
    struct aic_mbox_cmd_efuse_wr *efuse;
    struct aic_mbox_cmd_standby *standby;
    struct aic_mbox_cmd_resume *resume;
    struct aic_mbox_cmd_get_secure_status_result resp_status;
    struct aic_mbox_cmd_write_data_result resp_write;
    struct aic_mbox_cmd_verify_data_result resp_verify;

    u32 cmd_buf[MB_FIFO_DEPTH], *p, hlen;
    int ret = 0;
    if (!data) {
        pr_err("Error, cmd is null.\n");
        return -1;
    }
    memset(&cmd, 0, len);
    memcpy(&cmd, data, len);
    p = cmd_buf;
    memset(p, 0, MB_FIFO_DEPTH * 4);
    switch (cmd.head.cmd) {
    case AIC_MBOX_CMD_GET_SECURE_STATUS:
        p = (void *)&resp_status;
        memset(p, 0, sizeof(resp_status));
        resp_status.head.cmd = AIC_MBOX_CMD_GET_SECURE_STATUS_RESULT;
        resp_status.head.len = MBOX_CMD_DLEN(resp_status);
        resp_status.result = efuse_get_secure_boot_flag();
        ret = _write_response_to_csys(p);
        pr_err("AIC_MBOX_CMD_GET_SECURE_STATUS: %d\n", resp_status.result);
        if (ret)
            pr_err("Failed to write mbox response\n");
        break;

    case AIC_MBOX_CMD_WRITE_DATA:
        wr = (void *)data;
        ret = mbox_cmd_write_data(wr->src, wr->dst, wr->len);
        p = (void *)&resp_write;
        memset(p, 0, sizeof(resp_write));
        resp_write.head.cmd = AIC_MBOX_CMD_WRITE_DATA_RESULT;
        resp_write.head.len = MBOX_CMD_DLEN(resp_write);
        if (ret)
            resp_write.result = 0;
        else
            resp_write.result = 1;
        ret = _write_response_to_csys(p);
        if (ret)
            pr_err("Failed to write mbox response\n");
        break;

    case AIC_MBOX_CMD_EXECUTE:
        exec = (void *)data;
        hlen = exec->brn_hlen;
        mbox_cmd_exec(exec->brn_head, hlen, exec->fw_addr,
            exec->fw_param);
        break;

    case AIC_MBOX_CMD_VERIFY_DATA:
        ver = (void *)data;
        p = (void *)&resp_verify;
        memset(p, 0, sizeof(resp_verify));
        resp_verify.head.cmd = AIC_MBOX_CMD_VERIFY_DATA_RESULT;
        resp_verify.head.len = MBOX_CMD_DLEN(resp_verify);
        hlen = ver->brn_hlen;
        ret = mbox_cmd_verify(ver->brn_head, hlen,
            ver->data_addr, ver->data_len,
            ver->sign_addr, ver->sign_len);
        if (ret == 1)
            resp_verify.result = 1;
        ret = _write_response_to_csys(p);
        if (ret)
            pr_err("Failed to write mbox response\n");
        break;

    case AIC_MBOX_CMD_EFUSE_WR:
        efuse = (void *)data;
        hlen = efuse->brn_hlen;
        ret = mbox_cmd_efuse_wr(efuse->brn_head, hlen,
            efuse->data_addr,
            efuse->data_len);
        struct aic_mbox_cmd_efuse_wr_result resp_efuse;
        p = (void *)&resp_efuse;
        memset(p, 0, sizeof(resp_efuse));
        resp_efuse.head.cmd = AIC_MBOX_CMD_EFUSE_WR_RESULT;
        resp_efuse.head.len = MBOX_CMD_DLEN(resp_efuse);
        if (ret)
            resp_efuse.result = 0;
        else
            resp_efuse.result = 1;
        ret = _write_response_to_csys(p);
        if (ret)
            pr_err("Failed to write mbox response\n");
        break;

    case AIC_MBOX_CMD_STANDBY:
        standby = (void *)data;
        mbox_cmd_standby(standby->delay);
        break;

    case AIC_MBOX_CMD_RESUME:
        resume = (void *)data;
        mbox_cmd_resume(resume->param0);
        break;
    default:
        pr_err("Cmd is error %d\n", cmd.head.cmd);
        break;
    }

    return 0;
}
