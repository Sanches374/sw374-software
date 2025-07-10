// 配置ulog
#define LOG_TAG              "TcpServer"
#include <ulog.h>

#include <rtthread.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <finsh.h>
#include <errno.h>
#include <cJSON.h>

#include "sw_TCP_server.h"

// 配置文件
#include "SW_conf.h"



static const char send_data[] = "This is TCP Server from RT-Thread.\n";

struct cJson_msg {
    cJSON *json;
};

// 声明全局消息队列 
// extern rt_mq_t cJson_msg_queue;




// 线程入口函数 
void tcp_server_thread_entry(void *parameter)
{
    int Port = *(int *)parameter;

    char *recv_data;
    socklen_t sin_size;
    int sock, connected, bytes_received;
    struct sockaddr_in server_addr, client_addr;
    rt_bool_t stop = RT_FALSE;
    int ret;

    recv_data = rt_malloc(TcpServer_BUFSZ + 1);
    if (recv_data == RT_NULL) {
        LOG_E("No memory");
        return;
    }

    /* 创建 TCP socket */
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        LOG_E("Socket error");
        rt_free(recv_data);
        return;
    }

    /* 配置服务器地址 */
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(Port);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    rt_memset(&(server_addr.sin_zero), 0, sizeof(server_addr.sin_zero));

    /* 绑定端口 */
    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
        LOG_E("Unable to bind");
        rt_free(recv_data);
        closesocket(sock);
        return;
    }

    /* 开始监听 */
    if (listen(sock, 5) == -1) {
        LOG_E("Listen error");
        rt_free(recv_data);
        closesocket(sock);
        return;
    }

    LOG_I("TCPServer Waiting for client on port %d ... ", Port);
    
    while (!stop) {
        sin_size = sizeof(struct sockaddr_in);
        connected = accept(sock, (struct sockaddr *)&client_addr, &sin_size);
        
        if (connected < 0) {
            LOG_W("accept failed: %d", errno);
            continue;
        }

        LOG_I("Connection from: %s:%d\n", 
                   inet_ntoa(client_addr.sin_addr), 
                   ntohs(client_addr.sin_port));

        while (!stop) {
            /* 发送欢迎消息 */
            ret = send(connected, send_data, strlen(send_data), 0);
            if (ret <= 0) {
                if (ret < 0) LOG_E("send error");
                break;
            }
            /* 接收数据 */
            bytes_received = recv(connected, recv_data, TcpServer_BUFSZ, 0);
            if (bytes_received <= 0) {
                if (bytes_received < 0) LOG_E("recv error\n");
                break;
            }

            recv_data[bytes_received] = '\0';
// #ifdef TcpServerDebug
            LOG_I("Received: %s\n", recv_data);
// #endif
            
            /* 命令处理 */
            if (strcmp(recv_data, "q") == 0 || strcmp(recv_data, "Q") == 0) {
                break;
            } else if (strcmp(recv_data, "exit") == 0) {
                stop = RT_TRUE;
                break;
            }

            /* JSON 数据处理 */
            cJSON *json = cJSON_Parse(recv_data);
            if (json) {
                LOG_I("JSON text: %s\n", cJSON_Print(json));
                struct cJson_msg msg;
                msg.json = json;

                // /* 发送到消息队列 - 失败时需自行释放 */
                // if (rt_mq_send(cJson_msg_queue, &msg, sizeof(msg))) {
                //     LOG_E("Failed to send to message queue");
                //     cJSON_Delete(json);  // 发送失败时释放
                // }
                // 发送成功则接收方负责释放
            }
        }
        closesocket(connected);
    }

    /* 清理资源 */
    closesocket(sock);

    // 5. 线程退出前释放参数内存（避免内存泄漏）
    rt_free(recv_data);
    LOG_I("TCP Server exit.");
}

/* 线程创建命令 */
// static void start_tcp_server(int argc, char **argv)
void start_tcp_server(int port)
{
    rt_thread_t tid = rt_thread_create(
        "tcp_serv",
        tcp_server_thread_entry,  // 线程入口函数
        &port,                    // 传递端口号的地址
        TcpServer_THREAD_STACK_SIZE,
        TcpServer_THREAD_PRIORITY,
        TcpServer_THREAD_TIMESLICE
    );

    if (tid != RT_NULL) {
        rt_thread_startup(tid);
        LOG_I("TCP server thread started with port: %d", port);
    } else {
        LOG_E("Failed to create thread");
    }
}

MSH_CMD_EXPORT(start_tcp_server, start TCP server thread);
