/*
 * Copyright (c) 2023, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors: zrq <ruiqi.zheng@artinchip.com>
 */
#include <rtconfig.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <rtthread.h>
#include <rtdevice.h>
#include <finsh.h>
#include <string.h>
#include "aic_core.h"
#include <boot_param.h>
#include <burn.h>

#ifdef AIC_SPINOR_DRV
#include <fal.h>

/* the address offset of download partition */
#ifndef RT_USING_FAL
#error "Please enable and confirgure FAL part."
#endif /* RT_USING_FAL */

const struct fal_partition *dl_part = RT_NULL;
#endif

#ifdef AIC_SPINAND_DRV
struct rt_mtd_nand_device *nand_mtd;
rt_device_t nand_dev;
#endif

int aic_ota_find_part(char *partname)
{
    switch (aic_get_boot_device()) {
#ifdef AIC_SPINOR_DRV
        case BD_SPINOR:
            /* Get download partition information and erase download partition data */
            if ((dl_part = fal_partition_find(partname)) == RT_NULL) {
                LOG_E("Firmware download failed! Partition (%s) find error!",
                      partname);
                return -RT_ERROR;
            }
            break;
#endif
#ifdef AIC_SPINAND_DRV
        case BD_SPINAND:
            nand_dev = rt_device_find(partname);
            if (nand_dev == RT_NULL) {
                LOG_E("Firmware download failed! Partition (%s) find error!",
                      partname);
                return -RT_ERROR;
            }

            nand_mtd = (struct rt_mtd_nand_device *)nand_dev;
            break;
#endif
        default:
            return -RT_ERROR;
            break;
    }

    LOG_I("Partition (%s) find success!", partname);
    return 0;
}

#ifdef AIC_SPINOR_DRV
int aic_ota_nor_erase_part(void)
{
    LOG_I("Start erase flash (%s) partition!", dl_part->name);
    if (fal_partition_erase(dl_part, 0, dl_part->len) < 0) {
        LOG_E("Firmware download failed! Partition (%s) erase error! len = %d",
              dl_part->name, dl_part->len);
        return -RT_ERROR;
    }
    LOG_I("Erase flash (%s) partition success! len = %d", dl_part->name,
          dl_part->len);
    return 0;
}
#endif

#ifdef AIC_SPINAND_DRV
int aic_ota_nand_erase_part(void)
{
    unsigned long blk_offset = 0;

    LOG_I("Start erase nand flash partition!");

    while (nand_mtd->block_total > blk_offset) {
        if (rt_mtd_nand_check_block(nand_mtd, blk_offset) != RT_EOK) {
            LOG_W("Erase block is bad, skip it.\n");
            blk_offset++;
            continue;
        }

        rt_mtd_nand_erase_block(nand_mtd, blk_offset);
        blk_offset++;
    }

    LOG_I("Erase nand flash partition success! len = %d",
          nand_mtd->block_total);

    return 0;
}

int aic_ota_nand_write(uint32_t addr, const uint8_t *buf, size_t size)
{
    unsigned long blk = 0, offset = 0, page = 0;
    static unsigned long bad_block_off = 0;
    unsigned long blk_size = nand_mtd->pages_per_block * nand_mtd->page_size;
    rt_err_t ret = 0;

    if (size > 2048) {
        LOG_E("OTA_BURN_LEN need set 2048! size = %d", size);
        return -RT_ERROR;
    }

    ret = rt_device_open(nand_dev, RT_DEVICE_OFLAG_RDWR);
    if (ret) {
        LOG_E("Open MTD device failed.!\n");
        return ret;
    }

    offset = addr + bad_block_off;

    /* Search for the first good block after the given offset */
    if (offset % blk_size == 0) {
        blk = offset / blk_size;
        while (rt_mtd_nand_check_block(nand_mtd, blk) != RT_EOK) {
            LOG_W("find a bad block(%d), off adjust to the next block\n", blk);
            bad_block_off += nand_mtd->pages_per_block;
            offset = addr + bad_block_off;
            blk = offset / blk_size;
        }
    }

    page = offset / nand_mtd->page_size;
    ret = rt_mtd_nand_write(nand_mtd, page, buf, size, RT_NULL, 0);
    if (ret) {
        LOG_E("Failed to write data to NAND.\n");
        ret = -RT_ERROR;
        goto aic_ota_nand_write_exit;
    }

aic_ota_nand_write_exit:
    rt_device_close(nand_dev);

    return 0;
}
#endif

int aic_ota_erase_part(void)
{
    int ret = 0;

    switch (aic_get_boot_device()) {
#ifdef AIC_SPINOR_DRV
        case BD_SPINOR:
            ret = aic_ota_nor_erase_part();
            break;
#endif
#ifdef AIC_SPINAND_DRV
        case BD_SPINAND:
            ret = aic_ota_nand_erase_part();
            break;
#endif
        default:
            break;
    }

    return ret;
}

int aic_ota_part_write(uint32_t addr, const uint8_t *buf, size_t size)
{
    int ret = 0;

    switch (aic_get_boot_device()) {
#ifdef AIC_SPINOR_DRV
        case BD_SPINOR:
            ret = fal_partition_write(dl_part, addr, buf, size);
            if (ret < 0) {
                LOG_E(
                    "Firmware download failed! Partition (%s) write data error!",
                    dl_part->name);
                return -RT_ERROR;
            } else {
                ret = RT_EOK;
            }
            break;
#endif
#ifdef AIC_SPINAND_DRV
        case BD_SPINAND:
            ret = aic_ota_nand_write(addr, buf, size);
            if (ret < 0) {
                LOG_E(
                    "Firmware download failed! nand partition write data error!");
                return -RT_ERROR;
            }
            break;
#endif
        default:
            return -RT_ERROR;
            break;
    }

    return ret;
}

