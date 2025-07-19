#include "rt_thread_class.h"
#include "SW_conf.h"
#include <string>  // 如果支持C++标准库
#define LOG_TAG "thread_cpp"
#include <ulog.h>
thread::thread(std::string name, rt_uint32_t stack_size, rt_uint8_t priority, rt_uint32_t tick) : 
thread_(RT_NULL), 
thread_name_(name),
stack_size_(stack_size),
priority_(priority),
tick_(tick)
{};

thread::~thread() {
    int ret = rt_thread_delete(this->thread_);
    if(ret != RT_EOK)
    {
        LOG_E("failed to thread %s !!",this->thread_name_.c_str());
        return;
    }
    LOG_I("thread %s exit successful !!",this->thread_name_.c_str());
}

void thread::thread_entry(void *param) {
    thread *self = static_cast<thread*>(param);
    self->run();
}

void thread::start() {
    this->thread_ = rt_thread_create(this->thread_name_.c_str(),
                                  &thread::thread_entry,
                                  this,
                                  this->stack_size_,
                                  this->priority_,
                                  this->tick_);
    if (this->thread_) {
        rt_thread_startup(this->thread_);
        LOG_I("Thread started.\n");
    } else {
        LOG_E("Failed to create Thread!\n");
    }
}

void thread::run() {
    LOG_I("Thread runing.\n");
}