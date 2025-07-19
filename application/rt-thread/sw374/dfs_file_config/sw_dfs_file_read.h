#ifndef DFS_FILE_READ_H
#define DFS_FILE_READ_H

int dfs_read_from_file(char *buf, size_t buf_size, const char * file_dir);
int get_config_info(char ** ssid, char ** pwd, int * tcpserver_port, char ** version);


#endif // DFS_FILE_READ_H