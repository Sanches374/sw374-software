/*
 * Copyright (c) 2022, Artinchip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rtthread.h>
#include "rtdevice.h"
#include <aic_core.h>
#include "aic_hal_can.h"

/*
 * This program is used to test the sending and receiving of CAN.
 * You must ensure that there are one CAN modules on the demo board.
 * CAN* sends data and CAN* receives data.
 * can_rx needs to be executed before can_tx.
 */
#define CAN_DEV_NAME                 "can1"
#define CAN_RX_FILTER_ENABLE            0

static struct rt_semaphore g_rx_sem;
static rt_device_t g_can_tx_dev;
static rt_device_t g_can_rx_dev;
static char *g_can_dev_name = CAN_DEV_NAME;
static uint8_t g_can_rx_thread_inted = 0;

static rt_err_t can_self_rx_call(rt_device_t dev, rt_size_t size)
{
    rt_sem_release(&g_rx_sem);

    return RT_EOK;
}

static void can_self_rx_thread(void *parameter)
{
    int i;
    rt_size_t size;
    struct rt_can_msg rxmsg = {0};

    while (1)
    {
        rt_sem_take(&g_rx_sem, RT_WAITING_FOREVER);

        rxmsg.hdr = -1;
        size = rt_device_read(g_can_rx_dev, 0, &rxmsg, sizeof(rxmsg));
        if (!size)
        {
            rt_kprintf("CAN read error\n");
            break;
        }

        rt_kprintf("%s received msg:\nID: 0x%x ", g_can_dev_name, rxmsg.id);

        if (rxmsg.len)
            rt_kprintf("DATA: ");
        for (i = 0; i < rxmsg.len; i++)
        {
            rt_kprintf("%02x ", rxmsg.data[i]);
        }

        rt_kprintf("\n");
    }

    rt_device_close(g_can_rx_dev);
}

int test_can_self_rx(void)
{
    rt_err_t ret = 0;
    rt_thread_t thread;

    if (!g_can_rx_thread_inted)
    {
        /* can rx configuration */
        g_can_rx_dev = rt_device_find(g_can_dev_name);
        if (!g_can_rx_dev)
        {
            rt_kprintf("find %s failed!\n", g_can_dev_name);
            return -RT_ERROR;
        }

        ret = rt_device_open(g_can_rx_dev,
                             RT_DEVICE_FLAG_INT_TX | RT_DEVICE_FLAG_INT_RX);
        if (ret)
        {
            rt_kprintf("%s open failed!\n", g_can_dev_name);
            return -RT_ERROR;
        }

        ret = rt_device_control(g_can_rx_dev, RT_CAN_CMD_SET_BAUD,
                                (void *)CAN1MBaud);
        if (ret)
        {
            rt_kprintf("%s set baudrate failed!\n", g_can_dev_name);
            ret = -RT_ERROR;
            goto __exit;
        }

        //enable CAN RX interrupt
        rt_device_control(g_can_rx_dev, RT_DEVICE_CTRL_SET_INT, NULL);

#if CAN_RX_FILTER_ENABLE
        /* config can rx filter */
        struct rt_can_filter_item items[1] =
        {
            //Only receive standard data frame with ID 0x100~0x1FF
            RT_CAN_FILTER_ITEM_INIT(0x100, 0, 0, 0, 0x700, RT_NULL, RT_NULL),
        };

        struct rt_can_filter_config cfg = {1, 1, items};

        ret = rt_device_control(g_can_rx_dev, RT_CAN_CMD_SET_FILTER, &cfg);
#endif

        rt_device_set_rx_indicate(g_can_rx_dev, can_self_rx_call);

        rt_sem_init(&g_rx_sem, "can_rx_sem", 0, RT_IPC_FLAG_PRIO);

        thread = rt_thread_create("can_rx", can_self_rx_thread, RT_NULL,
                                  2048, 25, 10);
        if (thread != RT_NULL)
        {
            rt_thread_startup(thread);
        }
        else
        {
            rt_kprintf("create can_rx thread failed!\n");
            ret = -RT_ERROR;
        }

        g_can_rx_thread_inted = 1;
        rt_kprintf("The %s received thread is ready...\n", g_can_dev_name);
    }
    else
    {
        rt_kprintf("The %s received thread is running...\n", g_can_dev_name);
    }

    rt_device_control(g_can_rx_dev, RT_CAN_CMD_SET_MODE,
                      (void *)CAN_SELFTEST_MODE);

    return RT_EOK;

__exit:
    rt_device_close(g_can_rx_dev);
    return ret;
}

void test_can_self_parse_msg_data(rt_can_msg_t msg, char * optarg)
{
    char *token;
    uint8_t i = 0, id_received = 0;

    token = strtok(optarg, "#.");

    while (token)
    {
        if (!id_received)
        {
            msg->id = strtoul(token, NULL, 16);
            if (msg->id > 0x7FF)
                msg->ide = 1;
            else
                msg->ide = 0;

            id_received = 1;
        }
        else
        {
            /* frame data */
            msg->rtr = CAN_FRAME_TYPE_DATA;
            msg->data[i++] = strtoul(token, NULL, 16);
            if (i >= 8)
                break;
        }

        token = strtok(NULL, "#.");
    }

    msg->len = i;
}

int test_can_self_tx(char *argv[])
{
    rt_err_t ret = 0;
    rt_size_t  size;
    struct rt_can_msg msg = {0};

    if (!g_can_rx_thread_inted)
    {
        rt_kprintf("Please execute can_rx at first!\n");
        return -RT_EINVAL;
    }

    test_can_self_parse_msg_data(&msg, argv[0]);

    /* can tx configuration */
    g_can_tx_dev = rt_device_find(g_can_dev_name);
    if (!g_can_tx_dev)
    {
        rt_kprintf("find %s failed!\n", g_can_dev_name);
        return -RT_EINVAL;
    }

    ret = rt_device_open(g_can_tx_dev,
                         RT_DEVICE_FLAG_INT_TX | RT_DEVICE_FLAG_INT_RX);
    if (ret)
    {
        rt_kprintf("%s open failed!\n", g_can_dev_name);
        return -RT_ERROR;
    }

    ret = rt_device_control(g_can_tx_dev, RT_CAN_CMD_SET_BAUD, (void *)CAN1MBaud);
    if (ret)
    {
        rt_kprintf("%s set baudrate failed!\n", g_can_dev_name);
        ret = -RT_ERROR;
        goto __exit;
    }

    //enable CAN TX interrupt
    rt_device_control(g_can_tx_dev, RT_DEVICE_CTRL_SET_INT, NULL);

    size = rt_device_write(g_can_tx_dev, 0, &msg, sizeof(msg));
    if (size != sizeof(msg))
    {
        rt_kprintf("can dev write data failed!\n");
        ret = -RT_EIO;
    }

__exit:
    rt_device_close(g_can_tx_dev);
    return ret;
}

static void usage_test_can(char * program)
{
    printf("\n");
    printf("%s - test CAN send frame to itself (loopback mode)\n", program);
    printf("Usage: \n");
    printf("\t%s <can_name> frame_id#frame_data\n", program);
    printf("For example:\n");
    printf("\t%s can1 1a3#11.22.9a.88.ef.00\n", program);
    printf("\n");
}

static int cmd_test_can_loopback(int argc, char *argv[])
{
    if (argc < 2) {
        usage_test_can(argv[0]);
        return -RT_EINVAL;
    }

    g_can_dev_name = argv[1];

    test_can_self_rx();
    test_can_self_tx(&argv[2]);

    return 0;
}

MSH_CMD_EXPORT_ALIAS(cmd_test_can_loopback, test_can,
                     can device loopback sample);
