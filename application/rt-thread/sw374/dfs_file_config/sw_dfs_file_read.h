#ifndef DFS_FILE_READ_H
#define DFS_FILE_READ_H

// 配置ulog日志
#define LOG_TAG "dfs_file_read"
#include <ulog.h>

// 文件系统相关
#include <dfs_posix.h> 

// 轻量json解析库
#include <cJSON.h>

int dfs_read_from_file(char *buf, size_t buf_size, const char * file_dir);
int get_config_info(char ** ssid, char ** pwd, char ** version);


#endif // DFS_FILE_READ_H