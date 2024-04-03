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
#include <rtthread.h>

#include "rtdevice.h"
#include "aic_core.h"
#include "aic_log.h"
#include "rtdevice.h"
#include "hal_psadc.h"

/* Global macro and variables */
#define AIC_PSADC_NAME               "psadc"
#define AIC_PSADC_ADC_MAX_VAL        0xFFF
#define AIC_PSADC_DEFAULT_VOLTAGE    3
#define AIC_PSADC_QC_MODE            0

static rt_adc_device_t psadc_dev;
static const char sopts[] = "rt:sh";
static const struct option lopts[] = {
    {"read",          no_argument, NULL, 'r'},
    {"voltage", required_argument, NULL, 't'},
    {"status",        no_argument, NULL, 's'},
    {"help",          no_argument, NULL, 'h'},
    {0, 0, 0, 0}
    };

/* Functions */

static void cmd_psadc_usage(char *program)
{
    printf("Compile time: %s %s\n", __DATE__, __TIME__);
    printf("Usage: %s [options]\n", program);
    printf("\t -r, --read\t\tRead the adc value\n");
    printf("\t -t, --voltage\t\tInput standard voltage, default is 3\n");
    printf("\t -s, --status\t\tShow more hardware information\n");
    printf("\t -h, --help \n");
    printf("\n");
    printf("Example: %s -r -t 3\n", program);
}

static void adc2voltage(float st_voltage, int adc_value)
{
    int voltage;

    voltage = (adc_value * st_voltage * 100) / AIC_PSADC_ADC_MAX_VAL;
    rt_kprintf(" %d.%2dv", voltage / 100, voltage % 100);
    return;
}

int psadc_get_adc(float st_voltage)
{
    int ret = 0;
    u32 adc_values[AIC_PSADC_CH_NUM];
    int cnt = 0;
    int chan_cnt = 0;
    u64 start_us, end_us;

    psadc_dev = (rt_adc_device_t)rt_device_find(AIC_PSADC_NAME);
    if (!psadc_dev) {
        rt_kprintf("Failed to open %s device\n", AIC_PSADC_NAME);
        return -RT_ERROR;
    }

    rt_adc_enable(psadc_dev, AIC_PSADC_QC_MODE);
    chan_cnt = rt_adc_control(psadc_dev, RT_ADC_CMD_GET_CHAN_COUNT, NULL);
    rt_kprintf("Starting sampling for %d channels\n", chan_cnt);

    while (cnt < 10) {
        cnt++;

        start_us = aic_get_time_us();
        ret = rt_adc_control(psadc_dev, RT_ADC_CMD_GET_VALUES_POLL,
                             (void *)adc_values);
        end_us = aic_get_time_us();
        rt_kprintf("Sample time: %d us\n", abs(end_us - start_us));
        if (ret < 0) {
            rt_kprintf("Read timeout!\n");
            return -RT_ERROR;
        }
        // aic_udelay(10);

        rt_kprintf("[%d] PSADC: ", cnt);
        for (int i = 0; i < chan_cnt; i++) {
            rt_kprintf(" %d", adc_values[i]);
        }
        rt_kprintf("\nvoltage: ");
        for (int i = 0; i < chan_cnt; i++) {
            adc2voltage(st_voltage, adc_values[i]);
        }
        rt_kprintf("\n");

    }
    rt_adc_disable(psadc_dev, AIC_PSADC_QC_MODE);

    return -RT_ERROR;
}


static void cmd_test_psadc(int argc, char **argv)
{
    int c;
    float st_voltage = AIC_PSADC_DEFAULT_VOLTAGE;
    bool show_status = false;

    if (argc < 2) {
        cmd_psadc_usage(argv[0]);
        return;
    }

    optind = 0;
    while ((c = getopt_long(argc, argv, sopts, lopts, NULL)) != -1) {
        switch (c) {
        case 'r':
            psadc_get_adc(st_voltage);
            break;
        case 't':
            st_voltage = atof(optarg);
            break;
        case 's':
            show_status = true;
            break;
        case 'h':
        default:
            cmd_psadc_usage(argv[0]);
            return;
        }
    }

    if (show_status) {
        aich_psadc_status_show();
        aicos_msleep(10);
        return;
    }

    if (st_voltage < 0) {
        rt_kprintf("Please input standard voltage\n");
        return;
    }

    return;
}

MSH_CMD_EXPORT_ALIAS(cmd_test_psadc, test_psadc, psadc device sample);
