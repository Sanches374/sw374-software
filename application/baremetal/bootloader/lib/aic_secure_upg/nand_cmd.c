/*
 * Copyright (c) 2024, ArtInChip Technology CO.,LTD. All Rights Reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Author: Chen JunLong <junlong.chen@artinchip.com>
 */

#include <stdlib.h>
#include <string.h>
#include <aicupg.h>
#include <spinand_port.h>
#include <mtd.h>
#include <aic_common.h>
#include "brn.h"

#define NAND_PAGE_SIZE 0x800
static bool nand_is_prepared;

struct aicupg_nand_priv {
    /* MTD partitions FWC will be written to */
    struct mtd_dev *mtd;
    unsigned long start_offset;
    unsigned long erase_offset;
    unsigned int remain_len;
};
struct aicupg_nand_priv *nand_priv;

static void set_nand_prepare_status(bool value)
{
    nand_is_prepared = value;
}

static bool get_nand_prepare_status(void)
{
    return nand_is_prepared;
}

static s32 nand_prepare(u32 id)
{
    struct aic_spinand *flash;

    flash = spinand_probe(id);
    if (!flash) {
        printf("Failed to probe spinand flash.\n");
        return -1;
    }

#ifdef AIC_SPIENC_BYPASS_IN_UPGMODE
    spienc_set_bypass(1);
#endif
    set_nand_prepare_status(true);
    return 0;
}

s32 nand_start(struct brn_state *brn)
{
    u32 ret = 0;
    char *device_name = "nand0";

    if (!get_nand_prepare_status())
        ret = nand_prepare(brn->head.storage_index);
    if (ret)
        goto err;

    device_name[4] += brn->head.storage_index;
    nand_priv = malloc(sizeof(struct aicupg_nand_priv));
    if (!nand_priv) {
        pr_err("Out of memory, malloc failed.\n");
        goto err;
    }
    memset(nand_priv, 0, sizeof(struct aicupg_nand_priv));

    nand_priv->mtd = mtd_get_device(device_name);
    nand_priv->start_offset = brn->head.storage_address;
    nand_priv->erase_offset = brn->head.storage_address;
    nand_priv->remain_len = brn->head.length;
    if (!nand_priv->mtd) {
        pr_err("Get %s mtd failed.\n", device_name);
        goto err;
    }

    return 0;
err:
    pr_err("error:free(nand_priv)\n");
    free(nand_priv);
    return -1;
}

static s32 nand_fwc_mtd_erase_write(u32 dolen, struct mtd_dev *mtd, struct aicupg_nand_priv *priv, int i, u8 *buf)
{
    int ret = 0;

    if (((priv->start_offset + dolen) > priv->erase_offset)) {
    erase_err:
        /* Erase the block, before write it. */
        ret = mtd_erase(mtd, priv->erase_offset, ROUNDUP(dolen, mtd->erasesize));
        if (ret) {
            pr_err("Erase block is bad, mark it.\n");
            ret = mtd_block_markbad(mtd, priv->erase_offset);
            if (ret)
                pr_err("Mark block is bad.\n");
            priv->erase_offset += mtd->erasesize;
            priv->start_offset += mtd->erasesize;
            goto erase_err;
        }

        /* Check the block before write it. */
        if (mtd_block_isbad(mtd, priv->erase_offset)) {
            pr_err("Check block is bad, !!! Unexecpt happened. !!!\n");
            priv->erase_offset += mtd->erasesize;
            priv->start_offset += mtd->erasesize;
            goto erase_err;
        }
        priv->erase_offset += mtd->erasesize;
    }
    pr_debug("priv->erase_offset: %lu, priv->start_offset: %lu, dolen: %u\n",
            priv->erase_offset, priv->start_offset, dolen);
    ret = mtd_write(mtd, priv->start_offset, buf, dolen);
    if (ret) {
        pr_err("Write mtd %s block error, mark it.\n", mtd->name);
        ret = mtd_block_markbad(mtd, priv->start_offset);
        if (ret)
            pr_err("Mark block is bad.\n");
        goto erase_err;
    }
    priv->start_offset += dolen;
    priv->remain_len -= dolen;
    return 0;
}

static int nand_data_write(u32 addr, u8 *buf, u32 len)
{
    struct aicupg_nand_priv *priv;
    struct mtd_dev *mtd;

    priv = nand_priv;
    mtd = priv->mtd;
    if (!mtd) {
        pr_err("mtd is NULL\n");
        return 0;
    }

    if ((priv->start_offset + len) > (mtd->size)) {
        pr_err("Not enough space to write mtd %s\n", mtd->name);
        return 0;
    }
    pr_debug("mtd: %s, len:%u, remain_len: %u\n", mtd->name, len, priv->remain_len);
    u32 dolen = mtd->erasesize;
    u32 count = len / mtd->erasesize;
    int j = 0;
    /* Len is lager than block size, handle the aligned part. */
    if (len > mtd->erasesize) {
        pr_debug("priv->erase_offset: %lu, priv->start_offset: %lu\n", priv->erase_offset, priv->start_offset);
        for (j = 0; j < count; j++) {
            nand_fwc_mtd_erase_write(dolen, mtd, priv, 0, buf);
            buf += dolen;
        }
    }

    /* Handle the part out of aligned. */
    if (len % mtd->erasesize && (priv->remain_len == len)) {
        pr_debug("priv->erase_offset: %lu, priv->start_offset: %lu\n", priv->erase_offset, priv->start_offset);
        dolen = len - (count * mtd->erasesize);
        nand_fwc_mtd_erase_write(dolen, mtd, priv, 0, buf);
    } else if (len % mtd->erasesize && (priv->remain_len != len)) {     /* data len is not enough a blocksize */
        dolen = len - (count * mtd->erasesize);
        nand_fwc_mtd_erase_write(dolen, mtd, priv, 0, buf);
    }
    return len;
}

u32 nand_data_align_write(struct brn_state *brn, u8 *buf, u32 len)
{
    u32 addr = 0;
    static u8 temp_buf[NAND_PAGE_SIZE];
    static int index;

    for (u32 i = 0; i < len; i++, index++) {
        temp_buf[index] = buf[i];
        if (index == NAND_PAGE_SIZE - 1) {
            nand_data_write(addr, temp_buf, NAND_PAGE_SIZE);
            index = 0;
            memset(temp_buf, 0, NAND_PAGE_SIZE);
        }
    }

    /* last data */
    if (brn->recv_len + len >= brn->head.length) {
        nand_data_write(addr, temp_buf, index);
        index = 0;
        memset(temp_buf, 0, NAND_PAGE_SIZE);
    }
	return len;
}

void nand_data_end(void)
{
    struct aicupg_nand_priv *priv;

    priv = nand_priv;
    if (!priv)
        return;

    free(priv);
}
