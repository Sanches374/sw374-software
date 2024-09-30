/*
 * Copyright (c) 2024, ArtInChip Technology CO.,LTD. All Rights Reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Author: Chen JunLong <junlong.chen@artinchip.com>
 */


#include <stdio.h>
#include <string.h>
#include <console.h>
#include <getopt.h>
#include "aic_core.h"
#include "aic_log.h"
#include <sfud.h>
#include <mtd.h>
#include <boot.h>
#include <spienc.h>
#include <image.h>

#define APPP_OS_ADDR        0x80000
#define NOR_NAME            "nor5"
#define NOR_ID              5

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
    return 0;
}


static int spl_load_os(ulong *entry_point)
{
    int ret = 0;
    struct mtd_dev *mtd;
    struct image_header fw;


    ret = nor_prepare(NOR_ID);
    if (ret) {
        pr_err("%s prepare error.\n", NOR_NAME);
        return ret;
    }
    mtd = mtd_get_device(NOR_NAME);
    if (!mtd) {
        pr_err("read mtd %s error.\n", NOR_NAME);
        return -1;
    }

    ret = mtd_read(mtd, APPP_OS_ADDR, (u8 *)&fw, sizeof(struct image_header));
    if (ret) {
        pr_err("read image header error.\n");
        return ret;
    }
    ret = mtd_read(mtd, APPP_OS_ADDR, (u8 *)fw.load_address, fw.image_len);
    if (ret) {
        pr_err("mtd read %s error.\n", mtd->name);
        return ret;
    }
    *entry_point = (ulong)fw.entry_point;
    return ret;
}


static int do_senor_boot(int argc, char *argv[])
{
    int ret = 0;
    ulong entry_point;
    ret = spl_load_os(&entry_point);
    if (ret) {
        pr_err("%s error.\n", __func__);
        return ret;
    }
    boot_app((void *)entry_point);
    return ret;
}

CONSOLE_CMD(senor_boot, do_senor_boot, "Boot from SENOR.");

