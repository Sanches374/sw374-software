#include <stdlib.h>
#include <rtthread.h>
#include <sys/time.h>
#include "rtdevice.h"
#include <stdio.h>



#include "SW_conf.h"
#include "sw_ws2812.h"

#define LOG_TAG "WS2812B"
#include <ulog.h>

SW_WS2812::SW_WS2812() : 
redlinerpm(6650), rpm_when_flashing(6790), rpm_when_truning_on(5250),
left_right_flashing_counter(0), left_right_flashing_times(0), left_right_flashing_delay_times(10), left_right_flashing_color(WS2812_WHITE),
top_flashing_bool(false), top_flashing_counter(0), top_flashing_delay_times(3), top_flashing_color(WS2812_RED)
{
    if (WS2812_left_led_numbers + WS2812_left_led_numbers + WS2812_top_led_numbers == WS2812_NUMBERS)
    {
        for (size_t i = 0; i < WS2812_NUMBERS; i++)
        {
            Color_buff[i] = 0;
        }
        // WS2812 初始化
        this->WS2812_Init();
        // 定时器初始化
        this->tmr =  rt_timer_create(WS2812_TIMER_NAME,                 // 软件定时器名称       
						&SW_WS2812::timer_callback,            // 软件定时器超时函数
						this,                    // 超时函数参数
						WS2812_TIMER_PERIOD,                       // 超时时间
						RT_TIMER_FLAG_PERIODIC |   
						RT_TIMER_FLAG_SOFT_TIMER);  // 软件定时器模式，周期模式
        if(tmr != RT_NULL)
		    rt_timer_start(this->tmr);
    }
    else
        LOG_E("WS2812_left_led + WS2812_right_led + WS2812_top_led != WS2812_NUMBERS");
}

SW_WS2812::~SW_WS2812()
{
    if (this->tmr) {
        rt_timer_stop(this->tmr);
        rt_timer_delete(this->tmr);
        this->tmr = RT_NULL;
    }
}


void SW_WS2812::timer_callback(void *param)
{
    SW_WS2812 *self = static_cast<SW_WS2812*>(param);

    // 只能在 timer_callback 中调用(不需要判断直接调用即可，通过修改私有成员更改闪烁)
    self->rpm_led_no_drs();

    self->top_flashing();
    self->left_right_flashing();
    

    // 根据 Color_buff 点亮LED
    for (uint8_t i = 0; i < WS2812_NUMBERS; i++)
    {
        self->WS2812_Set_Color(i, self->Color_buff[i]);
    }
    self->WS2812_Send_Array();
}

void SW_WS2812::set_top_flashing_bool(bool flag)
{
    this->top_flashing_bool = flag;
}



void SW_WS2812::set_rpm(int rpm)
{
    this->rpm = rpm;
}





void SW_WS2812::run()
{
    // while (1)
    // {
    //     LOG_I("waiting");
    //     rt_thread_mdelay(5000);
    //     set_top_flashing_times(3);
    //     this->top_flashing_counter = 0;
    // }
}


/**********************************************************
 * 函 数 名 称：left_right_flashing
 * 函 数 功 能：左右两侧LED灯闪烁
 * 传 入 参 数：无
 * 函 数 返 回：无
 * 作       者：sanches
 * 备       注：
**********************************************************/

void SW_WS2812::left_right_flashing()
{
    if (this->left_right_flashing_counter < this->left_right_flashing_delay_times * 2 * this->left_right_flashing_times)
    {
        this->left_right_flashing_counter ++;
        if (this->left_right_flashing_counter % (2 * this->left_right_flashing_delay_times) == this->left_right_flashing_delay_times)
        {
            for (int j = WS2812_left_led_numbers; j < WS2812_left_led_numbers + WS2812_top_led_numbers; j++)
                this->Color_buff[j] = this->left_right_flashing_color;
            
        }
        if (this->left_right_flashing_counter % (2 * this->left_right_flashing_delay_times) == 0)
        {
            for (int j = WS2812_left_led_numbers; j < WS2812_left_led_numbers + WS2812_top_led_numbers; j++)
                this->Color_buff[j] = WS2812_BLACK;
        }
        LOG_D("%d",this->left_right_flashing_counter);
    }
}

/**********************************************************
 * 函 数 名 称：top_flashing
 * 函 数 功 能：顶部LED灯闪烁
 * 传 入 参 数：无
 * 函 数 返 回：无
 * 作       者：sanches
 * 备       注：
**********************************************************/
void SW_WS2812::top_flashing(void)
{
    
    if (this->top_flashing_bool)
    {
        this->top_flashing_counter ++;
        if (this->top_flashing_counter % (2 * this->top_flashing_delay_times) == this->top_flashing_delay_times)
        {
            for (int j = 0; j < WS2812_top_led_numbers; j++)
                this->Color_buff[WS2812_left_led_numbers + j] = RPM_FLASHING_COLOR[j];
            
        }
        if (this->top_flashing_counter % (2 * this->top_flashing_delay_times) == 0)
        {
            for (int j = 0; j < WS2812_top_led_numbers; j++)
                this->Color_buff[WS2812_left_led_numbers + j] = WS2812_BLACK;
            this->top_flashing_counter = 0;
        }
        LOG_D("top_flashing_counter: %d",this->top_flashing_counter);
    }    
    else
    {
        if (this->top_flashing_counter != 0)
        {
            this->top_flashing_counter = 0;
            for (int j = WS2812_left_led_numbers; j < WS2812_left_led_numbers + WS2812_top_led_numbers; j++)
                this->Color_buff[j] = WS2812_BLACK;
        }
    }

}


/**********************************************************
 * 函 数 名 称：rpm_led_no_drs
 * 函 数 功 能：rpm 转换为 LED 显示
 * 传 入 参 数：无
 * 函 数 返 回：无
 * 作       者：sanches
 * 备       注：
**********************************************************/
void SW_WS2812::rpm_led_no_drs(void)
{
    const uint64_t * buff_case = PRM_FLASHING_CASE_0;
    if (this->rpm > this->rpm_when_flashing)
    {
        set_top_flashing_bool(true);
    }
    // else if (this->rpm < this->rpm_when_truning_on)
    // {
    //     set_top_flashing_bool(false);
    //     for (int i = 0; i < WS2812_top_led_numbers; i++)
    //         this->Color_buff[WS2812_left_led_numbers + i] = WS2812_BLACK;
    // }
    else
    {
        set_top_flashing_bool(false);
        double steprpm = static_cast<double> (this->redlinerpm - this->rpm_when_truning_on) / WS2812_top_led_numbers;
        LOG_D("steprpm = %f",steprpm);
        int deltarpm = this->rpm - this->rpm_when_truning_on;
        LOG_D("deltarpm = %d, case = %d",deltarpm, int(deltarpm / steprpm));
        switch(int(deltarpm / steprpm))
        {
            case 0: buff_case = PRM_FLASHING_CASE_0; break;
            case 1: buff_case = PRM_FLASHING_CASE_1; break;
            case 2: buff_case = PRM_FLASHING_CASE_2; break;
            case 3: buff_case = PRM_FLASHING_CASE_3; break;
            case 4: buff_case = PRM_FLASHING_CASE_4; break;
            case 5: buff_case = PRM_FLASHING_CASE_5; break;
            case 6: buff_case = PRM_FLASHING_CASE_6; break;
            case 7: buff_case = PRM_FLASHING_CASE_7; break;
            case 8: buff_case = PRM_FLASHING_CASE_8; break;
            case 9: buff_case = PRM_FLASHING_CASE_9; break;
            case 10: buff_case = PRM_FLASHING_CASE_10; break;
            case 11: buff_case = PRM_FLASHING_CASE_11; break;
            case 12: buff_case = PRM_FLASHING_CASE_12; break;
            case 13: buff_case = PRM_FLASHING_CASE_13; break;
            case 14: buff_case = PRM_FLASHING_CASE_14; break;
            case 15: buff_case = PRM_FLASHING_CASE_15; break;
            default : buff_case = PRM_FLASHING_CASE_0; break;
        }
        for (int i = 0; i < WS2812_top_led_numbers; i++)
            this->Color_buff[WS2812_left_led_numbers + i] = buff_case[i];
    }
}











































/**********************************************************
 * 函 数 名 称：WS2812_Qspi_Attach
 * 函 数 功 能：挂载qspi的设备
 * 传 入 参 数：无
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：LP
**********************************************************/
// static int WS2812_Qspi_Attach(void)
int SW_WS2812::WS2812_Qspi_Attach(void)
{
    static struct rt_qspi_device *qspi_device = RT_NULL; // qspi设备结构体

    // 为spi结构体申请一片空间
    qspi_device = static_cast<struct rt_qspi_device*>(rt_malloc(sizeof(struct rt_qspi_device)));

    if(RT_NULL == qspi_device)
    {
        LOG_E("Failed to malloc the qspi device.");
        return -RT_ENOMEM;
    }    
    LOG_I("malloc the qspi succeed.\n");

    qspi_device->enter_qspi_mode = RT_NULL;
    qspi_device->exit_qspi_mode = RT_NULL;
    qspi_device->config.qspi_dl_width = 1;

    int ret = rt_spi_bus_attach_device(&qspi_device->parent, WS2812_DEVICE_NAME, WS2812_SQPSI_BUS_NANE, RT_NULL);
    if(ret != RT_EOK)
    {
        LOG_E("Failed to rt_spi_bus_attach_device.");
        return ret;
    }
    LOG_I("rt_spi_bus_attach_device succeed.\n");

    return RT_EOK;
}


/**********************************************************
 * 函 数 名 称：WS2812_Qspi_Init
 * 函 数 功 能：配置qspi进行初始化
 * 传 入 参 数：无
 * 函 数 返 回：无
 * 作       者：LCKFB
 * 备       注：LP
**********************************************************/
// static int WS2812_Qspi_Init(void)
int SW_WS2812::WS2812_Qspi_Init(void)
{
    // struct rt_qspi_device *g_qspi;
    struct rt_qspi_configuration qspi_cfg;
    struct rt_device *dev;
    std::string name;
    int ret = 0;

    name = WS2812_DEVICE_NAME;
    // ws2812_device = static_cast<struct rt_qspi_device*>rt_device_find(name);
    ws2812_device = (struct rt_qspi_device *)rt_device_find(name.c_str());
    if (!ws2812_device) 
    {
        LOG_E("Failed to get device in name %s\n", name.c_str());
        return -RT_ERROR;
    }
    LOG_I("find Qspi device :%s\n",name.c_str());

    dev = (struct rt_device *)ws2812_device;
    if (dev->type != RT_Device_Class_SPIDevice)
    {
        ws2812_device = RT_NULL;

        LOG_E("%s is not SPI device.\n", name);
        return -RT_ERROR;
    }

    rt_memset(&qspi_cfg, 0, sizeof(qspi_cfg));

    qspi_cfg.qspi_dl_width = 1;     // QSPI 总线位宽，单线模式 1 位、双线模式 2 位，4 线模式 4 位
    qspi_cfg.parent.mode = RT_SPI_MASTER | RT_SPI_MODE_3 | RT_SPI_MSB;
    qspi_cfg.parent.data_width = 8;
    qspi_cfg.parent.max_hz = 8 * 1000 * 1000;   // 8M
    ret = rt_qspi_configure(ws2812_device, &qspi_cfg);
    if (ret < 0) 
    {
        LOG_E("qspi configure failure.");
        return -RT_ERROR;;
    }
    LOG_I("rt_qspi_configure successful\n");

    return RT_EOK;
}

/******************************************************************
 * 函 数 名 称：WS2812_WriteData
 * 函 数 说 明：向WS2812写入len长度的字节
 * 函 数 形 参：send_buff数据地址   len字节长度
 * 函 数 返 回：RT_EOK成功  -RT_ERROR失败
 * 作       者：LCKFB
 * 备       注：1码的时序 = 高电平580ns~1us    再低电平220ns~420ns
 *              0码的时序 = 高电平220ns~380ns  再低电平580ns~1us
******************************************************************/
// static int WS2812_WriteData(uint8_t *send_buff, uint32_t len)
int SW_WS2812::WS2812_WriteData(uint8_t *send_buff, uint32_t len)
{
    // int i, j;
    uint32_t i, j;
    // int ret = 0;

    rt_size_t ret = 0;
    uint8_t buff[512] = {0};  // 发送缓存区
    struct rt_qspi_message msg;

    rt_memset(&msg, 0, sizeof(msg));

    msg.instruction.content = 0;		/* 指令内容 */
    msg.instruction.qspi_lines = 0;		/* 指令模式，单线模式 1 位、双线模式 2 位，4 线模式 4 位 */

    msg.alternate_bytes.content = 0;	/* 地址/交替字节 内容 */
    msg.alternate_bytes.size = 0;		/* 地址/交替字节 长度 */
    msg.alternate_bytes.qspi_lines = 0; /* 地址/交替字节 模式，单线模式 1 位、双线模式 2 位，4 线模式 4 位 */

    msg.dummy_cycles = 0;		/* 空指令周期阶段 */
    msg.qspi_data_lines = 1;	/*  QSPI 总线位宽 */

    /* 这里加了一个字节的数据发送，是因为发送开头不准，先发测试的数，之后再发数据！ */
    msg.parent.length = len * 8 + 1;		/* 发送 / 接收 数据字节数 */
    msg.parent.next = RT_NULL;			    /* 指向继续发送的下一条消息的指针 */
    msg.parent.cs_take = 1;				    /* 片选选中 */
    msg.parent.cs_release = 1;			    /* 释放片选 */

    msg.parent.send_buf = buff;		        /* 发送缓冲区指针 */
    msg.parent.recv_buf = RT_NULL; 			/* 接收缓冲区指针 */

    /* 测试数据 */
    buff[0] = WS2812_BLACK;

    // 转换send_buff中的每个bit并存入buff
    for (i = 0, j = 1; i < len; i++)
    {
        for (int bit = 0; bit < 8; bit++)
        {
            if (send_buff[i] & (0x80 >> bit)) // 当前位为1
            {
                buff[j++] = WS2812_1_CODE; // 存储1码
            }
            else // 当前位为0
            {
                buff[j++] = WS2812_0_CODE; // 存储0码
            }
        }
    }

    rt_spi_take_bus((struct rt_spi_device *)ws2812_device);

    ret = rt_qspi_transfer_message(ws2812_device, &msg);
    if (ret != msg.parent.length)
    {
        LOG_E("rt_qspi_transfer_message failed!!");
        return -RT_ERROR;
    }

    rt_spi_release_bus((struct rt_spi_device *)ws2812_device);

    return RT_EOK;
}


/******************************************************************
 * 函 数 名 称：WS2812_RESET
 * 函 数 说 明：复位ws2812
 * 函 数 形 参：无
 * 函 数 返 回：RT_EOK成功  -RT_ERROR失败
 * 作       者：LCKFB
 * 备       注：低电平280us以上
******************************************************************/
int SW_WS2812::WS2812_RESET(void)
{
    // int ret = 0;
    rt_size_t ret = 0;
    uint8_t buff[300] = {0x00};  // 发送缓存区
    struct rt_qspi_message msg;

    rt_memset(&msg, 0, sizeof(msg));

    msg.instruction.content = 0;		/* 指令内容 */
    msg.instruction.qspi_lines = 0;		/* 指令模式，单线模式 1 位、双线模式 2 位，4 线模式 4 位 */

    msg.alternate_bytes.content = 0;	/* 地址/交替字节 内容 */
    msg.alternate_bytes.size = 0;		/* 地址/交替字节 长度 */
    msg.alternate_bytes.qspi_lines = 0; /* 地址/交替字节 模式，单线模式 1 位、双线模式 2 位，4 线模式 4 位 */

    msg.dummy_cycles = 0;		/* 空指令周期阶段 */
    msg.qspi_data_lines = 1;	/*  QSPI 总线位宽 */

    msg.parent.length = 300;			    /* 发送 / 接收 数据字节数 */
    msg.parent.next = RT_NULL;			    /* 指向继续发送的下一条消息的指针 */
    msg.parent.cs_take = 1;				    /* 片选选中 */
    msg.parent.cs_release = 1;			    /* 释放片选 */

    msg.parent.send_buf = buff;		        /* 发送缓冲区指针 */
    msg.parent.recv_buf = RT_NULL; 			/* 接收缓冲区指针 */

    rt_spi_take_bus((struct rt_spi_device *)ws2812_device);

    ret = rt_qspi_transfer_message(ws2812_device, &msg);
    if (ret != msg.parent.length)
    {
        LOG_E("rt_qspi_transfer_message failed!!");
        return -RT_ERROR;
    }

    rt_spi_release_bus((struct rt_spi_device *)ws2812_device);

    return RT_EOK;
}


/******************************************************************
 * 函 数 名 称：WS2812_Set_Color
 * 函 数 说 明：设置彩灯颜色
 * 函 数 形 参：LED_NUM控制的第几个灯  Color颜色数据
 * 函 数 返 回：RT_EOK成功  -RT_ERROR失败
 * 作       者：LCKFB
 * 备       注：
******************************************************************/
int SW_WS2812::WS2812_Set_Color(uint8_t LED_NUM, uint64_t Color)
{
    if( LED_NUM >= ledsCount )
    {
        return -RT_ERROR;    //to avoid overflow
    }

    LedsArray[LED_NUM * 3]     = 0;
    LedsArray[LED_NUM * 3 + 1] = 0;
    LedsArray[LED_NUM * 3 + 2] = 0;

    LedsArray[LED_NUM * 3]     = (Color>>8)  & 0xff;
    LedsArray[LED_NUM * 3 + 1] = (Color>>16) & 0xff;
    LedsArray[LED_NUM * 3 + 2] = (Color>>0)  & 0xff;

    // LOG_I("WS2812_Set_Color successful !\n");

    return RT_EOK;
}

/******************************************************************
 * 函 数 名 称：WS2812_Send_Array
 * 函 数 说 明：发送彩灯数据
 * 函 数 形 参：无
 * 函 数 返 回：RT_EOK成功  -RT_ERROR失败
 * 作       者：LCKFB
 * 备       注：
******************************************************************/
int SW_WS2812::WS2812_Send_Array(void)
{
    this->WS2812_RESET();

    // aicos_mdelay(5);
    rt_thread_mdelay(5);

    int ret = this->WS2812_WriteData(LedsArray, nbLedsBytes);
    if(ret != RT_EOK)
    {
        LOG_E("%s--->WS2812_WriteByte failed !!",__FUNCTION__);
        return -RT_ERROR;
    }

    return RT_EOK;
}

/**********************************************************
 * 函 数 名 称：WS2812_Init
 * 函 数 功 能：初始化模块
 * 传 入 参 数：无
 * 函 数 返 回：RT_EOK成功  -RT_ERROR失败
 * 作       者：LCKFB
 * 备       注：
**********************************************************/
int SW_WS2812::WS2812_Init(void)
{
    /* 设备挂载 */
    if(RT_EOK != this->WS2812_Qspi_Attach())
    {
        LOG_E("%s-->WS2812_Qspi_Attach failed !!!",__FUNCTION__);
        return -RT_ERROR;
    }

    /* 设备初始化 */
    if(RT_EOK != this->WS2812_Qspi_Init())
    {
        LOG_E("%s-->WS2812_Qspi_Init failed !!!",__FUNCTION__);
        return -RT_ERROR;
    }

    LOG_I("WS2812_Init successful !!\n");

    return RT_EOK;
}
