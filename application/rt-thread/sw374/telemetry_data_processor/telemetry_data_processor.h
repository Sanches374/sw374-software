#ifndef ELEMETRY_DATA_PROCESSOR_H
#define ELEMETRY_DATA_PROCESSOR_H

// #ifdef __cplusplus
// extern "C" {
// #endif

#include <math.h> 
#include <cJSON.h>
// 配置ulog
#ifdef SW374_LVGL_UI
    #include "LVGL_SW_F1_Panel_1.h"
#endif
#include "LVGL_SW_F1_Panel_1.h"
#include "SW_conf.h"



#define cJson_Message_QUEUE_SIZE 64

// 初始化函数 - 创建消息队列和启动处理器
int data_proc_system_init(void);

// 数据类型转换
SW_DATA_TYPE::DiffLapTimeData convertDoubleToDiffLapTimeData(double diff);
SW_DATA_TYPE::LapTimeData convertDoubleToLapTimeData(const std::string& timeStr);


class DataProcessor {
public:
    DataProcessor();
    ~DataProcessor();
    void start(void);
    void run(void);
private:
    rt_thread_t thread_;
    SW_DATA_TYPE::LVGL_SW_Pannel_Data data;
    
    void process_json(cJSON *json);
    
};

void thread_entry(void *param);
// static void thread_entry(void *param);
// #ifdef __cplusplus
// }
// #endif


#endif