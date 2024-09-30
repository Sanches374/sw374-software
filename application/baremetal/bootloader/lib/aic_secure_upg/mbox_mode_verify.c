/*
 * Copyright (c) 2024, ArtInChip Technology CO.,LTD. All Rights Reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Author: Chen JunLong <junlong.chen@artinchip.com>
 */
#include "mbox_mode.h"
#include <osrce.h>
#include <aic_utils.h>
#include <hash.h>
#include <sm2.h>

#define VERIFY_FAIL 0
#define VERIFY_PASS 1

#define SM3_BYTE_LEN 32
#define SM2_PUBKEY_LEN 65
#define KEY_INDEX_FOR_MAILBOX 0

#define HEXDUMP_GROUPSIZE 8

static u32 osr_ce_sm3_2part(u8 *head, u32 hlen, u8 *data, u32 len, u8 *out)
{
	u32 ret = 0;
    ret = hash2(HASH_SM3, head, hlen, data, len, out);
    return ret;
}

u32 osr_ce_sm2_verify(u8 *hash, u8 *pk, u8 *sign)
{
	return sm2_verify(hash, pk, sign);
}

s32 mbox_cmd_verify(void *head, u32 hlen, u32 daddr, u32 dlen, u32 saddr,
		    u32 slen)
{
    u8 calc_sm3[SM3_BYTE_LEN];
	u8 sm2_pk[SM2_PUBKEY_LEN];
	u32 ret;

	if (slen != 64) {
		pr_err("signature length is not for SM2+SM3.\n");
		return VERIFY_FAIL;
	}

	efuse_get_sm2_pubkey(KEY_INDEX_FOR_MAILBOX, sm2_pk);

	hexdump_msg("pub key", sm2_pk, 65, HEXDUMP_GROUPSIZE);
	ret = osr_ce_sm3_2part(head, hlen, (void *)daddr, dlen, calc_sm3);
	if (ret != 0) {
        pr_err("Calculate image's SM3 failed.\n");
        return VERIFY_FAIL;
    }

	hexdump_msg("SM3 ", calc_sm3, 32, HEXDUMP_GROUPSIZE);
	hexdump_msg("sign ", (void *)saddr, slen, HEXDUMP_GROUPSIZE);
	ret = osr_ce_sm2_verify(calc_sm3, sm2_pk, (void *)saddr);
	if (ret != 0) {
        pr_err("eFuse signature verification failed.\n");
        return VERIFY_FAIL;
    }

	return VERIFY_PASS;
}
