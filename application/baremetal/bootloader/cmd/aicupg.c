/*
 * Copyright (c) 2023, Artinchip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Wu Dehuang <dehuang.wu@artinchip.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <malloc.h>
#include <console.h>
#include <aic_core.h>
#include <aic_common.h>
#include <aic_errno.h>
#include <boot_param.h>
#include <usb_drv.h>
#include <usbhost.h>
#include <config_parse.h>
#include <aicupg.h>
#include <fatfs.h>
#include <mmc.h>
#include <hal_syscfg.h>
#include <upg_uart.h>
#include <hal_rtc.h>
#include <wdt.h>

#define AICUPG_HELP                                                      \
    "ArtInChip upgrading command:\n"                                     \
    "aicupg [devtype] [interface]\n"                                     \
    "  - devtype: should be usb, uart, mmc, fat, brom\n"                 \
    "  - interface: specify the controller id\n"                         \
    "e.g.\n"                                                             \
    "  aicupg usb 0\n"                                                   \
    "  aicupg mmc 1\n"                                                   \
    "when devtype is fat: \n"                                            \
    "aicupg [devtype] [blkdev] [interface]\n"                            \
    "- blkdev: should be udisk,mmc \n"                                   \
    "e.g.: \n"                                                           \
    "  aicupg fat udisk 0\n"                                             \
    "  aicupg fat mmc 1\n"                                               \
    "when devtype is brom, device will reset to Boot ROM upgrade mode\n" \
    "  aicupg brom\n"                                                    \
    "  aicupg\n"

static void aicupg_help(void)
{
    puts(AICUPG_HELP);
}

#define AICUPG_ARGS_MAX 4
extern struct usb_device usbupg_device;
extern void stdio_unset_uart(int id);

static int ctrlc(void)
{
    switch (getchar()) {
        case 0x03:              /* ^C - Control C */
            return 1;
        default:
            break;
    }

    return 0;
}

#if defined(AIC_BOOTLOADER_FATFS_SUPPORT)
static int image_header_check(struct image_header_pack *header)
{
    /*check header*/
    if ((strcmp(header->hdr.magic, "AIC.FW") != 0)) {
        pr_err("Error:image check failed, maybe not have a image in media!\n");
        return -1;
    }

    return 0;
}
#endif

static int do_uart_protocol_upg(int intf)
{
    int ret = 0;

#if defined(AICUPG_UART_ENABLE)
    stdio_unset_uart(intf);
    aic_upg_uart_init(intf);
    while (1) {
        if (ctrlc())
            break;
        aic_upg_uart_loop();
    }
#endif

    return ret;
}

static int do_usb_protocol_upg(int intf)
{
    int ret = 0;

#if defined(AICUPG_USB_ENABLE)
    struct upg_init init;
#ifndef AIC_SYSCFG_DRV_V12
    syscfg_usb_phy0_sw_host(0);
#endif
    init.mode = INIT_MODE(UPG_MODE_FULL_DISK_UPGRADE);
    aicupg_initialize(&init);
    aic_udc_init(&usbupg_device);
    while (1) {
        if (ctrlc())
            break;
        aic_udc_state_loop();
    }
#endif

    return ret;
}

static int do_sdcard_upg(int intf)
{
    int ret = 0;

    return ret;
}

int boot_cfg_parse_direct_mode_val(char *strofs, char *name, u32 *offset,
                                   char *attr)
{
    int len, i;
    char *p = NULL;

    if (strofs == NULL || offset == NULL)
        return -1;

    len = strlen(strofs);

    for (i = 0; i < len; i++) {
        name[i] = strofs[i];
        if (name[i] == ',') {
            p = &strofs[i + 1];
            name[i] = ' ';
            break;
        }
    }
    while (i) {
        /* strip blank of tail */
        if (name[i] == ' ' || name[i] == '\t')
            name[i] = 0;
        else
            break;
        i--;
    }

    *offset = 0;
    if (p == NULL)
        return 0;

    if (attr == NULL)
        return 0;

    while (*p == ' ' || *p == '\t')
        p++;

    *offset = strtoul(p, NULL, 0);

    while (*p != 0 && *p != ',')
        p++;

    if (*p == ',')
        strcpy(attr, p + 1);

    return 0;
}

#if defined(AIC_BOOTLOADER_FATFS_SUPPORT)
/* Direct mode:
 * # writetype value can be: spi-nor, spi-nand, mmc
 * writetype=spi-nor
 * # writeintf specify the controller id, default is 0
 * writeintf=0
 * writeboot=bootloader.aic
 * write0=data0.bin,0x1000
 * write1=data1.bin,0x2000
 * write2=data2.bin,0x3000
 * # Support format:
 * #   writeX=file,offset,attribute
 * #   writeX=file,offset
 * #   writeX=file
 * # If offset is absent, default value is 0
 * write3=data.fatfs,0x3000,nftl
 */
static int do_fat_upg_in_direct_mode(char *cfg, u32 clen)
{
    u32 i, ret, maxlen, offset, intf_id;
    char keyname[16], keyval[IMG_NAME_MAX_SIZ], type[16], fname[32], attr[16];

    maxlen = IMG_NAME_MAX_SIZ;
    ret = boot_cfg_get_key_val(cfg, clen, "writetype", 9, keyval, maxlen);
    if (ret == 0) {
        pr_err("Key writetype is not found.\n");
        return -1;
    }

    if (boot_cfg_parse_direct_mode_val(keyval, type, &offset, NULL)) {
        pr_err("parse writetype error.\n");
        return -1;
    }

    intf_id = 0;
    ret = boot_cfg_get_key_val(cfg, clen, "writeintf", 9, keyval, maxlen);
    if (ret > 0) {
        intf_id = strtoul(keyval, NULL, 0);
    }
    printf("Going to program %s on controller %d\n", type, intf_id);
    ret = boot_cfg_get_key_val(cfg, clen, "writeboot", 9, keyval, maxlen);
    if (ret > 0) {
        memset(fname, 0, 32);
        boot_cfg_parse_direct_mode_val(keyval, fname, &offset, NULL);
        aicupg_fat_direct_write(type, intf_id, fname, offset, 1, NULL);
    }

    for (i = 0; i < 32; i++) {
        snprintf(keyname, 16, "write%d", i);
        ret = boot_cfg_get_key_val(cfg, clen, keyname, strlen(keyname), keyval,
                                   maxlen);
        if (ret == 0) {
            goto out;
        }
        memset(fname, 0, 32);
        memset(attr, 0, 16);
        boot_cfg_parse_direct_mode_val(keyval, fname, &offset, attr);
        aicupg_fat_direct_write(type, intf_id, fname, offset, 0, attr);
    }

out:
    printf("Done\n");
    return 0;
}

static int do_fat_upg_by_single_image(char *image_name, char *protection)
{
    struct image_header_pack *hdrpack;
    int ret;
    ulong actread;

    hdrpack = (void *)aicos_malloc_align(0, sizeof(struct image_header_pack), CACHE_LINE_SIZE);
    if (!hdrpack) {
        pr_err("Error, malloc hdrpack failed.\n");
        return -1;
    }
    memset(hdrpack, 0, sizeof(struct image_header_pack));

    ret = aic_fat_read_file(image_name, (void *)hdrpack, 0,
                            sizeof(struct image_header_pack), &actread);
    if (actread != sizeof(struct image_header_pack) || ret) {
        printf("Error:read file %s failed!\n", image_name);
        ret = -1;
        goto err;
    }

    /*check header*/
    ret = image_header_check(hdrpack);
    if (ret) {
        pr_err("check image header failed!\n");
        ret = -1;
        goto err;
    }

    /*write data to media*/
    ret = aicupg_fat_write(image_name, protection, &hdrpack->hdr);
    if (ret == 0) {
        pr_err("fat write data failed!\n");
        ret = -1;
        goto err;
    }

    ret = 0;
err:
    if (hdrpack)
        aicos_free_align(0, hdrpack);
    return ret;
}
#endif

static int do_fat_upg(int intf, char *const blktype)
{

    int ret = 0;
#if defined(AIC_BOOTLOADER_FATFS_SUPPORT)
    char image_name[IMG_NAME_MAX_SIZ] = {0};
    char protection[PROTECTION_PARTITION_LEN] = {0};
    ulong actread, offset, bootlen;
    char *file_buf;

    if (!strcmp(blktype, "udisk")) {
        /*usb init*/
#if defined(AICUPG_UDISK_ENABLE)
        if (usbh_initialize(intf) < 0) {
            pr_err("usbh init failed!\n");
            ret = -1;
            return ret;
        }

        ret = aic_fat_set_blk_dev(intf, BLK_DEV_TYPE_MSC);
        if (ret) {
            pr_err("set blk dev failed.\n");
            return ret;
        }
#else
        pr_err("udisk upgrade disabled.\n");
#endif
    } else if (!strcmp(blktype, "mmc")) {
#if defined(AICUPG_SDCARD_ENABLE)
        ret = mmc_init(intf);
        if (ret) {
            printf("sdmc %d init failed.\n", intf);
            return ret;
        }

        ret = aic_fat_set_blk_dev(intf, BLK_DEV_TYPE_MMC);
        if (ret) {
            pr_err("set blk dev failed.\n");
            return ret;
        }
#else
        pr_err("sdcard upgrade disabled.\n");
#endif
    } else {
        return ret;
    }

    file_buf = (char *)aicos_malloc_align(0, 2048, CACHE_LINE_SIZE);
    if (!file_buf) {
        pr_err("Error, malloc buf failed.\n");
        ret = -1;
        goto err;
    }
    memset((void *)file_buf, 0, 2048);

    ret = aic_fat_read_file("bootcfg.txt", (void *)file_buf, 0, 2048, &actread);
    if (actread == 0 || ret) {
        printf("Error:read file bootcfg.txt failed!\n");
        goto err;
    }

    ret = boot_cfg_get_boot0(file_buf, actread, image_name, IMG_NAME_MAX_SIZ,
                             &offset, &bootlen);

    if (offset > 0) {
        /* Perform upgrading from image file */
        ret = boot_cfg_get_protection(file_buf, actread, protection,
                                      PROTECTION_PARTITION_LEN);
        if (ret)
            pr_warn("No protected partition.\n");
        else
            pr_info("Protected=%s\n", protection);
        ret = do_fat_upg_by_single_image(image_name, protection);
    } else {
        ret = do_fat_upg_in_direct_mode(file_buf, actread);
    }

err:
    if (file_buf)
        aicos_free_align(0, file_buf);
#endif
    return ret;
}

static void do_brom_upg(void)
{
    aic_set_reboot_reason(REBOOT_REASON_UPGRADE);

#ifdef AIC_WDT_DRV
    wdt_init();
    printf("Going to reboot ...\n");
    wdt_expire_now();
    while(1)
        continue;
#endif
}

static int do_aicupg(int argc, char *argv[])
{
    char *devtype = NULL;
    int intf, ret = 0;

    aic_get_reboot_reason();

    if ((argc == 1) || ((argc == 2) && (!strcmp(argv[1], "brom")))) {
        do_brom_upg();
        return 0;
    }

    if ((argc < 3) || (argc > AICUPG_ARGS_MAX))
        goto help;
    devtype = argv[1]; /* mmc  usb fat */
    if (argc >= 4 && argv[3])
        intf = strtol(argv[3], NULL, 0);
    else
        intf = strtol(argv[2], NULL, 0);

    if (devtype == NULL)
        goto help;
    if (!strcmp(devtype, "uart"))
        ret = do_uart_protocol_upg(intf);
    if (!strcmp(devtype, "usb"))
        ret = do_usb_protocol_upg(intf);
    if (!strcmp(devtype, "mmc"))
        ret = do_sdcard_upg(intf);
    if (!strcmp(devtype, "fat"))
        ret = do_fat_upg(intf, argv[2]);

    return ret;

help:
    aicupg_help();
    return ret;
}

CONSOLE_CMD(aicupg, do_aicupg, "Upgrading mode command.");
