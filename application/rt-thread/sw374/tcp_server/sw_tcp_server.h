#ifndef SW_TCP_SERVER_H
#define SW_TCP_SERVER_H
#ifdef __cplusplus  // 仅在C++编译时使用extern "C"
extern "C" {
#endif

void tcp_server_thread_entry(void *parameter);
void start_tcp_server(int);

#ifdef __cplusplus
}
#endif

#endif // TCP_SERVER_H