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

extern int lvgl_thread_init(void);

#ifdef __cplusplus
}
#endif

#include <string>
// cpp 文件
#ifdef SW374_TCP_SERVER
#include "sw_tcp_server.h"
#endif

#ifdef SW374_LVGL_UI
#include "lvgl_sw_thread.h"
#include "LVGL_SW_F1_Panel_1.h"
#include "LVGL_SW_Log_Panel.h"
#endif

#ifdef SW374_WS2812B
#include "sw_ws2812_thread.h"
#endif

#ifdef SW374_TELEMETRY_DATA_PROCESSOR
#include "telemetry_data_processor.h"
#endif

#define LOG_TAG "SW374-main"
#include <ulog.h>

// 配置参数
#include "SW_conf.h"

//----------------------------全局变量定义区------------------------------------//
// 消息队列全局定义
rt_mq_t cJson_msg_queue = RT_NULL;
// 遥测数据处理器
#ifdef SW374_TELEMETRY_DATA_PROCESSOR
DataProcessor data_processor;
#endif

#ifdef SW374_LVGL_UI
LVGL_SW_Base_Panel * p_panel = nullptr;
LVGL_SW_Log_Panel * p_logpanel = nullptr;
#endif

#ifdef SW374_WS2812B
SW_WS2812 * p_SW_WS2812 = nullptr;
#endif

// 新增：全局标志和信号量  遥测数据传输连接成功标志
// rt_bool_t telemetry_data_connected_flag = RT_FALSE; //client_connected_flag
rt_sem_t telemetry_data_connected_sem = RT_NULL; //client_connected_sem
//-----------------------------------------------------------------------------//
int main(void){}

#ifdef SW374_JTAG_DEBUG_CONFIG
void debug_enter(void){}
#endif

// void gpio_detect(void)
// {
    
//     LOG_I("GPIO PA0 detected.");
//     bool flag = true;
//     bool last = flag;
//     rt_pin_mode(BOOT_GPIO_PIN,  PIN_MODE_INPUT_PULLUP);    //配置引脚为上拉输入模式
//     while (1)
//     {
//         if (rt_pin_read(BOOT_GPIO_PIN) == PIN_HIGH)
//         {
//             rt_thread_mdelay(5);
//             if (rt_pin_read(BOOT_GPIO_PIN) == PIN_HIGH)
//                 flag = true;
//         }
//         else
//         {
//             rt_thread_mdelay(5);
//             if (rt_pin_read(BOOT_GPIO_PIN) == PIN_LOW)
//                 flag = false;
//         }
                
        
            
//         if (flag != last)
//         {
//             last = flag;
//             if (flag)
//                 LOG_I("GPIO PA2 is high");
//             else
//                 LOG_I("GPIO PA2 is low");
//         }
//         rt_thread_mdelay(5);
        
//     }
// }

// MSH_CMD_EXPORT(gpio_detect, gpio_detect);

int sw374_main(void)
{
//  等待1秒后进入初始化
    char * wifissid = NULL;
    char * wifipwd = NULL;
    char * sw374version_from_config = NULL;
    int tcp_port = 0;
    int ret = 0;
    // 初始化信号量
    telemetry_data_connected_sem = rt_sem_create("tel_conn_sem", 0, RT_IPC_FLAG_FIFO);
    if (telemetry_data_connected_sem == RT_NULL) {
        LOG_E("Failed to create telemetry data connected semaphore");
        return -1;
    }

    rt_thread_mdelay(1000); // 等待 1 秒
    LOG_I("Steering Wheel 374 now is initializing ...");
    

#ifdef SW374_JTAG_DEBUG_CONFIG
    // 调试模式按键检测
    LOG_I("GPIO PA2 debug pin detection enabled");
    rt_pin_mode(BOOT_GPIO_PIN, PIN_MODE_INPUT_PULLUP);
    
    // 等待按键按下
    while (rt_pin_read(BOOT_GPIO_PIN) == PIN_HIGH) {
        rt_thread_mdelay(50);
    }
    
    // 消抖处理
    rt_thread_mdelay(20);
    if (rt_pin_read(BOOT_GPIO_PIN) == PIN_LOW) {
        LOG_I("Debug entry triggered");
        debug_enter();
    }
#endif
    
// 开启页面显示
#ifdef SW374_LVGL_UI
    LOG_I("Initializing LVGL display system ...");
    lvgl_thread_init();
    // 等待 1 秒 点亮屏幕
    rt_thread_mdelay(1000);
    p_logpanel = new LVGL_SW_Log_Panel();
    LOG_I("LVGL display initialized successfully!");
    if (p_logpanel) p_logpanel->updateLogDisplay("LVGL display initialized successfully!");



    // delete p_logpanel;
    // p_panel = new LVGL_SW_F1_Panel_1(); // 创建一个新的LVGL_SW_F1_Panel_1对象
    // delete p_panel;
    // p_panel = nullptr;
    // p_logpanel = new LVGL_SW_Log_Panel();
    
#endif

// WS2812 启动
#ifdef SW374_WS2812B
    LOG_I("Initializing WS2812B LED system ...");
    if (p_logpanel) p_logpanel->updateLogDisplay("Initializing WS2812B LED system ...");
    SW_2812_thread = new SW_WS2812_thread("WS2812", WS2812_THREAD_STACK_SIZE, WS2812_THREAD_PRIORITY, WS2812_THREAD_TIMESLICE); // 动态创建
    SW_2812_thread->start();
    LOG_I("WS2812B LED system initialized successfully!");
    if (p_logpanel) p_logpanel->updateLogDisplay("WS2812B LED system initialized successfully!");
#endif

// 读取配置文件
#ifdef SW374_DFS_FILE_CONFIG
    LOG_I("Reading system configuration from config.conf.");
    if (p_logpanel) p_logpanel->updateLogDisplay("Reading system configuration from config.conf.");
    int read_config_file_count = 1;
    while (get_config_info(&wifissid, &wifipwd, &tcp_port, &sw374version_from_config) != 0)
    {
        // 配置文件读取失败
        rt_thread_mdelay(1000);
        LOG_E("Failed to read config file (attempt %d/%d)", read_config_file_count, int(READ_CONFIG_FILE_MAX_TIME));
        if (p_logpanel) p_logpanel->updateLogDisplay(std::string("Failed to read config file (attempt ") + std::to_string(read_config_file_count) + std::string("/ ") + std::to_string(int(READ_CONFIG_FILE_MAX_TIME)) + std::string(")"));
        if (read_config_file_count ++ >= READ_CONFIG_FILE_MAX_TIME)
        {
            LOG_E("Failed to read config file after %d attempts! SW374_main exit!", int(READ_CONFIG_FILE_MAX_TIME));
            if (p_logpanel) 
            {
                p_logpanel->updateLogDisplay(std::string("Failed to read config file after ") + std::to_string(int(READ_CONFIG_FILE_MAX_TIME)) + std::string(" attempts!")); //
                p_logpanel->updateLogDisplay(std::string("SW374_main exit! Please verify the configuration file and reset the board."));
            }
            goto cleanup;
        }
    }
    LOG_I("Configuration loaded successfully");
    if (p_logpanel) p_logpanel->updateLogDisplay("Configuration loaded successfully!");
    LOG_I("Config version: %s", sw374version_from_config);
    if (p_logpanel) p_logpanel->updateLogDisplay(std::string("Config version:") + std::string(sw374version_from_config));
    LOG_I("Software version: %s", SW374_software_version);
    if (p_logpanel) p_logpanel->updateLogDisplay(std::string("Software version:") + std::string(SW374_software_version));
    LOG_I("WiFi SSID: %s", wifissid);
    // if (p_logpanel) p_logpanel->updateLogDisplay(std::string("WiFi SSID:") + std::string(wifissid));
    // LOG_I("WiFi password: %s", wifipwd);
    // if (p_logpanel) p_logpanel->updateLogDisplay(std::string("WiFi password:") + std::string(wifipwd));
#endif

// 连接wifi
#ifdef SW374_WIFI_CONNECT
    LOG_I("Connecting to WiFi network: %s", wifissid);
    if (p_logpanel) p_logpanel->updateLogDisplay(std::string("Connecting to WiFi network:") + std::string(wifissid));
    ret = sw374_wifi_connect(wifissid, wifipwd);
    // ret = sw374_wifi_connect("Sanches", "Sanches374");
    if (ret != 0) {
        LOG_E("WiFi connection failed");
        if (p_logpanel) p_logpanel->updateLogDisplay("WiFi connection failed");
        goto cleanup;
    }
    // dhcp客户端
    char ip_addr[40]; // 足够存储 IPv4 地址
    if (get_ip_via_dhcp(ip_addr, sizeof(ip_addr)) == 0) {
        LOG_I("Obtained IP address: %s", ip_addr);
        if (p_logpanel) p_logpanel->updateLogDisplay(std::string("Obtained IP address:") + std::string(ip_addr));
    }
    else
    {
        LOG_I("Failed to get ip address!");
        if (p_logpanel) p_logpanel->updateLogDisplay(std::string("Failed to get ip address"));
        goto cleanup;
    }

    LOG_I("WiFi connected successfully");
    if (p_logpanel) p_logpanel->updateLogDisplay("WiFi connected successfully");

#endif



// 开启遥测数据处理
#ifdef SW374_TELEMETRY_DATA_PROCESSOR
    LOG_I("Initializing telemetry data processor ...");
    if (p_logpanel) p_logpanel->updateLogDisplay("Initializing telemetry data processor ...");
    data_proc_system_init();
    LOG_I("Telemetry data processor initialized successfully!");
    if (p_logpanel) p_logpanel->updateLogDisplay("Telemetry data processor initialized successfully!");
#endif




// 建立TCPserver
#ifdef SW374_TCP_SERVER
    LOG_I("Starting TCP server on port : %d", tcp_port);
    if (p_logpanel) p_logpanel->updateLogDisplay(std::string("Starting TCP server on port : ") + std::to_string(tcp_port));
    start_tcp_server(tcp_port);
    LOG_I("TCP server started successfully!");
    if (p_logpanel) p_logpanel->updateLogDisplay("TCP server started successfully!");

    // 接收TCPserver的接入消息
    LOG_I("Waiting for tcp client connection...");
    if (p_logpanel) p_logpanel->updateLogDisplay("Waiting for tcp client connection...");

    ret = rt_sem_take(telemetry_data_connected_sem, RT_WAITING_FOREVER);
    if (ret != RT_EOK) {
        LOG_E("Failed to take client connected semaphore!");
        if (p_logpanel) p_logpanel->updateLogDisplay("Failed to take client connected semaphore!");
        if (telemetry_data_connected_sem != RT_NULL) 
        {
            rt_sem_delete(telemetry_data_connected_sem);
        }
        goto cleanup;
    }

    LOG_I("Tcp client connected!");
    if (p_logpanel) p_logpanel->updateLogDisplay("Tcp client connected!");
#endif
    LOG_I("Steering Wheel 374 initialization completed successfully!");
    if (p_logpanel) p_logpanel->updateLogDisplay("Steering Wheel 374 initialization completed successfully!");
    LOG_I("The dashboard is being opened.");

#ifdef SW374_JTAG_DEBUG_CONFIG
    if (p_logpanel) p_logpanel->updateLogDisplay("The dashboard is being opened...Please press the debug button!");
    // 调试模式按键检测
    // 再次按下启动页面
    LOG_I("GPIO PA2 debug pin detection enabled");
    rt_pin_mode(BOOT_GPIO_PIN, PIN_MODE_INPUT_PULLUP);
    
    // 等待按键按下
    while (rt_pin_read(BOOT_GPIO_PIN) == PIN_HIGH) {
        rt_thread_mdelay(50);
    }
    
    // 消抖处理
    rt_thread_mdelay(20);
    if (rt_pin_read(BOOT_GPIO_PIN) == PIN_LOW) {
        LOG_I("Debug entry triggered");
        debug_enter();
    }
#else
    if (p_logpanel) p_logpanel->updateLogDisplay("The dashboard is being opened...Please wait!");
    rt_thread_mdelay(2000);
#endif

    delete p_logpanel;
    p_panel = new LVGL_SW_F1_Panel_1(); // 创建一个新的LVGL_SW_F1_Panel_1对象
    return 0;

cleanup:
    LOG_E("Steering Wheel 374 initialization failed!");
    if (p_logpanel) p_logpanel->updateLogDisplay("Steering Wheel 374 initialization failed!");
    delete p_logpanel;
    return -1;
}

// MSH_CMD_EXPORT(sw374_main, sw374_main);
INIT_LATE_APP_EXPORT(sw374_main);