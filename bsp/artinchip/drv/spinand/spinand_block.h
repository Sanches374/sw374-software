/*
 * Copyright (c) 2023, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors: xuan.wen <xuan.wen@artinchip.com>
 */

#ifndef __SPI_NAND_BLOCK_H__
#define __SPI_NAND_BLOCK_H__

#include <rtthread.h>
#include <rtdevice.h>
#include "spinand_parts.h"

struct spinand_blk_device {
    struct rt_device parent;
    struct rt_device_blk_geometry geometry;
    struct rt_mtd_nand_device *mtd_device;
#ifdef AIC_NFTL_SUPPORT
    struct nftl_api_handler_t *nftl_handler;
#endif
    char name[32];
    enum part_attr attr;
    u8 *pagebuf;
};

int rt_blk_nand_register_device(const char *name, struct rt_mtd_nand_device *mtd_device);

#endif /* __SPI_NAND_BLOCK_H__ */
