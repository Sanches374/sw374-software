#ifndef WIFI_CONNECT_H
#define WIFI_CONNECT_H




// 从文件系统中读取wifi配置文件内容
// int dfs_read_from_file(char *buf, size_t buf_size, const char * file_dir);
// int get_wifi_info(char ** ssid, char ** pwd, char ** version);
int get_ip_via_dhcp(void);
int sw374_wifi_connect(char *, char *);

#endif