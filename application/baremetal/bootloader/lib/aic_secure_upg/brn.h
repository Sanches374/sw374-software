/*
 * Copyright (c) 2024, ArtInChip Technology CO.,LTD. All Rights Reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Author: Chen JunLong <junlong.chen@artinchip.com>
 */

#ifndef _BRN_H_
#define _BRN_H_

#include "mbox_mode.h"

enum brn_action {
	BRN_TYPE_WRITE = 0,
	BRN_TYPE_EXECUTE,
	BRN_TYPE_READ,
	BRN_TYPE_PARSE,
	BRN_TYPE_EFUSE_BITWRITE,
	BRN_TYPE_EFUSE_BITREAD,
	BRN_TYPE_WRITE_SECURE,
	BRN_TYPE_EFUSE_BITWRITE_SECURE,
	BRN_TYPE_EFUSE_WRITE_SECURE,
	BRN_TYPE_UNKNOWN,
};

enum brn_storage_type {
	BRN_STORAGE_RAM = 0,
	BRN_STORAGE_SPINOR,
	BRN_STORAGE_SPINAND,
	BRN_STORAGE_MMC,
	BRN_STORAGE_EFUSE,
	BRN_STORAGE_INVALID,
};

#define BRN_STORAGE_EFUSE_CSYS 0
#define BRN_STORAGE_EFUSE_SESS 1

#define BRN_PRINT_CPU_ID 0
#define BRN_MAIN_CORE_CPU_ID 1
#define BRN_SCAN_CPU_ID 2
#define BRN_SECURE_CPU_ID 3

struct brn_header {
	u8 magic[2];
	u8 action;
	u8 cpu_id;
	u8 secure_flag;
	u8 headcksum; /* 8bit checksum of first 20byte */
	u8 storage_type;
	u8 storage_index;
	u32 storage_address;
	u32 execute_address;
	u32 length;
	u32 checksum; /* 32bit checksum from file start to end */
};

struct brn_state {
	struct brn_header head;
	int recv_len;
	int secure_enable;
	u32 checksum;
};

u8 brn_head_cksum(u8 *head);
s32 brn_image_sign_verify(u8 *head, u32 hlen, u8 *brn_data);
s32 brn_image_sm4_ecb_decrypt(u8 *head, u32 hlen, u8 *brn_data);

u32 nand_data_align_write(struct brn_state *brn, u8 *buf, u32 len);
s32 nor_data_write(u32 addr, u8 *buf, s32 len);
s32 nand_start(struct brn_state *brn);
u32 nor_start(struct brn_state *brn);
void nor_data_end(void);
void nand_data_end(void);

#endif /* _BRN_H_ */
