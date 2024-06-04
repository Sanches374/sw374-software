/*
 * Copyright (c) 2024-2025, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date                Notes
 * 2024-04-22          the first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include "st16xx.h"

#define THREAD_PRIORITY   25
#define THREAD_STACK_SIZE 4096
#define THREAD_TIMESLICE  5

static rt_sem_t     g_st16xx_sem = RT_NULL;
static rt_device_t  g_dev = RT_NULL;
static struct rt_touch_data *g_read_data;
static struct rt_touch_info g_info;

static void st16xx_entry(void *parameter)
{
    rt_device_control(g_dev, RT_TOUCH_CTRL_GET_INFO, &g_info);

    g_read_data = (struct rt_touch_data *)rt_malloc(sizeof(struct rt_touch_data) * g_info.point_num);

    while (1) {
        rt_sem_take(g_st16xx_sem, RT_WAITING_FOREVER);

        if (rt_device_read(g_dev, 0, g_read_data, g_info.point_num) == g_info.point_num) {
            for (rt_uint8_t i = 0; i < g_info.point_num; i++) {
                if (g_read_data[i].event == RT_TOUCH_EVENT_DOWN ||
                    g_read_data[i].event == RT_TOUCH_EVENT_MOVE ||
                    g_read_data[i].event == RT_TOUCH_EVENT_UP) {
                    rt_kprintf("%d %d %d %d\n", g_read_data[i].track_id,
                               g_read_data[i].x_coordinate,
                               g_read_data[i].y_coordinate,
                               g_read_data[i].event);
                }
            }
        }
        rt_device_control(g_dev, RT_TOUCH_CTRL_ENABLE_INT, RT_NULL);
    }
}

static rt_err_t rx_callback(rt_device_t dev, rt_size_t size)
{
    rt_sem_release(g_st16xx_sem);
    rt_device_control(dev, RT_TOUCH_CTRL_DISABLE_INT, RT_NULL);
    return 0;
}

/* Test function */
static void test_st16xx(void *parameter)
{
    rt_thread_t  st16xx_thread = RT_NULL;

    g_dev = rt_device_find("st16xx");
    if (g_dev == RT_NULL) {
        rt_kprintf("can't find device: st16xx\n");
        return;
    }

    if (rt_device_open(g_dev, RT_DEVICE_FLAG_INT_RX) != RT_EOK) {
        rt_kprintf("open device failed!");
        return;
    } else {
        rt_device_control(g_dev, RT_TOUCH_CTRL_GET_INFO, &g_info);
        rt_kprintf("type       :%d\n", g_info.type);
        rt_kprintf("point_num  :%d\n", g_info.point_num);
        rt_kprintf("range_x    :%d\n", g_info.range_x);
        rt_kprintf("range_y    :%d\n", g_info.range_y);
    }

    rt_device_set_rx_indicate(g_dev, rx_callback);
    g_st16xx_sem = rt_sem_create("dsem", 0, RT_IPC_FLAG_FIFO);
    if (g_st16xx_sem == RT_NULL) {
        rt_kprintf("create dynamic semaphore failed.\n");
        return;
    }

    st16xx_thread = rt_thread_create("st16xx",
                                     st16xx_entry,
                                     RT_NULL,
                                     THREAD_STACK_SIZE,
                                     THREAD_PRIORITY,
                                     THREAD_TIMESLICE);

    if (st16xx_thread != RT_NULL)
        rt_thread_startup(st16xx_thread);

    return;
}
MSH_CMD_EXPORT(test_st16xx, test st16xx sample);
