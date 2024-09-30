/*
 * Copyright (c) 2024, ArtInChip Technology CO.,LTD. All Rights Reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Author: Chen JunLong <junlong.chen@artinchip.com>
 */

#include "mbox_mode.h"
#include <efuse.h>

/* One WORD is 32bit */
#define EFUSE_OFFSET_BROM_CFG_WID     8
#define EFUSE_OFFSET_SECURE_CFG_WID   9
#define SECURE_EFUSE_MAX_WID          128

#define EFUSE_SM2_PK_WLEN 16
#define EFUSE_SM4_KEY_WLEN 4
#define KEY_INDEX_FOR_MAILBOX 0
#define KEY_INDEX_FOR_BOOT    1

#define SM2_PK_START_WID_0 16
#define SM2_PK_START_WID_1 32
#define SM4_KEY_START_WID_0 48
#define SM4_KEY_START_WID_1 52

/* WARN: If position of secure cfg is changed, please double check the secure_wakeup_check in start.S */
union secure_cfg {
	u32 val;
	struct secure_cfg_bits {
		u32 secure_boot_en		: 1; /* [0]   : Secure boot enable */
		u32 secure_unrecoverable_en	: 1; /* [1]   : Secure unrecoverable */
		u32 secure_sign_en		: 1; /* [2]   : Signature boot enable */
		u32 secure_encrypt_en		: 1; /* [3]   : Encrypt boot enable */
		u32 spi_enc_en          	: 1; /* [4]   : SPIENC enable */
		u32 sram_enc_en          	: 1; /* [5]   : SRAM encrypt enable */
		u32 bus_srm_dis          	: 1; /* [6]   : Bus scramble disable */
		u32 r1				: 25;
	} bits;
};

u32 efuse_read_u32(u32 wid)
{
    u32 data, ret;
    /* make sure the size is 4? */
    ret = efuse_read(wid, &data, sizeof(data));
    if (ret)
        return data;
    else
        return -1;
}

s32 efuse_get_secure_boot_flag(void)
{
    union secure_cfg secure_cfg;
	secure_cfg.val = efuse_read_u32(EFUSE_OFFSET_SECURE_CFG_WID);
	return (secure_cfg.bits.secure_boot_en);
}

s32 mbox_cmd_efuse_wr(void *head, u32 hlen, u32 daddr, u32 dlen)
{
	return 0;
}

s32 efuse_get_sm2_pubkey(u8 index, u8 *key)
{
	u32 i, efusekey[EFUSE_SM2_PK_WLEN];

	memset(efusekey, 0, EFUSE_SM2_PK_WLEN * 4);
	if (index == KEY_INDEX_FOR_MAILBOX) {
		for (i = 0; i < EFUSE_SM2_PK_WLEN; i++) {
			efusekey[i] = efuse_read_u32(SM2_PK_START_WID_0 + i);
		}
	}
	if (index == KEY_INDEX_FOR_BOOT) {
		for (i = 0; i < EFUSE_SM2_PK_WLEN; i++) {
			efusekey[i] = efuse_read_u32(SM2_PK_START_WID_1 + i);
		}
	}
	key[0] = 0x04;
	memcpy(key + 1, efusekey, 64);
	return 0;
}
