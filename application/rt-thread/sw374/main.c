/*
 * Copyright (c) 2022-2024, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors: weilin.peng@artinchip.com
 */

#include <rtthread.h>

#ifdef SW374_DFS_FILE_CONFIG
#include "sw_dfs_file_read.h"
#endif

#ifdef SW374_WIFI_CONNECT
#include "sw_wifi_connect.h"
#endif

#ifdef SW374_TCP_SERVER
#include "sw_TCP_server.h"
#endif


#define LOG_TAG "SW374-main"
#include <ulog.h>


int main(void)
{
//  等待1秒后进入初始化
    char * wifissid = NULL;
    char * wifipwd = NULL;
    char * sw374version = NULL;

    rt_thread_mdelay(1000); // 等待 1 秒
    LOG_I("Steering Wheel 374 now is initializing ...");

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

// 建立TCPserver
#ifdef SW374_TCP_SERVER
    LOG_I("TCP server is starting ...");
    start_tcp_server();
#endif





    return 0;
}

MSH_CMD_EXPORT(main, main)
