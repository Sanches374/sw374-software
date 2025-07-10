#include "sw_dfs_file_read.h"

const char file_dir[] = "/rodata/config.conf";

// 从wifi_conf文件读取数据
int dfs_read_from_file(char *buf, size_t buf_size, const char * file_dir)
{
    int fd;
    ssize_t read_bytes;

    // 打开文件以只读模式
    fd = open(file_dir, O_RDONLY);
    if (fd < 0) {
        LOG_E("Failed to open /data/wifi_conf for reading, errno: %d", errno);
        return -1;
    }

    // 读取数据
    read_bytes = read(fd, buf, buf_size - 1);
    if (read_bytes < 0) {
        LOG_E("Failed to read from /data/wifi_conf, errno: %d", errno);
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
int get_config_info(char ** ssid, char ** pwd, char ** version)
{
    
    // 文件读取
    char buffer[512];
    if (dfs_read_from_file(buffer, sizeof(buffer), file_dir) == 0) {
        // LOG_D("%s\n", buffer);
        LOG_I("Read file %s successfully!", file_dir);
    }
    // json解析
    cJSON *config_json = cJSON_Parse(buffer);
    if (config_json == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            LOG_E("JSON parse error before: %s", error_ptr);
        }
        LOG_E("Failed to parse JSON");
        return -1;
    }
    LOG_I("JSON parsed successfully!");

    cJSON * wifi_json = cJSON_GetObjectItem(config_json, "wifi");
    if (cJSON_IsString(cJSON_GetObjectItem(wifi_json, "ssid")))
    {
        *ssid = cJSON_GetObjectItem(wifi_json, "ssid")->valuestring;
        LOG_I("wifi_ssid: %s", *ssid);
    }
    else {
        LOG_E("Invalid WIFI SSID data");
    }

    if (cJSON_IsString(cJSON_GetObjectItem(wifi_json, "pwd")))
    {
        *pwd = cJSON_GetObjectItem(wifi_json, "pwd")->valuestring;
        LOG_I("wifi_pwd: %s", *pwd);
    }
    else {
        LOG_E("Invalid WIFI password data");
    }

    *version = cJSON_GetObjectItem(config_json, "version")->valuestring;
    LOG_I("version: %s", *version);

    //释放json, 无需再额外添加cJSON_Delete(wifi_json)
    cJSON_Delete(config_json);
    
    return 0;
}
