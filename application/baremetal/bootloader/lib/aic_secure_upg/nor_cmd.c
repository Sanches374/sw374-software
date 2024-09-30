/*
 * Copyright (c) 2024, ArtInChip Technology CO.,LTD. All Rights Reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Author: Chen JunLong <junlong.chen@artinchip.com>
 */

#include <stdlib.h>
#include <string.h>
#include <aic_core.h>
#include <aicupg.h>
#include <mtd.h>
#include <sfud.h>
#include <spienc.h>
#include "brn.h"

#define MAX_NOR_NAME        32

static bool nor_is_prepared;

struct aicupg_nor_priv {
    struct mtd_dev *mtd;
    unsigned long start_offset;
    unsigned long erase_offset;
};
struct aicupg_nor_priv *nor_priv;

static void set_nor_prepare_status(bool value)
{
    nor_is_prepared = value;
}

static bool get_nor_prepare_status(void)
{
    return nor_is_prepared;
}

/*
 * storage device prepare,should probe spi norflash
 *  - probe spi device
 *  - set env for MTD partitions
 *  - probe MTD device
 */
extern sfud_flash *sfud_probe(u32 spi_bus);
static s32 nor_prepare(u8 id)
{
    sfud_flash *flash;

    flash = sfud_probe(id);
    if (flash == NULL) {
        pr_err("Failed to probe spinor flash.\n");
        return -1;
    }

#ifdef AIC_SPIENC_BYPASS_IN_UPGMODE
    spienc_set_bypass(1);
#endif
    set_nor_prepare_status(true);
    return 0;
}

/*
 * New FirmWare Component start, should prepare to burn FWC to NOR
 *  - Get FWC attributes
 *  - Parse MTD partitions  FWC going to upgrade
 */
u32 nor_start(struct brn_state *brn)
{
    char *device_name = "nor0";
    u32 ret = 0;

    if (!get_nor_prepare_status())
        ret = nor_prepare(brn->head.storage_index);
    if (ret)
            goto err;

    device_name[3] += brn->head.storage_index;
    nor_priv = malloc(sizeof(struct aicupg_nor_priv));
    if (!nor_priv) {
        pr_err("Out of memory, malloc failed.\n");
        goto err;
    }
    memset(nor_priv, 0, sizeof(struct aicupg_nor_priv));
    nor_priv->mtd = mtd_get_device(device_name);
    nor_priv->start_offset = brn->head.storage_address;
    nor_priv->erase_offset = brn->head.storage_address;
    if (!nor_priv->mtd) {
        pr_err("Get %s mtd failed.\n", device_name);
        goto err;
    }
    return 0;
err:
    pr_err("error:free(nor_priv)\n");
    free(nor_priv);
    return -1;
}

/*
 * New FirmWare Component write, should write data to NOR
 *  - Erase MTD partitions for prepare write
 *  - Write data to MTD partions
 */
s32 nor_data_write(u32 addr, u8 *buf, s32 len)
{
    struct aicupg_nor_priv *priv;
    struct mtd_dev *mtd;
    unsigned long offset, erase_offset;
    int ret = 0;

    priv = nor_priv;
    mtd = priv->mtd;

    offset = priv->start_offset;
    erase_offset = priv->erase_offset;
    if ((offset + len) > (mtd->size)) {
        pr_err("Not enough space to write mtd %s\n", mtd->name);
        return 0;
    }

    /* erase 1 sector when offset+len more than erased address */
    while ((offset + len) > erase_offset) {
        ret = mtd_erase(mtd, erase_offset, ROUNDUP(len, mtd->erasesize));
        if (ret) {
            pr_err("MTD erase sector failed!\n");
            return 0;
        }
        priv->erase_offset = erase_offset + ROUNDUP(len, mtd->erasesize);
        erase_offset = priv->erase_offset;
    }

    ret = mtd_write(mtd, offset, buf, len);
    if (ret) {
        pr_err("Write mtd %s error.\n", mtd->name);
        return 0;
    }
    priv->start_offset = offset + len;

    return len;
}

/*
 * New FirmWare Component end, should free memory
 *  - free the memory what to  deposit device information
 */
void nor_data_end(void)
{
    struct aicupg_nor_priv *priv;

    priv = nor_priv;
    if (!priv)
        return;

    free(priv);
}

