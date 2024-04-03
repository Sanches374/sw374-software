/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date         Author          Notes
 * 2018-08-15   misonyo         first implementation.
 * 2023-05-25   geo.dong        ArtInChip
 */

#include <string.h>
#include <rtthread.h>
#include <aic_core.h>
#define SAMPLE_UART_NAME "uart4"

static struct rt_semaphore rx_sem;
static rt_device_t serial;
static char str_send[] = "1234567890ArtInChip1234567890\n";
static int g_recv_max = 128;
static int uart_test_result = 1;
static int g_exit = 0;

rt_err_t uart_input(rt_device_t dev, rt_size_t size)
{
    if (size > 0)
        rt_sem_release(&rx_sem);

    return RT_EOK;
}

void serial_thread_entry(void *parameter)
{
    char ch;
    int ret = 0;
    char str_receive[128 + 1] = {0};
    int cnt = 0;

    while (1) {
        ch = 0;
        ret = rt_device_read(serial, -1, &ch, 1);
        if (ret == 1) {
            str_receive[cnt % 30] = ch;
            // pr_debug("%d/%d: Receive %c[0x%x]\n", cnt, g_recv_max, ch, ch);
            cnt++;
            if (cnt > g_recv_max)
                break;

            if (cnt % 30 == 0 || ch == '\n') {
                printf("Send: |%s| \n", str_send);
                printf("Recv: |%s| \n", str_receive);
                uart_test_result = strncmp(str_send, str_receive, cnt);
                break;
            }
        } else {
            if (ret < 0)
                pr_debug("read() return [%ld] %s\n",
                         rt_get_errno(), rt_strerror(rt_get_errno()));
            rt_sem_take(&rx_sem, RT_WAITING_FOREVER);
            aicos_msleep(100);
        }
    }

    printf("test_uart received %d bytes, then exit\n", cnt);
    rt_sem_detach(&rx_sem);
    rt_thread_delete(rt_thread_self());
    rt_device_close(serial);
    g_exit = 1;
}


int test_uart(int argc, char *argv[])
{
    rt_err_t ret = RT_EOK;
    static rt_uint8_t open_cnt = 0;
    char uart_name[RT_NAME_MAX] = SAMPLE_UART_NAME;

    if (argc > 1)
        rt_strncpy(uart_name, argv[1], RT_NAME_MAX);

    if (argc > 2)
        g_recv_max = atoi(argv[2]);

    g_exit = 0;
    printf("Try to open(%s), and recieve %d bytes\n", uart_name, g_recv_max);
    serial = rt_device_find(uart_name);
    if (!serial)
    {
        rt_kprintf("find %s failed!\n", uart_name);
        return -RT_ERROR;
    }

    ret = rt_device_open(serial, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
    if (ret != RT_EOK)
    {
        rt_kprintf("open %s failed : %d !\n", uart_name, ret);
        return -RT_ERROR;
    }

    /* ensure to initialize only once */
    if (open_cnt == 0) {
        rt_device_set_rx_indicate(serial, uart_input);
        rt_sem_init(&rx_sem, "rx_sem", 0, RT_IPC_FLAG_FIFO);
    }

    /* NOTE: thread stack-size at least for 1024*2 Bytes !!! */
    rt_thread_t thread = rt_thread_create("serial", serial_thread_entry, RT_NULL, 1024*2, 25, 10);
    if (thread != RT_NULL) {
        rt_thread_startup(thread);
    } else {
        rt_device_close(serial);
        return -RT_ERROR;
    }

    open_cnt++;
    rt_device_write(serial, 0, str_send, (sizeof(str_send) - 1));
    /* obtain return value of the test result */
    while (1) {
        if (g_exit == 1) break;
        rt_thread_mdelay(10);
    }
    return uart_test_result;
}

MSH_CMD_EXPORT(test_uart, ArtInChip Uart Test);
