
#include <rtthread.h>
#include <ulog.h>

#include "SW_conf.h"
#include "sw_ws2812_thread.h"


thread * SW_2812_thread = nullptr; 
extern SW_WS2812 * p_SW_WS2812;

// static int test_cpp_ws2812_module(void)
// {
//     SW_2812_thread = new SW_WS2812_thread("WS2812", WS2812_THREAD_STACK_SIZE, WS2812_THREAD_PRIORITY, WS2812_THREAD_TIMESLICE); // 动态创建
//     SW_2812_thread->start();
//     return 0;
// }
// 导出函数为命令
// MSH_CMD_EXPORT(test_cpp_ws2812_module, ws2812 module test);

// INIT_APP_EXPORT(test_cpp_ws2812_module);



SW_WS2812_thread::SW_WS2812_thread(std::string name, rt_uint32_t stack_size, rt_uint8_t priority, rt_uint32_t tick) : thread(name, stack_size, priority, tick)
{
    p_SW_WS2812 = new SW_WS2812;
    p_SW_WS2812->run();
}

SW_WS2812_thread::~SW_WS2812_thread()
{
    delete p_SW_WS2812;
}
