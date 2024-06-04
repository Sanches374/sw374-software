/*
 * Copyright (c) 2022-2023, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors: Li Siyao <siyao.li@artinchip.com>
 */

#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <sys/time.h>

#define DBG_TAG  "tsen.cmd"

#include "aic_core.h"
#include "aic_log.h"
#include "sensor.h"
#include "hal_tsen.h"

/* Global macro and variables */
#define THREAD_PRIORITY                 25
#define THREAD_STACK_SIZE               2048
#define THREAD_TIMESLICE                50

static rt_sensor_t g_sensor;
static int g_samples_num = 10;
static int g_sensor_id = 0;
static int g_polling_time = 10000;
static rt_device_t g_tsen_dev = RT_NULL;
static rt_thread_t  g_tsen_thread = RT_NULL;

static const char sopts[] = "s:n:t:h";
static const struct option lopts[] = {
    {"sensor_id", required_argument,             NULL, 's'},
    {"samples_num", required_argument,           NULL, 'n'},
    {"polling_interval", required_argument,      NULL, 't'},
    {"help", no_argument,                        NULL, 'h'},
    {0, 0, 0, 0}
    };

static void cmd_tsen_usage(char *program)
{
    printf("Usage: %s [options]\n", program);
    printf("\t -s, --sensor_id\t\tSelect the sensor, default is 0\n");
    printf("\t -n, --samples_number\t\tSet the samples number, default is 10\n");
    printf("\t -t, --polling_interval\t\tSet the polling interval (ms), default is 10000ms\n");
    printf("\t -h, --help \n");
    printf("\n");
    printf("Example: %s -s 1 -n 10\n", program);
    printf("Sensor ID:\n");
    printf("\t[0] sensor_cpu\n");
    printf("\t[1] sensor_gpai\n");
}


static void tsen_single_mode(void) {
    int num = 0;
    struct rt_sensor_data data;

    for (num = 0; num < g_samples_num; num++) {
        if (rt_device_read(g_tsen_dev, 0, &data, 1) == 1)
            rt_kprintf("num:%3d, temp:%3d.%d C, timestamp:%5d\n",
                  num, data.data.temp / 10,(rt_uint32_t)data.data.temp % 10,
                  data.timestamp);
        else
            rt_kprintf("read data failed!");
    }
    rt_device_close(g_tsen_dev);

}

static void tsen_polling_mode(void *parameter)
{
    int num = 0;
    struct rt_sensor_data data;

    while (1) {
        for (num = 0; num < g_samples_num; num++) {
            if (rt_device_read(g_tsen_dev, 0, &data, 1) == 1)
                rt_kprintf("num:%3d, temp:%3d.%d C, timestamp:%5d\n", num,
                           data.data.temp / 10,
                           (rt_uint32_t)data.data.temp % 10, data.timestamp);
            else
                rt_kprintf("read data failed!");
        }
        rt_thread_mdelay(g_polling_time);
    }

}

static void tsen_start_polling(void)
{
    g_tsen_thread = rt_thread_create("tsen_polling", tsen_polling_mode,
                                 RT_NULL, THREAD_STACK_SIZE,
                                 THREAD_PRIORITY, THREAD_TIMESLICE);
    if (g_tsen_thread != RT_NULL)
        rt_thread_startup(g_tsen_thread);

    return;
}

static char* tsen_sensor_selected(int sensor_id)
{
    switch (sensor_id) {
    case AIC_TSEN_SENSOR_CPU:
        return "temp_tsen_cpu";
    case AIC_TSEN_SENSOR_GPAI:
        return "temp_tsen_gpai";
    default:
        return NULL;
    }

}

static void cmd_test_tsen(int argc, char **argv)
{
    int c, ret;
    struct aic_tsen_ch tsen_info;
    char *aic_tsen_name = NULL;

    if (argc < 2) {
        cmd_tsen_usage(argv[0]);
        return;
    }

    optind = 0;
    while ((c = getopt_long(argc, argv, sopts, lopts, NULL)) != -1) {
        switch (c) {
        case 's':
            g_sensor_id = atoi(optarg);
            continue;
        case 'n':
            g_samples_num = atoi(optarg);
            continue;
        case 't':
            g_polling_time = atoi(optarg);
            continue;
        case 'h':
            cmd_tsen_usage(argv[0]);
            return;
        default:
            break;
        }
    }

    g_sensor = (rt_sensor_t)g_tsen_dev;
    aic_tsen_name = tsen_sensor_selected(g_sensor_id);

    if (aic_tsen_name == NULL) {
        rt_kprintf("The sensor_id is error\n");
        return;
    }

    g_tsen_dev = rt_device_find(aic_tsen_name);
    if (g_tsen_dev == RT_NULL) {
        rt_kprintf("Failed to find %s device\n", aic_tsen_name);
        return;
    }

    ret = rt_device_open(g_tsen_dev, RT_DEVICE_FLAG_RDONLY);
    if (ret != RT_EOK) {
        rt_kprintf("Failed to open %s device\n", aic_tsen_name);
        return;
    }

    rt_device_control(g_tsen_dev, RT_SENSOR_CTRL_GET_CH_INFO, &tsen_info);

    switch (tsen_info.soft_mode) {
        case AIC_TSEN_SOFT_MODE_SINGLE:
            rt_kprintf("[%s] Starting single mode\n", aic_tsen_name);
            tsen_single_mode();
            break;
        case AIC_TSEN_SOFT_MODE_POLLING:
            rt_kprintf("[%s] Starting polling mode\n", aic_tsen_name);
            tsen_start_polling();
            break;
        default:
            rt_kprintf("Unknown mode %d\n", tsen_info.mode);
        }

    return;
}

MSH_CMD_EXPORT_ALIAS(cmd_test_tsen, test_tsen, tsen device sample);
