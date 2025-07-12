#ifndef RT_THREAD_CLASS_H
#define RT_THREAD_CLASS_H

#include <rtthread.h>
#include "SW_conf.h"
#include <ulog.h>
#include <string>  // 如果支持C++标准库

class thread {
protected:
    rt_thread_t thread_;
    std::string thread_name_;  // 或 const char*
    rt_uint32_t stack_size_;
    rt_uint8_t priority_;
    rt_uint32_t tick_;

    virtual void run();  // 改为虚函数， 线程里运行的程序
public:
    thread(std::string name, rt_uint32_t stack_size, rt_uint8_t priority, rt_uint32_t tick);
    virtual ~thread();  // 虚析构函数
    
    // 禁止拷贝
    thread(const thread&) = delete;
    thread& operator=(const thread&) = delete;

    static void thread_entry(void *param);
    void start();
};

#endif