#include <rtthread.h>

void my_thread_entry(void* parameter)
{
    int index = 0;

    while (1)
    {
        rt_kprintf("index => %d\n", index ++);
        rt_thread_delay(RT_TICK_PER_SECOND);
    }
}

int my_thread_init(void)
{
    rt_thread_t tid;

    tid = rt_thread_create("tMyTask", my_thread_entry, RT_NULL,
        2048, 20, 20);

    if (tid != RT_NULL)
        rt_thread_startup(tid);

    return 0;
}

