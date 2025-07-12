#ifndef SW_WS2812_H
#define SW_WS2812_H

#include "rt_thread_class.h"
#include "stdio.h"


class SW_WS2812 {
private:
    int rpm;
    int redlinerpm;
    int rpm_when_flashing;
    int rpm_when_truning_on;

    // 定时器周期刷新 WS2812
    rt_timer_t tmr = RT_NULL;
    static void timer_callback(void *parameter);
    int left_right_flashing_counter;        // 用于两侧LED闪烁计数
    int left_right_flashing_times;          // 存储两侧LED闪烁次数
    int left_right_flashing_delay_times;    // 存储两侧LED两次闪烁之间间隔的 WS2812_TIMER_PERIOD 个数
    uint64_t left_right_flashing_color;     // 存储两侧LED闪烁的颜色

    bool top_flashing_bool;         // 是否开启闪烁
    int top_flashing_counter;       // 用于顶部LED闪烁计数
    int top_flashing_delay_times;   // 存储顶部LED两次闪烁之间间隔的 WS2812_TIMER_PERIOD 个数
    uint64_t top_flashing_color;    // 存储顶部LED闪烁的颜色

    struct rt_qspi_device * ws2812_device = RT_NULL;
    uint8_t   LedsArray[WS2812_NUMBERS * 3];
    uint32_t  ledsCount   = WS2812_NUMBERS;
    uint32_t  nbLedsBytes = WS2812_NUMBERS*3;

    uint64_t Color_buff[WS2812_NUMBERS];
    
    // WS2812 移植
    int WS2812_Init(void);
    int WS2812_RESET(void);
    int WS2812_Set_Color(uint8_t LED_NUM, uint64_t Color);  // 设置彩灯颜色
    int WS2812_Send_Array(void);    // 发送彩灯数据
    int WS2812_Qspi_Attach(void);
    int WS2812_Qspi_Init(void);
    int WS2812_WriteData(uint8_t *send_buff, uint32_t len);

    // 闪烁两侧的LED灯
    void left_right_flashing();
    // 闪烁顶部的LED灯
    void top_flashing(void);
    // rpm 转换为 Colorbuff
    void rpm_led_no_drs(void);
public: 
    SW_WS2812();
    ~SW_WS2812();
    void run();
    void set_top_flashing_bool(bool);
    void set_rpm(int);
};

#endif