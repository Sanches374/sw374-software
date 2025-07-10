#include "telemetry_data_processor.h"

#include "LVGL_SW_F1_Panel_1.h"

#include "SW_conf.h"

#ifdef __cplusplus  // 仅在C++编译时使用extern "C"
extern "C" {
#endif
    
    #include <math.h> 
    #include <cJSON.h>
    // 配置ulog
    #define LOG_TAG "Telemetry_data_processor"
    #include <ulog.h>

}

struct cJson_msg {
    cJSON *json;
};
#define cJson_Message_QUEUE_SIZE 32
// 全局消息队列指针
extern rt_mq_t cJson_msg_queue;
// 全局显示变量
extern LVGL_SW_Base_Panel * p_pannel;
// extern SW_WS2812 * p_SW_WS2812;

SW_DATA_TYPE::DiffLapTimeData convertDoubleToDiffLapTimeData(double diff) {
    SW_DATA_TYPE::DiffLapTimeData data;

    // 设置符号
    if (diff > 0.0) {
        data.diffsign = SW_DATA_TYPE::DiffLapTimeData::DiffSign::POSITIVE;
    } else if (diff < 0.0) {
        data.diffsign = SW_DATA_TYPE::DiffLapTimeData::DiffSign::NEGATIVE;
    } else {
        data.diffsign = SW_DATA_TYPE::DiffLapTimeData::DiffSign::NONE;
    }

    // 计算总毫秒数（四舍五入）
    // long totalMilliseconds = static_cast<long>(std::round(std::abs(diff) * 1000.0));

    double absDiff = fabs(diff);  // 使用 fabs 替代 std::abs
    long totalMilliseconds = static_cast<long>(absDiff * 1000.0 + 0.5);

    // 分解时间单位
    data.hour = static_cast<int>(totalMilliseconds / 3600000);  // 1小时 = 3600000毫秒
    long remaining = totalMilliseconds % 3600000;

    data.minute = static_cast<int>(remaining / 60000);         // 1分钟 = 60000毫秒
    remaining %= 60000;

    data.second = static_cast<int>(remaining / 1000);          // 1秒 = 1000毫秒
    data.millisecond = static_cast<int>(remaining % 1000);

    return data;
}

SW_DATA_TYPE::LapTimeData convertDoubleToLapTimeData(const std::string& timeStr) {
    SW_DATA_TYPE::LapTimeData result = {0, 0, 0, 0};
    
    const char* str = timeStr.c_str();
    char* endptr = nullptr;
    
    // 解析分钟部分
    long totalMinutes = std::strtol(str, &endptr, 10);
    if (endptr == str || *endptr != ':') return result; // 无效格式
    
    // 解析秒部分
    str = endptr + 1; // 跳过冒号
    long seconds = std::strtol(str, &endptr, 10);
    if (endptr - str != 2 || *endptr != ':') return result; // 需要两位秒数
    
    // 解析毫秒部分
    str = endptr + 1; // 跳过冒号
    long milliseconds = std::strtol(str, &endptr, 10);
    if (endptr - str != 3) return result; // 需要三位毫秒数
    
    // 验证范围
    if (totalMinutes < 0 || seconds < 0 || seconds > 59 || 
        milliseconds < 0 || milliseconds > 999) {
        return result;
    }
    
    // 计算小时和分钟
    result.hour = static_cast<int>(totalMinutes / 60);
    result.minute = static_cast<int>(totalMinutes % 60);
    result.second = static_cast<int>(seconds);
    result.millisecond = static_cast<int>(milliseconds);
    
    return result;

}

/* 速度处理线程类 */
class DataProcessor {
public:
    DataProcessor() : thread_(RT_NULL) {}
    
    ~DataProcessor() {
        if (thread_) {
            rt_thread_delete(thread_);
        }
    }
    
    void start() {
        thread_ = rt_thread_create("data_proc",
                                  &DataProcessor::thread_entry,
                                  this,
                                  DataProcessor_THREAD_STACK_SIZE,
                                  DataProcessor_THREAD_PRIORITY,
                                  DataProcessor_THREAD_TIMESLICE);
        if (thread_) {
            rt_thread_startup(thread_);
            LOG_I("Data processor started\n");
        } else {
            LOG_E("Failed to create Data processor\n");
        }
    }
    
private:
    rt_thread_t thread_;
    SW_DATA_TYPE::LVGL_SW_Pannel_Data data;

    void run() {
        struct cJson_msg msg;
        
        while (1) {
            /* 从消息队列接收数据 */
            if (rt_mq_recv(cJson_msg_queue, &msg, sizeof(msg), RT_WAITING_FOREVER) == RT_EOK) {
                process_json(msg.json);
            }
        }
    }
    
    void process_json(cJSON *json) {
        if (!json) {
            LOG_E("Invalid JSON pointer\n");
            return;
        }
        
//----------------------------在此之内处理数据------------------------------------//
// 处理 CAR_DATA
        if (cJSON_GetObjectItem(json, "CAR_DATA"))
        {
            cJSON * CAR_DATA_json = cJSON_GetObjectItem(json, "CAR_DATA");
    // 速度         
            cJSON *speed_json = cJSON_GetObjectItem(CAR_DATA_json, "speed");
            LOG_D("JSON text: %s\n", cJSON_Print(speed_json));
            if (speed_json && cJSON_IsNumber(speed_json)) 
            {
                int speed = speed_json->valueint;
                // 实际处理逻辑
                LOG_D("Processing speed: %d\n", speed);
                this->data.Speed = speed;
            } 
            else {
                LOG_E("Invalid speed data\n");
            }
    // 转速
            cJSON *rpm_json = cJSON_GetObjectItem(CAR_DATA_json, "rpm");
            LOG_D("JSON text: %s\n", cJSON_Print(rpm_json));
            if (rpm_json && cJSON_IsNumber(rpm_json)) 
            {
                int rpm = rpm_json->valueint;
                // 实际处理逻辑
                LOG_D("Processing rpm: %d\n", rpm_json);
                this->data.RPM = rpm;
                // p_SW_WS2812->set_rpm(rpm);
            } 
            else {
                LOG_E("Invalid rpm data\n");
            }
    // 挡位
            cJSON *gear_json = cJSON_GetObjectItem(CAR_DATA_json, "gear");
            LOG_D("JSON text: %s\n", cJSON_Print(gear_json));
            if (gear_json && cJSON_IsString(gear_json))
            {
                std::string gear = std::string(gear_json->valuestring);
                LOG_D("Processing gear: %s\n", gear.c_str());
                if (gear == "N")
                    this->data.Gear = SW_DATA_TYPE::GearSet::Gear_N;
                else if (gear == "R")
                    this->data.Gear = SW_DATA_TYPE::GearSet::Gear_R;
                else
                    this->data.Gear = static_cast<SW_DATA_TYPE::GearSet>(atoi(gear.c_str()));
            }
            else {
                LOG_E("Invalid gear data\n");
            }
    // diff        
            cJSON *diff_json = cJSON_GetObjectItem(CAR_DATA_json, "delta");
            LOG_D("JSON text: %s\n", cJSON_Print(diff_json));
            if (diff_json && cJSON_IsNumber(diff_json)) 
            {
                LOG_D("diff laptime: %f\n", diff_json->valuedouble);
                this->data.Diff_self = convertDoubleToDiffLapTimeData(diff_json->valuedouble);
            } 
            else {
                LOG_E("Invalid speed data\n");
            }
    // current
            cJSON *current_json = cJSON_GetObjectItem(CAR_DATA_json, "current");
            LOG_D("JSON text: %s\n", cJSON_Print(current_json));
            if (current_json && cJSON_IsString(current_json))
            {
                std::string laptime = std::string(current_json->valuestring);
                LOG_D("Processing laptime: %s\n", laptime.c_str());
                this->data.Laptime = convertDoubleToLapTimeData(laptime);
            }
            else {
                LOG_E("Invalid gear data\n");
            }
        }

// 处理 TYRE_DATA
        if (cJSON_GetObjectItem(json, "TYRE_DATA"))
        {
            cJSON * TYRE_DATA_json = cJSON_GetObjectItem(json, "TYRE_DATA");
            cJSON * wear, * pr, *tr;
    // 左前         
            cJSON *FL_json = cJSON_GetObjectItem(TYRE_DATA_json, "FL");
            LOG_D("JSON text: %s\n", cJSON_Print(FL_json));
        // 轮胎剩余
            wear = cJSON_GetObjectItem(FL_json, "wear");
            if (wear && cJSON_IsNumber(wear)) 
            {
                // 实际处理逻辑
                LOG_D("TyreWearFrontLeft: %d\n", wear->valueint);
                this->data.FL_TireWear = wear->valueint;
            } 
            else {
                LOG_E("Invalid TyreWearFrontLeft data\n");
            }
        // 胎压
            pr = cJSON_GetObjectItem(FL_json, "pr");
            if (pr && cJSON_IsNumber(pr)) 
            {
                // 实际处理逻辑
                LOG_D("FL_TirePressure: %d\n", pr->valuedouble);
                this->data.FL_TirePressure = pr->valuedouble;
            } 
            else {
                LOG_E("Invalid FL_TirePressure data\n");
            }
        // 胎温
            tr = cJSON_GetObjectItem(FL_json, "tr");
            if (tr && cJSON_IsNumber(tr)) 
            {
                // 实际处理逻辑
                LOG_D("FL_TireTemperature: %f\n", tr->valueint);
                this->data.FL_TireTemperature = tr->valueint;
            } 
            else {
                LOG_E("Invalid FL_TireTemperature data\n");
            }
    // 右前
            cJSON *FR_json = cJSON_GetObjectItem(TYRE_DATA_json, "FR");
            LOG_D("JSON text: %s\n", cJSON_Print(FR_json));
        // 轮胎剩余
            wear = cJSON_GetObjectItem(FR_json, "wear");
            if (wear && cJSON_IsNumber(wear)) 
            {
                // 实际处理逻辑
                LOG_D("TyreWearFrontRight: %d\n", wear->valueint);
                this->data.FR_TireWear = wear->valueint;
            } 
            else {
                LOG_E("Invalid TyreWearFrontRight data\n");
            }
        // 胎压
            pr = cJSON_GetObjectItem(FR_json, "pr");
            if (pr && cJSON_IsNumber(pr)) 
            {
                // 实际处理逻辑
                LOG_D("FR_TirePressure: %f\n", pr->valuedouble);
                this->data.FR_TirePressure = pr->valuedouble;
            } 
            else {
                LOG_E("Invalid FR_TirePressure data\n");
            }
        // 胎温
            tr = cJSON_GetObjectItem(FR_json, "tr");
            if (tr && cJSON_IsNumber(tr)) 
            {
                // 实际处理逻辑
                LOG_D("FR_TireTemperature: %d\n", tr->valueint);
                this->data.FR_TireTemperature = tr->valueint;
            } 
            else {
                LOG_E("Invalid FR_TireTemperature data\n");
            }
    // 左后
            cJSON *RL_json = cJSON_GetObjectItem(TYRE_DATA_json, "RL");
            LOG_D("JSON text: %s\n", cJSON_Print(RL_json));
        // 轮胎剩余
            wear = cJSON_GetObjectItem(RL_json, "wear");
            if (wear && cJSON_IsNumber(wear)) 
            {
                // 实际处理逻辑
                LOG_D("TyreWearRearLeft: %d\n", wear->valueint);
                this->data.RL_TireWear = wear->valueint;
            } 
            else {
                LOG_E("Invalid TyreWearRearLeft data\n");
            }
        // 胎压
            pr = cJSON_GetObjectItem(RL_json, "pr");
            if (pr && cJSON_IsNumber(pr)) 
            {
                // 实际处理逻辑
                LOG_D("RL_TirePressure: %f\n", pr->valuedouble);
                this->data.RL_TirePressure = pr->valuedouble;
            } 
            else {
                LOG_E("Invalid RL_TirePressure data\n");
            }
        // 胎温
            tr = cJSON_GetObjectItem(RL_json, "tr");
            if (tr && cJSON_IsNumber(tr)) 
            {
                // 实际处理逻辑
                LOG_D("RL_TireTemperature: %d\n", tr->valueint);
                this->data.RL_TireTemperature = tr->valueint;
            } 
            else {
                LOG_E("Invalid RL_TireTemperature data\n");
            }
    // 右后
            cJSON *RR_json = cJSON_GetObjectItem(TYRE_DATA_json, "RR");
            LOG_D("JSON text: %s\n", cJSON_Print(RR_json));
        // 轮胎剩余
            wear = cJSON_GetObjectItem(RR_json, "wear");
            if (wear && cJSON_IsNumber(wear)) 
            {
                // 实际处理逻辑
                LOG_D("TyreWearRearRoght: %d\n", wear->valueint);
                this->data.RR_TireWear = wear->valueint;
            } 
            else {
                LOG_E("Invalid TyreWearRearRoght data\n");
            }
        // 胎压
            pr = cJSON_GetObjectItem(RR_json, "pr");
            if (pr && cJSON_IsNumber(pr)) 
            {
                // 实际处理逻辑
                LOG_D("RR_TirePressure: %f\n", pr->valuedouble);
                this->data.RR_TirePressure = pr->valuedouble;
            } 
            else {
                LOG_E("Invalid RR_TirePressure data\n");
            }
        // 胎温
            tr = cJSON_GetObjectItem(RR_json, "tr");
            if (tr && cJSON_IsNumber(tr)) 
            {
                // 实际处理逻辑
                LOG_D("RR_TireTemperature: %d\n", tr->valueint);
                this->data.RR_TireTemperature = tr->valueint;
            } 
            else {
                LOG_E("Invalid RR_TireTemperature data\n");
            }
        }

// 处理 LAP_DATA
        if (cJSON_GetObjectItem(json, "LAP_DATA"))
        {
            cJSON * LAP_DATA_json = cJSON_GetObjectItem(json, "LAP_DATA");
    // last
            cJSON *last_json = cJSON_GetObjectItem(LAP_DATA_json, "last");
            LOG_D("JSON text: %s\n", cJSON_Print(last_json));
            if (last_json && cJSON_IsString(last_json))
            {
                std::string laptime = std::string(last_json->valuestring);
                LOG_D("Processing laptime: %s\n", laptime.c_str());
                this->data.LastLapTime = convertDoubleToLapTimeData(laptime);
            }
            else {
                LOG_E("Invalid gear data\n");
            }
    // best
            cJSON *best_json = cJSON_GetObjectItem(LAP_DATA_json, "best");
            LOG_D("JSON text: %s\n", cJSON_Print(best_json));
            if (best_json && cJSON_IsString(best_json))
            {
                std::string laptime = std::string(best_json->valuestring);
                LOG_D("Processing laptime: %s\n", laptime.c_str());
                this->data.BestLapTime = convertDoubleToLapTimeData(laptime);
            }
            else {
                LOG_E("Invalid gear data\n");
            }
    // laps      
            cJSON *laps_json = cJSON_GetObjectItem(LAP_DATA_json, "laps");
            LOG_D("JSON text: %s\n", cJSON_Print(laps_json));
            if (laps_json && cJSON_IsNumber(laps_json)) 
            {
                LOG_D("current laps: %f\n", laps_json->valuedouble);
                this->data.Laps.laps = laps_json->valuedouble;
            } 
            else {
                LOG_E("JSON text: %s\n", cJSON_Print(laps_json));
                LOG_E("Invalid Current Laps data\n");
            }
    // totallaps
            cJSON *totallaps_json = cJSON_GetObjectItem(LAP_DATA_json, "totallaps");
            LOG_D("JSON text: %s\n", cJSON_Print(totallaps_json));
            if (totallaps_json && cJSON_IsNumber(totallaps_json)) 
            {
                int totallaps = totallaps_json->valuedouble;
                LOG_D("total laps: %f\n", totallaps);
                if (totallaps == 0)
                    this->data.Laps.total = totallaps - 1;
                else
                    this->data.Laps.total = totallaps;
            } 
            else {
                LOG_E("Invalid Total Laps data\n");
            }
    // pos      
            cJSON *pos_json = cJSON_GetObjectItem(LAP_DATA_json, "pos");
            LOG_D("JSON text: %s\n", cJSON_Print(pos_json));
            if (pos_json && cJSON_IsNumber(pos_json)) 
            {
                LOG_D("current position: %f\n", pos_json->valuedouble);
                this->data.Pos.pos = pos_json->valuedouble;
            } 
            else {
                LOG_E("Invalid Current Position data\n");
            }
    // totalpos
            cJSON *totalpos_json = cJSON_GetObjectItem(LAP_DATA_json, "totalpos");
            LOG_D("JSON text: %s\n", cJSON_Print(totalpos_json));
            if (totalpos_json && cJSON_IsNumber(totalpos_json)) 
            {
                LOG_D("total position: %f\n", totalpos_json->valuedouble);
                this->data.Pos.total = totalpos_json->valuedouble;
            } 
            else {
                LOG_E("Invalid Total Position data\n");
            }
        }



//------------------------------------------------------------------------------//
        p_pannel->setCurrentData(data);
        cJSON_Delete(json);  // 关键：处理完成后释放内存
    }
    
    static void thread_entry(void *param) {
        DataProcessor *self = static_cast<DataProcessor*>(param);
        self->run();
    }
};

/* 全局对象 */
DataProcessor data_processor;

/* 初始化函数 - 创建消息队列和启动处理器 */
static int data_proc_system_init(void) {
    // 创建消息队列
    cJson_msg_queue = rt_mq_create("cJson_msg_queue", 
                           sizeof(struct cJson_msg), 
                           cJson_Message_QUEUE_SIZE, 
                           RT_IPC_FLAG_FIFO);
    
    if (!cJson_msg_queue) {
        LOG_E("Failed to create message queue\n");
        return -1;
    }
    // 启动速度处理器
    data_processor.start();
    return 0;
}
INIT_APP_EXPORT(data_proc_system_init);  // 自动初始化



