#include <stdio.h>

#define RT_API_TEST
#ifdef RT_API_TEST
#include <rtthread.h>
#include <dlmodule.h>
extern int my_thread_init(void);
#endif

int main(int argc, char *argv[])
{
    printf("[AIC-DM-APP] Hello, world!\n");

#ifdef RT_API_TEST
    my_thread_init();
    return RT_DLMODULE_DEAMON;
#endif

    return 0;
}

void module_init(struct rt_dlmodule *module)
{
    printf("[AIC-DM-APP] init!\n");
}

void module_cleanup(struct rt_dlmodule *module)
{
    printf("[AIC-DM-APP] exit!\n");
}
