#ifndef WIFI_CONNECT_H
#define WIFI_CONNECT_H

#include <rtthread.h>

// 配置ulog日志
#define LOG_TAG "wifi_connect"
#include <ulog.h>

// Lwip
#include <lwip/netif.h>
#include <lwip/dhcp.h>
#include <lwip/init.h>

// wifi
#include <wifi/wifi_conf.h>

// #include <rtconfig.h>

// 配置文件存放地址


// 从文件系统中读取wifi配置文件内容
// int dfs_read_from_file(char *buf, size_t buf_size, const char * file_dir);
// int get_wifi_info(char ** ssid, char ** pwd, char ** version);
int get_ip_via_dhcp(void);
int sw374_wifi_connect(char *, char *);

#endif