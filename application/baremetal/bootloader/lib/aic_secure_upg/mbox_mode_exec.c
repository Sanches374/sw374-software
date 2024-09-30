/*
 * Copyright (c) 2024, ArtInChip Technology CO.,LTD. All Rights Reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Author: Chen JunLong <junlong.chen@artinchip.com>
 */

#include "mbox_mode.h"
#include <image.h>
#include "brn.h"

#define IMG_TYPE_UNKNOW -1
#define IMG_TYPE_BRN 0
#define IMG_TYPE_AIC 1
typedef int (*entry_func)(u32 boot_param, void *res_addr);
typedef int (*pbp_entry)(u32 boot_param, void *res_addr);

static s32 _check_image_type(u8 *addr)
{
	if (memcmp(addr, "PT", 2) == 0)
		return IMG_TYPE_BRN;
	if (memcmp(addr, "AIC ", 2) == 0)
		return IMG_TYPE_AIC;
	return IMG_TYPE_UNKNOW;
}

static void _exec_brn(void *head, u32 hlen, u32 fw_addr, u32 param)
{
}

static void _exec_aic(u32 fw_addr, u32 param)
{
}
/*
 * SESS execute firmware
 * If it is BRN image
 * If it is AIC image
 */
void mbox_cmd_exec(void *head, u32 hlen, u32 fw_addr, u32 param)
{
    s32 ret = IMG_TYPE_UNKNOW;

    if (hlen) {
		/* PT upgmode, always pass the BRN header to secure CPU
		 * but in AIC upgmode, there is no BRN header
		 */
		ret = _check_image_type((void *)head);
	}
	if (ret == IMG_TYPE_UNKNOW)
		ret = _check_image_type((void *)fw_addr);

	if (ret == IMG_TYPE_BRN)
		_exec_brn(head, hlen, fw_addr, param);
	if (ret == IMG_TYPE_AIC)
		_exec_aic(fw_addr, param);
}
