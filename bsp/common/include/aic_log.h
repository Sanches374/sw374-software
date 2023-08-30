/*
 * Copyright (c) 2022, Artinchip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __AIC_LOG_H__
#define __AIC_LOG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#define AIC_LOG_ERR     3
#define AIC_LOG_WARN    4
#define AIC_LOG_INFO    6
#define AIC_LOG_DEBUG   7

#if defined(KERNEL_RTTHREAD)
#include <rtdbg.h>
#if defined(ULOG_USING_FILTER)
#define aic_log(level, tag, fmt, ...) \
        do { \
            if (level <= ulog_global_filter_lvl_get()) \
                rt_kprintf("[%s] %s()%d "fmt, tag, __func__, __LINE__, ##__VA_ARGS__); \
        } while (0)
#else
#define aic_log(level, tag, fmt, ...) \
        do { \
            rt_kprintf("[%s] %s()%d "fmt, tag, __func__, __LINE__, ##__VA_ARGS__);\
        } while (0)
#endif
#elif defined(KERNEL_FREERTOS)
#define aic_log(level, tag, fmt, ...)
#elif defined(KERNEL_BAREMETAL)
#define aic_log(level, tag, fmt, ...)                           \
    do {                                                        \
        if (level <= AIC_BAREMETAL_LOG_LEVEL)                   \
            printf("[%s] %s()%d " fmt, tag, __func__, __LINE__, \
                   ##__VA_ARGS__);                              \
    } while (0)
#endif

#define hal_log_err(fmt, ...)    aic_log(AIC_LOG_ERR, "E", fmt, ##__VA_ARGS__)
#define hal_log_warn(fmt, ...)   aic_log(AIC_LOG_WARN, "W", fmt, ##__VA_ARGS__)
#define hal_log_info(fmt, ...)   aic_log(AIC_LOG_INFO, "I", fmt, ##__VA_ARGS__)

#if defined(ULOG_OUTPUT_LVL) && (ULOG_OUTPUT_LVL < AIC_LOG_DEBUG)
#define hal_log_debug(fmt, ...) \
    do { \
    } while (0)
#else
#define hal_log_debug(fmt, ...) aic_log(AIC_LOG_DEBUG, "D", fmt, ##__VA_ARGS__)
#endif

#ifndef pr_fmt
#define pr_fmt(fmt) fmt
#endif

#define pr_emerg(fmt, ...)   hal_log_err(pr_fmt(fmt), ##__VA_ARGS__)
#define pr_alert(fmt, ...)   hal_log_err(pr_fmt(fmt), ##__VA_ARGS__)
#define pr_crit(fmt, ...)    hal_log_err(pr_fmt(fmt), ##__VA_ARGS__)
#define pr_err(fmt, ...)     hal_log_err(pr_fmt(fmt), ##__VA_ARGS__)
#define pr_warn(fmt, ...)    hal_log_warn(pr_fmt(fmt), ##__VA_ARGS__)
#define pr_notice(fmt, ...)  hal_log_info(pr_fmt(fmt), ##__VA_ARGS__)
#define pr_info(fmt, ...)    hal_log_info(pr_fmt(fmt), ##__VA_ARGS__)
#define pr_debug(fmt, ...)   hal_log_debug(pr_fmt(fmt), ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif
