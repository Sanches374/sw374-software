/*
 * Copyright (c) 2024, ArtInChip Technology CO.,LTD. All Rights Reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Author: Chen JunLong <junlong.chen@artinchip.com>
 */

#ifndef _MBOX_MODE_H_
#define _MBOX_MODE_H_

#include <aic_core.h>

#define AIC_MBOX_CMD_DEF(c, n)	((c) << 8 | (n))

#define AIC_MBOX_CMD_GET_SECURE_STATUS		AIC_MBOX_CMD_DEF('e', 0)
#define AIC_MBOX_CMD_GET_SECURE_STATUS_RESULT	AIC_MBOX_CMD_DEF('e', 1)
#define AIC_MBOX_CMD_WRITE_DATA			AIC_MBOX_CMD_DEF('e', 2)
#define AIC_MBOX_CMD_WRITE_DATA_RESULT		AIC_MBOX_CMD_DEF('e', 3)
#define AIC_MBOX_CMD_EXECUTE			AIC_MBOX_CMD_DEF('e', 4)
#define AIC_MBOX_CMD_VERIFY_DATA		AIC_MBOX_CMD_DEF('e', 5)
#define AIC_MBOX_CMD_VERIFY_DATA_RESULT		AIC_MBOX_CMD_DEF('e', 6)
#define AIC_MBOX_CMD_EFUSE_WR			AIC_MBOX_CMD_DEF('e', 7)
#define AIC_MBOX_CMD_EFUSE_WR_RESULT		AIC_MBOX_CMD_DEF('e', 8)
#define AIC_MBOX_CMD_STANDBY			AIC_MBOX_CMD_DEF('e', 9)
#define AIC_MBOX_CMD_RESUME			AIC_MBOX_CMD_DEF('e', 10)

struct aic_mbox_cmd_head {
	u16 cmd;
	u8 seq;
	u8 len; /* length of data, count in 32-bit word */
};

struct aic_mbox_cmd {
	struct aic_mbox_cmd_head head;
	u32 data[];
};

/*
 * Direction:
 * Normal Boot -> Secure  Boot
 */
struct aic_mbox_cmd_get_secure_status {
	struct aic_mbox_cmd_head head;
};

/*
 * Direction:
 * Secure Boot -> Normal Boot
 */
struct aic_mbox_cmd_get_secure_status_result {
	struct aic_mbox_cmd_head head;
	u32 result;
};

/*
 * Direction:
 * Normal Boot -> Secure  Boot
 */
struct aic_mbox_cmd_write_data {
	struct aic_mbox_cmd_head head;
	u32 src;
	u32 dst;
	u32 len;
};

/*
 * Direction:
 * Secure Boot -> Normal Boot
 */
struct aic_mbox_cmd_write_data_result {
	struct aic_mbox_cmd_head head;
	u32 result;
};

/*
 * Direction:
 * Normal Boot -> Secure  Boot
 * Pass full image to secure CPU
 */
struct aic_mbox_cmd_exec {
	struct aic_mbox_cmd_head head;
	u32 fw_addr;
	u32 fw_param;
	u32 brn_hlen;
	u32 brn_head[8]; /* 24 bytes */
};

/*
 * Direction:
 * Normal Boot -> Secure  Boot
 */
struct aic_mbox_cmd_verify_data {
	struct aic_mbox_cmd_head head;
	u32 data_addr;
	u32 data_len;
	u32 sign_addr;
	u32 sign_len;
	u32 brn_hlen;
	u32 brn_head[8]; /* 24 bytes */
};

/*
 * Direction:
 * Secure Boot -> Normal Boot
 */
struct aic_mbox_cmd_verify_data_result {
	struct aic_mbox_cmd_head head;
	u32 result;
};

/*
 * Full BRN file
 */
struct aic_mbox_cmd_efuse_wr {
	struct aic_mbox_cmd_head head;
	u32 data_addr;
	u32 data_len;
	u32 brn_hlen;
	u32 brn_head[8]; /* 24 bytes */
};

struct aic_mbox_cmd_efuse_wr_result {
	struct aic_mbox_cmd_head head;
	u32 result;
};

struct aic_mbox_cmd_standby {
	struct aic_mbox_cmd_head head;
	u32 delay;
};

struct aic_mbox_cmd_resume {
	struct aic_mbox_cmd_head head;
	u32 param0;
};

struct aic_mbox_rpmsg {
    u16 cmd;
    u8  seq;
    u8  len;    /* length of data[], unit: dword */
    u32 data[]; /* length varies according to the scene */
};

s32 efuse_get_secure_boot_flag(void);
s32 mbox_cmd_write_data(u32 src, u32 dst, u32 len);
void mbox_cmd_exec(void *head, u32 hlen, u32 fw_addr, u32 param);
s32 mbox_cmd_verify(void *head, u32 hlen, u32 daddr, u32 dlen, u32 saddr,
    u32 slen);
s32 mbox_cmd_efuse_wr(void *head, u32 hlen, u32 daddr, u32 dlen);
void mbox_cmd_standby(u32 delay);
void mbox_cmd_resume(u32 delay);

s32 efuse_get_sm2_pubkey(u8 index, u8 *key);

#endif
