#include "74hc165.h"

#define THREAD_PRIORITY         20      // 线程优先级
#define THREAD_STACK_SIZE       4096    // 线程大小
#define THREAD_TIMESLICE        15      // 时间片
static rt_thread_t _74hc165_thread = RT_NULL;   // 线程控制块

uint8_t data[MAX_74HC165_CHIPS] = {0};       // 存储每个芯片的读取数据
uint8_t lastdata[MAX_74HC165_CHIPS] = {0};   // 存储每个芯片的上一次读取数据

void setup()
{
    rt_pin_mode(_74HC165_CLK_GPIO_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(_74HC165_SHLD_GPIO_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(_74HC165_SO_GPIO_PIN, PIN_MODE_INPUT);
    rt_pin_write(_74HC165_CLK_GPIO_PIN, PIN_HIGH);  // 时钟引脚初始化为高电平
    rt_pin_write(_74HC165_SHLD_GPIO_PIN, PIN_HIGH); // 数据加载引脚初始化为高电平
}

// 读取单个 74HC165 芯片的数据
uint8_t read_74hc165()
{
    uint8_t chip_data = 0;
    for (uint8_t i = 0; i < 8; i++)
    {
        chip_data <<= 1;
        rt_pin_write(_74HC165_CLK_GPIO_PIN, PIN_LOW); 
        rt_hw_us_delay(10);

        if (rt_pin_read(_74HC165_SO_GPIO_PIN) == PIN_HIGH) // 读数数据输出引脚的电平，如果为高电平则置1
            chip_data = chip_data | 0x01;
        rt_pin_write(_74HC165_CLK_GPIO_PIN, PIN_HIGH); 
    }
    return chip_data;
}

void loop()
{

    rt_pin_write(_74HC165_SHLD_GPIO_PIN, PIN_LOW); // 数据加载引脚初始化为低电平
    rt_hw_us_delay(10);
    rt_pin_write(_74HC165_SHLD_GPIO_PIN, PIN_HIGH); // 数据加载引脚初始化为高电平
    for (int i = 0; i < MAX_74HC165_CHIPS; i++)
    {
        data[i] = read_74hc165();

        if (lastdata[i] != data[i]) 
        {
            rt_kprintf("Chip %d: %02x, %2d\n", i, data[i], data[i]);
            lastdata[i] = data[i];
        }
    }
}

// 线程入口函数
static void _74hc165_thread_entry(void *param)
{
    setup();
    while (1)
    {
        loop();
    }
}

static void test_74165(int argc, char **argv)
{
    /* 创建线程，名称是 74hc165_thread，入口是 74hc165_thread_entry */
    _74hc165_thread = rt_thread_create("74hc165_thread",
                            _74hc165_thread_entry, RT_NULL,
                            THREAD_STACK_SIZE,
                            THREAD_PRIORITY, THREAD_TIMESLICE);

    /* 如果获得线程控制块，启动这个线程 */
    if (_74hc165_thread != RT_NULL)
        rt_thread_startup(_74hc165_thread);

    rt_kprintf("test_74165 run END!!\n");
}
MSH_CMD_EXPORT(test_74165, 74HC165 Parallel-In Serial-Out Shift Register Module);