#ifndef SW_WS2812_THREAD_H
#define SW_WS2812_THREAD_H

#include "stdio.h"
#include "rt_thread_class.h"
#include "sw_ws2812.h"

extern thread * SW_2812_thread; 

class SW_WS2812_thread : public thread {
private:
    
    // SW_WS2812 * p_SW_WS2812;
public:
    SW_WS2812_thread(std::string name, rt_uint32_t stack_size, rt_uint8_t priority, rt_uint32_t tick);
    ~SW_WS2812_thread();
    int rpm;
};



#endif