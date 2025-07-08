/*
 * 立创开发板软硬件资料与相关扩展板软硬件资料官网全部开源
 * 开发板官网：www.lckfb.com
 * 文档网站：wiki.lckfb.com
 * 技术支持常驻论坛，任何技术问题欢迎随时交流学习
 * 嘉立创社区问答：https://www.jlc-bbs.com/lckfb
 * 关注bilibili账号：【立创开发板】，掌握我们的最新动态！
 * 不靠卖板赚钱，以培养中国工程师为己任
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <sys/time.h>
#include <rtthread.h>
#include "rtdevice.h"
#include "aic_core.h"
#include "aic_hal_gpio.h"

#define THREAD_PRIORITY         25     // 线程优先级，数字越大优先级越高
#define THREAD_STACK_SIZE       512    // 线程堆栈大小，决定了线程可以使用的内存空间
#define THREAD_TIMESLICE        5      // 线程时间片，决定了线程在调度时能占用CPU的最长时间

static rt_thread_t led_thread = RT_NULL; // 声明线程控制块指针，用于后续创建和管理线程
static rt_base_t user_led_pin = RT_NULL; // 声明引脚编号变量，初始化为NULL，用于存储LED的引脚编号

// 线程入口函数，这是线程启动后执行的函数
static void user_led_thread_entry(void *param)
{
    while(1) // 无限循环，使线程持续运行
    {
        rt_pin_write(user_led_pin, PIN_HIGH); // 将LED引脚电平设置为高
        rt_thread_mdelay(100); // 线程延时100毫秒，让LED保持高电平一段时间

        rt_pin_write(user_led_pin, PIN_LOW); // 将LED引脚电平设置为低
        rt_thread_mdelay(100); // 线程延时100毫秒，让LED保持低电平一段时间
    }
}

// 初始化LED的函数，在系统启动时调用
static void usr_led_run(int argc, char **argv)
{
    user_led_pin = rt_pin_get("PE.17"); // 获取名为"PE.17"的引脚编号
    if ((user_led_pin == -RT_EINVAL) || (user_led_pin == -RT_ENOSYS)) // 如果获取失败，打印错误信息并返回
    {
        rt_kprintf("Failed to get the pin PE.17\n");
        return; // 获取引脚失败，直接返回，避免后续操作导致错误
    }

    rt_pin_mode(user_led_pin, PIN_MODE_OUTPUT); // 设置引脚为输出模式

    rt_pin_write(user_led_pin, PIN_LOW); // 初始时将LED设置为低电平

    /* 创建线程，名称是 led_thread，入口是 user_led_thread_entry */
    led_thread = rt_thread_create("led_thread", // 线程名称
                                  user_led_thread_entry, RT_NULL, // 线程入口函数和参数
                                  THREAD_STACK_SIZE, // 线程堆栈大小
                                  THREAD_PRIORITY, // 线程优先级
                                  THREAD_TIMESLICE); // 线程时间片
    if (led_thread == RT_NULL) // 如果线程创建失败，打印错误信息并返回
    {
        rt_kprintf("Failed to create the led_thread\n");
        return; // 线程创建失败，直接返回，防止对无效的线程进行操作
    }

    /* 如果获得线程控制块，启动这个线程 */
    rt_thread_startup(led_thread); // 启动线程，使其开始执行
}

// 导出函数自动运行，在系统初始化时调用usr_led_run函数
INIT_APP_EXPORT(usr_led_run);
