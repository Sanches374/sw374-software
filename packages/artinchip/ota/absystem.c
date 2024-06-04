/*
 * Copyright (c) 2023, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors: xuan.wen <xuan.wen@artinchip.com>
 */

#include <rtconfig.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <aic_core.h>
#include <env.h>
#include <absystem.h>

#define OTA_FILE_CNT           3
#define OTA_FILE_REDUNDAND_CNT OTA_FILE_CNT * 2

char partname[OTA_FILE_REDUNDAND_CNT][32] = {
    "os",
    "rodata",
    "blk_data",
    "os_r",
    "rodata_r",
    "blk_data_r",
};

#define APPLICATION_RO_PART           "blk_rodata"
#define APPLICATION_RO_PART_REDUNDAND "blk_rodata_r"
#define APPLICATION_WR_PART           "blk_data"
#define APPLICATION_WR_PART_REDUNDAND "blk_data_r"
#define APPLICATION_MMC_RO_PART           "mmc0p5"
#define APPLICATION_MMC_RO_PART_REDUNDAND "mmc0p6"

static unsigned char target_offset = 0;

int aic_upgrade_start(void)
{
    char *now = NULL;
    int ret = 0;

    if (fw_env_open()) {
        LOG_E("Open env failed");
        return -1;
    }

    now = fw_getenv("osAB_now");
    LOG_I("osAB_now = %s", now);
    if (strncmp(now, "A", 1) == 0) {
        LOG_I("Upgrade B system");
        target_offset = OTA_FILE_CNT;
    } else if (strncmp(now, "B", 1) == 0) {
        LOG_I("Upgrade A system");
        target_offset = 0;
    } else {
        ret = -1;
        LOG_I("invalid osAB_now");
    }

    fw_env_close();

    return ret;
}

char *aic_upgrade_get_partname(unsigned char index)
{
    return partname[index + target_offset];
}

int aic_upgrade_end(void)
{
    char *now = NULL;
    int ret = 0;

    if (fw_env_open())
        return -1;

    now = fw_getenv("osAB_now");

    if (strncmp(now, "A", 1) == 0) {
        ret = fw_env_write("osAB_next", "B");
        LOG_I("Next startup in B system");
    } else if (strncmp(now, "B", 1) == 0) {
        ret = fw_env_write("osAB_next", "A");
        LOG_I("Next startup in A system");
    } else {
        LOG_I("invalid osAB_now");
        ret = -1;
    }

    if (ret)
        goto aic_upgrade_end_out;

    ret = fw_env_write("bootcount", "0");
    if (ret)
        goto aic_upgrade_end_out;

    ret = fw_env_write("upgrade_available", "1");
    if (ret)
        goto aic_upgrade_end_out;

    /* flush to flash */
    fw_env_flush();

aic_upgrade_end_out:

    fw_env_close();

    return ret;
}

int aic_ota_status_update(void)
{
    char *status = NULL;
    int ret = 0;

    if (fw_env_open()) {
        pr_err("Open env failed\n");
        return -1;
    }

    status = fw_getenv("upgrade_available");
#ifdef AIC_ENV_DEBUG
    printf("upgrade_available = %s\n", status);
#endif
    if (strncmp(status, "1", 2) == 0) {

        ret = fw_env_write("upgrade_available", "0");
        if (ret) {
            pr_err("Env write fail\n");
            goto aic_get_upgrade_status_err;
        }

        fw_env_flush();
    } else if (strncmp(status, "0", 2) == 0) {
        ret = 0;
        goto aic_get_upgrade_status_err;
    } else {
        pr_err("Invalid upgrade_available\n");
        ret = -1;
        goto aic_get_upgrade_status_err;
    }

aic_get_upgrade_status_err:
    fw_env_close();
    return ret;
}

int aic_get_rodata_to_mount(char *target_rodata)
{
    char *now = NULL;
    int ret = 0;

    if (fw_env_open()) {
        pr_err("Open env failed\n");
        return -1;
    }

    now = fw_getenv("osAB_now");
#ifdef AIC_ENV_DEBUG
    printf("osAB_now = %s\n", now);
#endif
    if (strncmp(now, "A", 2) == 0) {
        memcpy(target_rodata, APPLICATION_RO_PART, strlen(APPLICATION_RO_PART));
    } else if (strncmp(now, "B", 2) == 0) {
        memcpy(target_rodata, APPLICATION_RO_PART_REDUNDAND,
               strlen(APPLICATION_RO_PART_REDUNDAND));
    } else {
        ret = -1;
        pr_err("invalid osAB_now\n");
    }

    fw_env_close();

    return ret;
}

int aic_get_mmc_rodata_to_mount(char *target_rodata)
{
    char *now = NULL;
    int ret = 0;

    if (fw_env_open()) {
        pr_err("Open env failed\n");
        return -1;
    }

    now = fw_getenv("osAB_now");
#ifdef AIC_ENV_DEBUG
    printf("osAB_now = %s\n", now);
#endif
    if (strncmp(now, "A", 2) == 0) {
        memcpy(target_rodata, APPLICATION_MMC_RO_PART, strlen(APPLICATION_MMC_RO_PART));
    } else if (strncmp(now, "B", 2) == 0) {
        memcpy(target_rodata, APPLICATION_MMC_RO_PART_REDUNDAND,
               strlen(APPLICATION_MMC_RO_PART_REDUNDAND));
    } else {
        ret = -1;
        pr_err("invalid osAB_now\n");
    }

    fw_env_close();

    return ret;
}

int aic_get_data_to_mount(char *target_data)
{
    char *now = NULL;
    int ret = 0;

    if (fw_env_open()) {
        pr_err("Open env failed\n");
        return -1;
    }

    now = fw_getenv("osAB_now");
#ifdef AIC_ENV_DEBUG
    printf("osAB_now = %s\n", now);
#endif
    if (strncmp(now, "A", 2) == 0) {
        memcpy(target_data, APPLICATION_WR_PART, strlen(APPLICATION_WR_PART));
    } else if (strncmp(now, "B", 2) == 0) {
        memcpy(target_data, APPLICATION_WR_PART_REDUNDAND,
               strlen(APPLICATION_WR_PART_REDUNDAND));
    } else {
        ret = -1;
        pr_err("invalid osAB_now\n");
    }

    fw_env_close();

    return ret;
}
