#ifdef __cplusplus
extern "C" {
#endif
#include "bt_config.h"
#include "bt_api.h"
#include "bt_os.h"
#include "bt_core.h"

#if LINUX


static __krnl_stmr_t   			*bt_receivecmd_timer;
static __krnl_stmr_t   			*bt_decodecmd_timer;

static pthread_t   	uart_recive_task_id;
static pthread_t   	uart_decode_task_id;


#if (runmode == 1)//线程运行
#if 0
__s32 bt_task_start(void)//task
{
        int     err;
        
		printf("uart_key_start\n");
		err = pthread_create(&uart_decode_task_id, NULL, sys_bt_decode_cmd, (void*)NULL);
        if (err) 
        {
            printf("decode_task pthread_create error!");
            return EPDK_FAIL;
        }
        printf("create uart_decode_task_id:0x%x", uart_decode_task_id);
       
		err = pthread_create(&uart_recive_task_id, NULL, sys_bt_receive_cmd, (void*)NULL);
        if (err) 
        {
            printf("decode_task pthread_create error!");
            return EPDK_FAIL;
        }
        printf("create uart_recive_task_id:0x%x", uart_recive_task_id);
		uart_recive_task_id = esKRNL_TCreate(sys_bt_receive_cmd, (void*)NULL, 0x800, KRNL_priolevel1);

		printf("uart_key_start");
	    return EPDK_OK;	
}

__s32 bt_task_stop(void)
{
	if(uart_recive_task_id != 0)
    {
        do  
        {
            esKRNL_TimeDlyResume(uart_recive_task_id);
            esKRNL_TimeDly(1);
        } while (OS_TASK_NOT_EXIST != esKRNL_TDelReq(uart_recive_task_id));
        uart_recive_task_id = 0;
    }
    if(uart_decode_task_id != 0)
    {
        do  
        {
            esKRNL_TimeDlyResume(uart_decode_task_id);
            esKRNL_TimeDly(1);
        } while (OS_TASK_NOT_EXIST != esKRNL_TDelReq(uart_decode_task_id));
        uart_decode_task_id = 0;
    }
	return EPDK_OK;
}
#else
__s32 bt_task_start(void)//task
{

		printf("uart_key_start\n");
		uart_decode_task_id = esKRNL_TCreate(sys_bt_decode_cmd, (void*)NULL, 0x800, KRNL_priolevel2);
		if(uart_decode_task_id == NULL)
		{
			printf("create uart_decode_task_id failed\n");
			return EPDK_FAIL;
		}
		
		
		uart_recive_task_id = esKRNL_TCreate(sys_bt_receive_cmd, (void*)NULL, 0x800, KRNL_priolevel1);
		if(uart_recive_task_id == NULL)
		{
			printf("create uart_recive_task_id failed\n");
			return EPDK_FAIL;
		}
		printf("uart_key_start");
	    return EPDK_OK;	
}

__s32 bt_task_stop(void)
{
	if(uart_recive_task_id != 0)
    {
        do  
        {
            esKRNL_TimeDlyResume(uart_recive_task_id);
            esKRNL_TimeDly(1);
        } while (OS_TASK_NOT_EXIST != esKRNL_TDelReq(uart_recive_task_id));
        uart_recive_task_id = 0;
    }
    if(uart_decode_task_id != 0)
    {
        do  
        {
            esKRNL_TimeDlyResume(uart_decode_task_id);
            esKRNL_TimeDly(1);
        } while (OS_TASK_NOT_EXIST != esKRNL_TDelReq(uart_decode_task_id));
        uart_decode_task_id = 0;
    }
	return EPDK_OK;
}
#endif
#elif(runmode == 0)//定时器运行
__s32 bt_task_start(void) timer
{

		printf("esTIME_StartTimer bt_decodecmd_timer\n");
		bt_decodecmd_timer = esKRNL_TmrCreate(20,OS_TMR_OPT_PRIO_HIGH | OS_TMR_OPT_PERIODIC,sys_bt_decode_cmd, NULL);
		if(bt_decodecmd_timer == NULL)
		{
			printf("create bt_decodecmd_timer failed\n");

			return EPDK_FAIL;
		}
		esKRNL_TmrStart(bt_decodecmd_timer);
			printf("esTIME_StartTimer bt_receivecmd_timer\n");
		bt_receivecmd_timer = esKRNL_TmrCreate(80,OS_TMR_OPT_PRIO_LOW | OS_TMR_OPT_PERIODIC,sys_bt_receive_cmd, NULL);
		if(bt_receivecmd_timer == NULL)
		{
			printf("create bt_receivecmd_timer failed\n");

			return EPDK_FAIL;
		}
		esKRNL_TmrStart(bt_receivecmd_timer);
	
	    printf("esKRNL_TmrStart end\n");
	    return EPDK_OK;
start_exit:
    if(bt_receivecmd_timer!=RT_NULL)
	{
		esKRNL_TmrStop(bt_receivecmd_timer,OS_TMR_OPT_PRIO_LOW | OS_TMR_OPT_PERIODIC,NULL);
		esKRNL_TmrDel(bt_receivecmd_timer);
		bt_receivecmd_timer = RT_NULL;
	}
	if(bt_decodecmd_timer!=RT_NULL)
	{
		esKRNL_TmrStop(bt_decodecmd_timer,OS_TMR_OPT_PRIO_LOW | OS_TMR_OPT_PERIODIC,NULL);
		esKRNL_TmrDel(bt_decodecmd_timer);
		bt_decodecmd_timer = RT_NULL;
	}
	return EPDK_FAIL;
}

__s32 bt_task_stop(void)
{
	if(bt_receivecmd_timer!=RT_NULL)
	{
		esKRNL_TmrStop(bt_receivecmd_timer,OS_TMR_OPT_PRIO_LOW | OS_TMR_OPT_PERIODIC,NULL);
		esKRNL_TmrDel(bt_receivecmd_timer);
		bt_receivecmd_timer = RT_NULL;
	}
	if(bt_decodecmd_timer!=RT_NULL)
	{
		esKRNL_TmrStop(bt_decodecmd_timer,OS_TMR_OPT_PRIO_LOW | OS_TMR_OPT_PERIODIC,NULL);
		esKRNL_TmrDel(bt_decodecmd_timer);
		bt_decodecmd_timer = RT_NULL;
	}
	return EPDK_OK;
}
#endif
#endif
#ifdef __cplusplus
}
#endif

