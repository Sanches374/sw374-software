/*
 * Copyright (c) 2023-2024, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors:  Zequan Liang <zequan.liang@artinchip.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common_test.h"

#define AIC_GPAI_VOLTAGE_ACCURACY   10000
#define AIC_GPAI_VOLITAGE_REFERENCE 2.5
#define RTC_TEST_TIME               5

static char write_buff[512];
#if QC_OS_RT_THREAD
#include <sys/stat.h>
#include <rtthread.h>
#include <rtdevice.h>

qc_res_t gpai_calculate_range_error_voltage(float input_voltage, float reference_voltage, float voltage)
{
    if (abs(input_voltage - (reference_voltage * AIC_GPAI_VOLTAGE_ACCURACY)) > (voltage * AIC_GPAI_VOLTAGE_ACCURACY)) {
        return QC_RES_INV;
    }
    return QC_RES_OK;
}

qc_res_t gpai_calculate_percentage_error_voltage(float input_voltage, float reference_voltage, float percentage)
{
    if (abs(input_voltage - (reference_voltage * AIC_GPAI_VOLTAGE_ACCURACY)) / AIC_GPAI_VOLITAGE_REFERENCE > (percentage * AIC_GPAI_VOLTAGE_ACCURACY)) {
        return QC_RES_INV;
    }
    return QC_RES_OK;
}

qc_res_t common_sdcard_module_test(void *param)
{
    const char *file_name = "/sdcard/sd_text.txt";
    return qc_block_dev_test(file_name, write_buff, sizeof(write_buff));
}

qc_res_t common_usb_module_test(void *param)
{
    const char *file_name = "/udisk/udisk_text.txt";
    return qc_block_dev_test(file_name, write_buff, sizeof(write_buff));
}

qc_res_t common_qc_save_chip(void *param)
{
    struct qc_board_config *board = (struct qc_board_config *)(param);

    return qc_save_write(board);
}

qc_res_t common_compare_chip_intel_module(void *param)
{
    char chip_intel_module[DESC_LEN] = {0};
    qc_board_config_t *board_config = (qc_board_config_t *)param;

    qc_read_chip_intel_model(chip_intel_module, sizeof(chip_intel_module));
    if (strncmp(chip_intel_module, board_config->chip_intel_module, DESC_LEN - 1) == 0)
        return QC_RES_OK;
    return QC_RES_INV;
}

#ifdef AIC_RTC_DRV
static int last_hour = 0;
static int last_min = 0;
static int last_sec = 0;

qc_res_t common_rtc_module_test_init(void *param)
{
    if (set_date(2024, 3, 20) != RT_EOK) {
        printf("set RTC date failed");
        return QC_RES_INV;
    }
    rt_thread_mdelay(1);
    if (set_time(00, 00, 00) != RT_EOK) {
        printf("set RTC time failed");
        return QC_RES_INV;
    }
    return QC_RES_OK;
}

qc_res_t common_rtc_module_test_start(void *param)
{
    time_t now;
    struct tm *local_time;

    now = time(RT_NULL);
    local_time = localtime(&now);
    last_hour = local_time->tm_hour;
    last_min = local_time->tm_min;
    last_sec = local_time->tm_sec;
    return QC_RES_OK;
}

qc_res_t common_rtc_module_test(void *param)
{
    time_t now;
    struct tm *local_time;

    now = time(RT_NULL);
    local_time = localtime(&now);
    if (abs(local_time->tm_year + 1900 - 2024) > 0 ||
        abs(local_time->tm_mon + 1 - 3) > 0 ||
        abs(local_time->tm_mday - 20) > 0) {
            return QC_RES_INV;
    } else {
        int now_seconds = local_time->tm_hour *3600 + local_time->tm_min * 60 + local_time->tm_sec;
        int last_seconds = last_hour * 3600 + last_min * 60 + last_sec;
        if (abs(last_seconds - now_seconds) > RTC_TEST_TIME) {
            return QC_RES_INV;
        }
    }

    return QC_RES_OK;
}
#else
qc_res_t common_rtc_module_test_init(void *param)
{
    return QC_RES_OK;
}

qc_res_t common_rtc_module_test_start(void *param)
{
    return QC_RES_OK;
}

qc_res_t common_rtc_module_test(void *param)
{
    return QC_RES_OK;
}
#endif

#ifdef AIC_USING_GMAC0
#include "lwip/netif.h"
qc_res_t common_mac_test_disp_testing(void *param)
{
    return QC_RES_OK;
}

qc_res_t common_mac_wait_start_up(void *param)
{
    int timeout = 1000;
    while ((!netif_is_link_up(netif_default)) || (timeout > 0)) {
        rt_thread_mdelay(50);
        timeout -= 50;
        if (timeout < 0)
            return QC_RES_INV;
    }
    return QC_RES_OK;
}
#else
qc_res_t common_mac_test_disp_testing(void *param)
{
    return QC_RES_OK;
}

qc_res_t common_mac_wait_start_up(void *param)
{
    return QC_RES_OK;
}
#endif

qc_res_t common_uart_test(void *param)
{
    return QC_RES_OK;
}

#else
qc_res_t gpai_calculate_range_error_voltage(float input_voltage, float reference_voltage, float voltage)
{
    if (abs(input_voltage - (reference_voltage * AIC_GPAI_VOLTAGE_ACCURACY)) > (voltage * AIC_GPAI_VOLTAGE_ACCURACY)) {
        return QC_RES_INV;
    }
    return QC_RES_OK;
}

qc_res_t gpai_calculate_percentage_error_voltage(float input_voltage, float reference_voltage, float percentage)
{
    if (abs(input_voltage - (reference_voltage * AIC_GPAI_VOLTAGE_ACCURACY)) / AIC_GPAI_VOLITAGE_REFERENCE > (percentage * AIC_GPAI_VOLTAGE_ACCURACY)) {
        return QC_RES_INV;
    }
    return QC_RES_OK;
}

qc_res_t common_sdcard_module_test(void *param)
{
    const char *file_name = "./sd_text.txt";
    return qc_block_dev_test(file_name, write_buff, sizeof(write_buff));
}

qc_res_t common_usb_module_test(void *param)
{
    const char *file_name = "./udisk_text.txt";
    return qc_block_dev_test(file_name, write_buff, sizeof(write_buff));
}

qc_res_t common_qc_save_chip(void *param)
{
    struct qc_board_config *board = (struct qc_board_config *)(param);

    return qc_save_write(board);
}

qc_res_t common_compare_chip_intel_module(void *param)
{
    return QC_RES_OK;
}

qc_res_t common_rtc_module_test_init(void *param)
{
    return QC_RES_OK;
}

qc_res_t common_rtc_module_test_start(void *param)
{
    return QC_RES_OK;
}

qc_res_t common_rtc_module_test(void *param)
{
    return QC_RES_OK;
}

qc_res_t common_mac_test_disp_testing(void *param)
{
    return QC_RES_OK;
}

qc_res_t common_mac_wait_start_up(void *param)
{
    return QC_RES_OK;
}

qc_res_t common_uart_test(void *param)
{
    return QC_RES_OK;
}

#endif
