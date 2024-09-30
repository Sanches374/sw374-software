/*
 * Copyright (c) 2024, ArtInChip Technology CO.,LTD. All Rights Reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Author: Chen JunLong <junlong.chen@artinchip.com>
 */

#include "mbox_mode.h"
#include "brn.h"
#include "image.h"
#include "platform.h"

typedef int (*brn_data_op)(struct brn_state *brn, u8 *buf, u32 len);
static struct brn_state usb_brn_state, *usb_brn;

u8 brn_head_cksum(u8 *head)
{
    u8 i, cksum = 0;

    for (i = 0; i < 20; i++)
        cksum += head[i];

    return cksum;
}

static s32 _check_is_secure_sram(u32 addr)
{
    if ((addr >= BASE_SE_SRAM) && (addr < (BASE_SE_SRAM + SE_SRAM_SIZE)))
        return 0;
    pr_err("It is not secure_sram\n");
    return -1;
}

static s32 ram_data_write(u32 addr, u8 *buf, u32 len)
{
    memcpy((u8 *)addr, buf, len);
    return 0;
}

s32 fw_start(struct brn_state *brn)
{
	switch (brn->head.storage_type) {
#ifdef AIC_BOOTLOADER_SPINOR_SUPPORT
	case BRN_STORAGE_SPINOR:
		return nor_start(brn);
#endif
		break;
#ifdef  AIC_BOOTLOADER_SPINAND_SUPPORT
	case BRN_STORAGE_SPINAND:
		return nand_start(brn);
#endif
		break;
	default:
		break;
	}
	return 0;
}

static int brn_data_op_unknown(struct brn_state *brn, u8 *buf, u32 len)
{
    brn->recv_len += len;
    return len;
}

static int mbox_write_data(struct brn_state *brn, u8 *buf, u32 len)
{
    u32 addr, percent = 0;

    brn->checksum += image_calc_checksum((u8 *)buf, len);
	if (brn->head.action != BRN_TYPE_WRITE_SECURE)
        return len;

    switch (brn->head.storage_type) {
	case BRN_STORAGE_RAM:
		addr = brn->head.storage_address + (u32)brn->recv_len;
		brn->recv_len += len;
		percent = brn->recv_len * 100 / brn->head.length;
        if (!_check_is_secure_sram(addr)) {
            ram_data_write(addr, buf, len);
            pr_debug("ram_addr 0x%08x, write %d/%d, %d%%\n",
			 addr, brn->recv_len, brn->head.length, percent);
        }
        break;
#ifdef AIC_BOOTLOADER_SPINOR_SUPPORT
	case BRN_STORAGE_SPINOR:
		addr = brn->head.storage_address + (u32)brn->recv_len;
		nor_data_write(addr, buf, len);
		brn->recv_len += len;
		percent = brn->recv_len * 100 / brn->head.length;
		pr_debug("nor_addr 0x%08x, write %d/%d, %d%%\n",
            addr, brn->recv_len, brn->head.length, percent);
        if (brn->head.length == brn->recv_len)
            nor_data_end();
        break;
#endif
#ifdef AIC_BOOTLOADER_SPINAND_SUPPORT
	case BRN_STORAGE_SPINAND:
		addr = brn->head.storage_address + (u32)brn->recv_len;
		brn->recv_len += nand_data_align_write(brn, buf, len);
		percent = brn->recv_len * 100 / brn->head.length;
		pr_debug("nand_addr 0x%08x, write %d/%d, %d%%\n",
        addr, brn->recv_len, brn->head.length, percent);
        if (brn->head.length == brn->recv_len)
            nand_data_end();
        break;
#endif
	default:
		break;
	}
	return len;
}

s32 mbox_cmd_write_data(u32 src, u32 dst, u32 len)
{
    static brn_data_op data = NULL;
    u32 datalen;
    u8 *p, headcksum;
    p = (u8 *)src;
    datalen = len;
    if (usb_brn == NULL) {
        if (p[0] != 'P' || p[1] != 'T') {
            pr_err("Image header is invalid.\n");
            return -1;
        }
        headcksum = brn_head_cksum((u8 *)p);
        if (headcksum != 0xFF) {
            pr_err("Image header checksum is invalid.\n");
            return -1;
        }
        usb_brn = &usb_brn_state;
        memset(usb_brn, 0, sizeof(usb_brn_state));
        memcpy(&usb_brn->head, p, sizeof(usb_brn->head));
        usb_brn->checksum =
            image_calc_checksum((u8 *)p, sizeof(usb_brn->head));
        datalen -= sizeof(usb_brn->head);
        /* The BRN header won't not write to dest storage address */
        p += sizeof(usb_brn->head);
        if(fw_start(usb_brn)) {
            pr_err("fw_satrt %s err\n", (usb_brn->head.storage_type==BRN_STORAGE_SPINOR)?"nor":"nand");
            data = brn_data_op_unknown;
            return -1;
        }
        data = mbox_write_data;
    }
    data(usb_brn, p, datalen);
    if (usb_brn->head.length == usb_brn->recv_len) {
        pr_debug("The transfer of the components is complete.\n");
        data = NULL;
        usb_brn = NULL;
    }
    return 0;
}
