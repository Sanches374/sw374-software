/*
 * Copyright (c) 2022-2024, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors: weilin.peng@artinchip.com
 */

#ifdef __cplusplus
extern "C" {
#endif
// c程序
#include <rtthread.h>

#include "rtdevice.h"
// #include "aic_hal_gpio.h"

#define BOOT_GPIO_PIN    rt_pin_get("PA.2")
#ifdef SW374_DFS_FILE_CONFIG
#include "sw_dfs_file_read.h"
#endif

#ifdef SW374_WIFI_CONNECT
#include "sw_wifi_connect.h"
#endif

#ifdef __cplusplus
}
#endif


// cpp 文件
#ifdef SW374_TCP_SERVER
#include "sw_tcp_server.h"
#endif

#ifdef SW374_LVGL_UI
#include "lvgl_sw_thread.h"
#endif

#ifdef SW374_TELEMETRY_DATA_PROCESSOR
#include "telemetry_data_processor.h"
#endif

#define LOG_TAG "SW374-main"
#include <ulog.h>

// 全局变量定义区
/* 消息队列全局定义 */
rt_mq_t cJson_msg_queue = RT_NULL;
/* 全局对象 */
#ifdef SW374_TELEMETRY_DATA_PROCESSOR
DataProcessor data_processor;
#endif
int main(void){}
void gpio_detect(void)
{
    LOG_I("GPIO PA0 detected.");
    bool flag = true;
    bool last = flag;
    rt_pin_mode(BOOT_GPIO_PIN,  PIN_MODE_INPUT_PULLUP);    //配置引脚为上拉输入模式
    while (1)
    {
        if (rt_pin_read(BOOT_GPIO_PIN) == PIN_HIGH)
        {
            rt_thread_mdelay(5);
            if (rt_pin_read(BOOT_GPIO_PIN) == PIN_HIGH)
                flag = true;
        }
        else
        {
            rt_thread_mdelay(5);
            if (rt_pin_read(BOOT_GPIO_PIN) == PIN_LOW)
                flag = false;
        }
                
        
            
        if (flag != last)
        {
            last = flag;
            if (flag)
                LOG_I("GPIO PA0 is high");
            else
                LOG_I("GPIO PA0 is low");
        }
        rt_thread_mdelay(5);
        
    }
}

MSH_CMD_EXPORT(gpio_detect, gpio_detect);

int sw374_main(void)
{
//  等待1秒后进入初始化
    char * wifissid = NULL;
    char * wifipwd = NULL;
    char * sw374version = NULL;

    rt_thread_mdelay(1000); // 等待 1 秒
    LOG_I("Steering Wheel 374 now is initializing ...");


// 页面检测

    LOG_I("GPIO PA2 detected...");
    rt_pin_mode(BOOT_GPIO_PIN,  PIN_MODE_INPUT_PULLUP);    //配置引脚为上拉输入模式
    while (rt_pin_read(BOOT_GPIO_PIN) == PIN_HIGH)
    {
        if (rt_pin_read(BOOT_GPIO_PIN) == PIN_LOW)
        {
            rt_thread_mdelay(5);
            if (rt_pin_read(BOOT_GPIO_PIN) == PIN_LOW)
                break;
        }
        rt_thread_mdelay(100);
    }

// 开启页面显示
#ifdef SW374_LVGL_UI
    LOG_I("LVGL display is starting ...");
    // extern LVGL_SW_Base_Panel * p_pannel;
    // p_pannel = new LVGL_SW_F1_Panel_1();
    // lvgl_sw_thread();
#endif



// 读取配置文件
#ifdef SW374_DFS_FILE_CONFIG
    LOG_I("Reading file \'config.conf\' ...");
    get_config_info(&wifissid, &wifipwd, &sw374version);
#endif

// 连接wifi
#ifdef SW374_WIFI_CONNECT
    LOG_I("Wifi is connecting ...");
    sw374_wifi_connect(wifissid, wifipwd);
#endif



// 开启遥测数据处理
#ifdef SW374_TELEMETRY_DATA_PROCESSOR
    LOG_I("Telemetry data processor is starting ...");
    data_proc_system_init();
#endif

// 建立TCPserver
#ifdef SW374_TCP_SERVER
    LOG_I("TCP server is starting ...");
    start_tcp_server(10001);
#endif

    return 0;
}

MSH_CMD_EXPORT(sw374_main, sw374_main);
INIT_APP_EXPORT(sw374_main);
