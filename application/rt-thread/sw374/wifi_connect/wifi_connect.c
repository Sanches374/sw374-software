#include <rtthread.h>

// 配置ulog日志
#define LOG_TAG "wifi_connect"
#include <ulog.h>

// 文件系统相关
#include <dfs_posix.h> 
// 轻量json解析库
#include <cJSON.h>

// Lwip
#include <lwip/netif.h>
#include <lwip/dhcp.h>
#include <lwip/init.h>

// wifi
#include <wifi/wifi_conf.h>

// 配置文件存放地址
const char file_dir[] = "/rodata/wificonfig.conf";

// 从文件系统中读取wifi配置文件内容
int dfs_read_from_file(char *buf, size_t buf_size, const char * file_dir);
int get_wifi_info(char ** ssid, char ** pwd, char ** version);
int get_ip_via_dhcp(void);
int sw374_wifi_connect(void);

// 从wifi_conf文件读取数据
int dfs_read_from_file(char *buf, size_t buf_size, const char * file_dir)
{
    int fd;
    ssize_t read_bytes;

    // 打开文件以只读模式
    fd = open(file_dir, O_RDONLY);
    if (fd < 0) {
        rt_kprintf("Failed to open /data/wifi_conf for reading, errno: %d\n", errno);
        return -1;
    }

    // 读取数据
    read_bytes = read(fd, buf, buf_size - 1);
    if (read_bytes < 0) {
        rt_kprintf("Failed to read from /data/wifi_conf, errno: %d\n", errno);
        close(fd);
        return -1;
    }

    // 确保字符串以空字符结尾
    buf[read_bytes] = '\0';

    // 关闭文件
    close(fd);
    return 0;
}

//获取wifi配置文件中的数据
int get_wifi_info(char ** ssid, char ** pwd, char ** version)
{
    
    // 文件读取
    char buffer[256];
    if (dfs_read_from_file(buffer, sizeof(buffer), file_dir) == 0) {
        // LOG_D("%s\n", buffer);
        LOG_I("Read file %s successfully!", file_dir);
    }
    // json解析
    cJSON *json = cJSON_Parse(buffer);
    if (json == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            LOG_E("JSON parse error before: %s\n", error_ptr);
        }
        LOG_E("Failed to parse JSON\n");
        return -1;
    }
    LOG_I("JSON parsed successfully!");

    
    if (cJSON_IsString(cJSON_GetObjectItem(json, "ssid")))
    {
        *ssid = cJSON_GetObjectItem(json, "ssid")->valuestring;
        LOG_I("wifi_ssid: %s", ssid);
    }
    else {
        LOG_E("Invalid WIFI SSID data\n");
    }

    if (cJSON_IsString(cJSON_GetObjectItem(json, "pwd")))
    {
        *pwd = cJSON_GetObjectItem(json, "pwd")->valuestring;
        LOG_I("wifi_pwd: %s", pwd);
    }
    else {
        LOG_E("Invalid WIFI password data\n");
    }

    *version = cJSON_GetObjectItem(json, "version")->valuestring;
    LOG_I("version: %s", version);

    return 0;
}


// 通过dhcp获取ip
int get_ip_via_dhcp(void)
{
    // 1. 查找网络接口 (WL0 是示例接口名)
    struct netif *netif = netif_find("WL0");
    
    if (netif == NULL) {
        LOG_E("Network interface not found!\n");
        return 0;
    }

    // 2. 确保接口已启用
    netif_set_up(netif);
    
    // 3. 启动 DHCP 客户端
    err_t err = dhcp_start(netif);
    
    if (err != ERR_OK) {
        LOG_E("Failed to start DHCP client: %d\n", err);
        return 0;
    }
    
    LOG_I("DHCP client started. Waiting for IP address...\n");
    
    // 4. 等待 DHCP 分配 IP (简单实现，实际应用中可能需要更完善的等待机制)
    int max_wait = 10; // 最多等待 10 秒
    while (dhcp_supplied_address(netif) == 0 && max_wait > 0) {
        rt_thread_mdelay(1000); // 等待 1 秒
        max_wait--;
        LOG_I(".");
    }
    
    if (dhcp_supplied_address(netif)) {
        LOG_I("\nSuccess! IP address: %s\n", ip4addr_ntoa(&netif->ip_addr));
        return 1;
    } else {
        LOG_E("\nFailed to get IP address within timeout!\n");
        return 0;
    }
}

// wifi连接
int sw374_wifi_connect(void)
{
    char * ssid = NULL;
    char * pwd = NULL;
    char * version = NULL;
    get_wifi_info(&ssid, &pwd, &version);
    // char *ssid = "Sanches";      // 替换为您的 SSID
    // char *pwd = "Sanches374"; // 替换为您的密码

        rtw_security_t security = RTW_SECURITY_WPA2_AES_PSK; // 安全类型
        int ssid_len = strlen(ssid);
        int pwd_len = strlen(pwd);
        int key_id = 0; // WEP 密钥索引（非 WEP 时设为 0）
        void *semaphore = RT_NULL; // 信号量（命令模式下可置空）

        // 1. 开启 Wi-Fi 模块
        if (wifi_on(RTW_MODE_STA) < 0) {
            LOG_E("WiFi init failed!\n");
            return -1;
        }
        else
        {
            LOG_I("WiFi init successfully!\n");
        }

        // 2. 执行连接操作
        int ret = wifi_connect(
            ssid, 
            security, 
            pwd, 
            ssid_len, 
            pwd_len, 
            key_id, 
            semaphore
        );

        // 3. 检查连接结果
        if (ret == RTW_SUCCESS) {
            LOG_I("WiFi connected successfully!\n");
            
            get_ip_via_dhcp();
        } else {
            LOG_E("Connection failed! Code: %d\n", ret);
        }
        return ret;
}



MSH_CMD_EXPORT(sw374_wifi_connect, wifi connect directly);