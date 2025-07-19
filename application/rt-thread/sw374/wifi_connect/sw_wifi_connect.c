#include "sw_wifi_connect.h"
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


// 通过dhcp获取ip
int get_ip_via_dhcp(void)
{
    // 1. 查找网络接口 (WL0 是示例接口名)
    struct netif *netif = netif_find("WL0");
    
    if (netif == NULL) {
        LOG_E("Network interface not found!");
        return 0;
    }

    // 2. 确保接口已启用
    netif_set_up(netif);
    
    // 3. 启动 DHCP 客户端
    err_t err = dhcp_start(netif);
    
    if (err != ERR_OK) {
        LOG_E("Failed to start DHCP client: %d", err);
        return 0;
    }
    
    LOG_I("DHCP client started. Waiting for IP address...");
    
    // 4. 等待 DHCP 分配 IP (简单实现，实际应用中可能需要更完善的等待机制)
    int max_wait = 10; // 最多等待 10 秒
    while (dhcp_supplied_address(netif) == 0 && max_wait > 0) {
        rt_thread_mdelay(1000); // 等待 1 秒
        max_wait--;
        LOG_I(".");
    }
    
    if (dhcp_supplied_address(netif)) {
        LOG_I("Success! IP address: %s", ip4addr_ntoa(&netif->ip_addr));
        return 1;
    } else {
        LOG_E("\nFailed to get IP address within timeout!");
        return 0;
    }
}


// wifi连接
int sw374_wifi_connect(char ** ssid, char ** pwd)
{
        rtw_security_t security = RTW_SECURITY_WPA2_AES_PSK; // 安全类型
        int ssid_len = strlen(*ssid);
        int pwd_len = strlen(*pwd);
        int key_id = 0; // WEP 密钥索引（非 WEP 时设为 0）
        void *semaphore = RT_NULL; // 信号量（命令模式下可置空）

        // 1. 开启 Wi-Fi 模块
        if (wifi_on(RTW_MODE_STA) < 0) {
            LOG_E("WiFi init failed!");
            return -1;
        }
        else
        {
            LOG_I("WiFi init successfully!");
        }

        // 2. 执行连接操作
        int ret = wifi_connect(
            *ssid, 
            security, 
            *pwd, 
            ssid_len, 
            pwd_len, 
            key_id, 
            semaphore
        );

        // 3. 检查连接结果
        if (ret == RTW_SUCCESS) {
            LOG_I("WiFi connected successfully!");
            
            get_ip_via_dhcp();
        } else {
            LOG_E("Connection failed! Code: %d", ret);
        }
        return ret;
}
