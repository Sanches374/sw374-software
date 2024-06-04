/*
 * Copyright (c) 2024, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date                Notes
 * 2024-05-27          the first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include "axs15260.h"

#define THREAD_PRIORITY   25
#define THREAD_STACK_SIZE 4096
#define THREAD_TIMESLICE  5

static rt_sem_t     g_axs15260_sem = RT_NULL;
static rt_device_t  g_dev = RT_NULL;
static struct rt_touch_data *g_read_data;
static struct rt_touch_info g_info;

static void axs15260_entry(void *parameter)
{
    g_read_data = (struct rt_touch_data *)rt_malloc(sizeof(struct rt_touch_data) * 2);

    while (1) {
        rt_sem_take(g_axs15260_sem, RT_WAITING_FOREVER);
        rt_memset(g_read_data, 0, sizeof(g_read_data));
        rt_device_read(g_dev, 0, g_read_data, 1);
        switch(g_read_data->event)
        {
            case RT_TOUCH_EVENT_DOWN:
                rt_kprintf("down x: %03d y: %03d\n",
                    g_read_data->x_coordinate, g_read_data->y_coordinate);
                break;

            case RT_TOUCH_EVENT_UP:
                rt_kprintf("up x: %03d y: %03d\n",
                    g_read_data->x_coordinate, g_read_data->y_coordinate);
                break;

            case RT_TOUCH_EVENT_MOVE:
                rt_kprintf("move x: %03d y: %03d\n",
                    g_read_data->x_coordinate, g_read_data->y_coordinate);
                break;

            default:
                break;
        }

        rt_device_control(g_dev, RT_TOUCH_CTRL_ENABLE_INT, RT_NULL);
    }
}

static rt_err_t rx_callback(rt_device_t dev, rt_size_t size)
{
    rt_sem_release(g_axs15260_sem);
    rt_device_control(dev, RT_TOUCH_CTRL_DISABLE_INT, RT_NULL);
    return 0;
}

/* Test function */
static void test_axs15260(void *parameter)
{
    rt_thread_t  axs15260_thread = RT_NULL;

    g_dev = rt_device_find("axs15260");
    if (g_dev == RT_NULL) {
        rt_kprintf("can't find device: axs15260\n");
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
    g_axs15260_sem = rt_sem_create("dsem", 0, RT_IPC_FLAG_FIFO);
    if (g_axs15260_sem == RT_NULL) {
        rt_kprintf("create dynamic semaphore failed.\n");
        return;
    }

    axs15260_thread = rt_thread_create("axs15260",
                                     axs15260_entry,
                                     RT_NULL,
                                     THREAD_STACK_SIZE,
                                     THREAD_PRIORITY,
                                     THREAD_TIMESLICE);

    if (axs15260_thread != RT_NULL)
        rt_thread_startup(axs15260_thread);

    return;
}
MSH_CMD_EXPORT(test_axs15260, test axs15260 sample);
