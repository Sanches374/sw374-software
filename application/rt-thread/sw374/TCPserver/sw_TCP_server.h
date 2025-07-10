#ifndef SW_TCP_SERVER_H
#define SW_TCP_SERVER_H

#include <rtthread.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <finsh.h>
#include <errno.h>
#include <cJSON.h>

void tcp_server_thread_entry(void *parameter);
// static void start_tcp_server(int argc, char **argv);
void start_tcp_server(void);

#endif // TCP_SERVER_H