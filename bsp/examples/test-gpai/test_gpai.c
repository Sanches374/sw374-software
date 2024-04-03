/*
 * Copyright (c) 2022-2023, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors: Siyao.Li <lisy@artinchip.com>
 */
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <sys/time.h>
#include <rtthread.h>

#include "hal_adcim.h"
#include "rtdevice.h"
#include "aic_core.h"
#include "aic_log.h"

/* Global macro and variables */
#define AIC_GPAI_NAME               "gpai"
/* The default voltages are set to D21x->3.0V, D31x、D12x->2.5V */
#define AIC_GPAI_DEFAULT_VOLTAGE    3
#define AIC_GPAI_VOLTAGE_ACCURACY   10000

static rt_adc_device_t gpai_dev;
static const char sopts[] = "c:t:n:h";
static const struct option lopts[] = {
    {"channel", required_argument, NULL, 'c'},
    {"voltage", required_argument, NULL, 't'},
    {"number",  required_argument, NULL, 'n'},
    {"help",          no_argument, NULL, 'h'},
    {0, 0, 0, 0}
    };

static int g_sample_num = -1;
/* Functions */

static void cmd_gpai_usage(char *program)
{
    printf("Compile time: %s %s\n", __DATE__, __TIME__);
    printf("Usage: %s [options]\n", program);
    printf("\t -c, --channel\t\tSelect one channel in [0, %d], default is 0\n",
           AIC_GPAI_CH_NUM - 1);
    printf("\t -t, --voltage\t\tModify default voltage\n");
    printf("\t -n, --number\t\tSet the number of samples\n");
    printf("\t -h, --help \n");
    printf("\n");
    printf("Example: %s -c 4 -n 100 -t 3\n", program);
}

static int gpai_get_adc(long chan, float def_voltage)
{
    int val;
    int cnt = 0;
    u32 cal_param;
    int voltage = 0;
    int last_irq_count = 0;
    int current_irq_count = 0;
    int scale = AIC_GPAI_VOLTAGE_ACCURACY;
    int sample_cnt = g_sample_num;

    cal_param = hal_adcim_auto_calibration();
    gpai_dev = (rt_adc_device_t)rt_device_find(AIC_GPAI_NAME);
    if (!gpai_dev) {
        rt_kprintf("Failed to open %s device\n", AIC_GPAI_NAME);
    }

    rt_adc_enable(gpai_dev, chan);
    while(sample_cnt) {
        while(1) {
            current_irq_count = rt_adc_control(gpai_dev, RT_ADC_CMD_IRQ_COUNT,
                                           (void *)chan);
            if (current_irq_count != last_irq_count) {
                last_irq_count = current_irq_count;
                break;
            }
        }

        cnt++;
        val = rt_adc_read(gpai_dev, chan);
        rt_kprintf("[%d] GPAI ch%d: %d\n", cnt, chan, val);

        if (val) {
            voltage = hal_adcim_adc2voltage(val, cal_param,
                                            AIC_GPAI_VOLTAGE_ACCURACY,
                                            def_voltage);
            rt_kprintf("GPAI ch%d-voltage:%d.%04d v\n", chan, voltage / scale,
                       voltage % scale);
        }
        sample_cnt--;
    }

    rt_adc_disable(gpai_dev, chan);

    return voltage;
}

static int cmd_test_gpai(int argc, char **argv)
{
    int c;
    u32 ch = 0;
    float def_voltage = AIC_GPAI_DEFAULT_VOLTAGE;
    int voltage = -1;

    if (argc < 2) {
        cmd_gpai_usage(argv[0]);
        return voltage;
    }

    optind = 0;
    while ((c = getopt_long(argc, argv, sopts, lopts, NULL)) != -1) {
        switch (c) {
        case 'c':
            ch = atoi(optarg);
            if ((ch < 0) || (ch >= AIC_GPAI_CH_NUM))
                pr_err("Invalid channel No.%s\n", optarg);
            break;
        case 't':
            def_voltage = atof(optarg);
            break;
        case 'n':
            g_sample_num = atoi(optarg);
            break;
        case 'h':
        default:
            cmd_gpai_usage(argv[0]);
            return voltage;
        }
    }

    if (ch < 0) {
        rt_kprintf("Please select a channel first\n");
        return voltage;
    }
    if (def_voltage < 0) {
        rt_kprintf("Please input valid standard voltage\n");
        return voltage;
    }

    if (g_sample_num <= 0) {
        rt_kprintf("Please set the number of samples\n");
        return voltage;
    }
    voltage = gpai_get_adc(ch, def_voltage);

    return voltage;
}

MSH_CMD_EXPORT_ALIAS(cmd_test_gpai, test_gpai, gpai device sample);
