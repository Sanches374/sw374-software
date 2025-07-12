#ifndef _LVGL_SW_F1_Pannel_1_H_
#define _LVGL_SW_F1_Pannel_1_H_

#include <string>
#include <iostream>
#include <sstream>

#include <iomanip>


#include <cmath>
#include <string>
#include <algorithm>
#include <climits>

#include "lvgl/lvgl.h"


#include "LVGL_SW_box.h"
#include "LVGL_SW_text.h"
#include "LVGL_SW_data.h"
#include "LVGL_SW_arc.h"
#include "LVGL_SW_Base_Panel.h"

const double EPSINON = 0.00001;

// using namespace SW_DATA_TYPE;
// #include "__dso_handle.h"

std::string to_string_with_precision(double value, size_t precision = 1);

class LVGL_SW_F1_Panel_1 : public LVGL_SW_Base_Panel {
private:
    // lv_obj_t* mainscr; // LVGL基础屏幕对象
    LVGL_SW_box box;  // 外显示框
    LVGL_SW_box box_laptime;  // 单圈时间显示框
    LVGL_SW_box box_rpm;  // RPM显示框
    LVGL_SW_box box_diff;  // 速度差显示框
    LVGL_SW_box box_speed;  // 速度显示框
    LVGL_SW_box box_laps;  // 圈数显示框
    LVGL_SW_box box_gear;  // 档位显示框
    LVGL_SW_box box_fl;  // 前左轮胎磨损显示框
    LVGL_SW_box box_fr;  // 前右轮胎磨损显示框
    LVGL_SW_box box_rl;  // 后左轮胎磨损显示框
    LVGL_SW_box box_rr;  // 后右轮胎磨损显示框
    LVGL_SW_box box_pos;  // 位置显示框
    LVGL_SW_box box_soc;  // 电池电量显示框
    LVGL_SW_box box_lastandbeatlaptime;  // 上一圈和最快单圈时间显示框
    LVGL_SW_box box_overtake;  // 超车提示显示框
    LVGL_SW_box box_fuel;  // 燃料显示框
    LVGL_SW_box box_drs;  // DRS状态文本
    LVGL_SW_box box_socbargroup[20];

    LVGL_SW_text text_laptime;  // 单圈时间文本
    LVGL_SW_text text_rpm;  // RPM文本
    LVGL_SW_text text_diff;  // 速度差文本
    LVGL_SW_text text_diff_front;  // 前方速度差文本
    LVGL_SW_text text_diff_after;  // 后方速度差文本
    LVGL_SW_text text_speed;  // 速度文本
    LVGL_SW_text text_laps_laps;  // 圈数文本
    LVGL_SW_text text_laps_total;  // 总圈数文本
    LVGL_SW_text text_gear;  // 档位文本
    LVGL_SW_text text_fl_wear;  // 前左轮胎磨损文本
    LVGL_SW_text text_fr_wear;  // 前右轮胎磨损文本
    LVGL_SW_text text_rl_wear;  // 后左轮胎磨损文本
    LVGL_SW_text text_rr_wear;  // 后右轮胎磨损文本
    LVGL_SW_text text_fl_temp;  // 前左轮胎温度文本
    LVGL_SW_text text_fr_temp;  // 前右轮胎温度文本
    LVGL_SW_text text_rl_temp;  // 后左轮胎温度文本
    LVGL_SW_text text_rr_temp;  // 后右轮胎温度文本
    LVGL_SW_text text_fl_pressure;  // 前左轮胎压力文本
    LVGL_SW_text text_fr_pressure;  // 前右轮胎压力文本
    LVGL_SW_text text_rl_pressure;  // 后左轮胎压力文本
    LVGL_SW_text text_rr_pressure;  // 后右轮胎压力文本
    LVGL_SW_text text_pos_pos;  // 位置文本
    LVGL_SW_text text_pos_total;  // 总位置文本
    LVGL_SW_text text_soc;  // 电池电量文本
    LVGL_SW_text text_overtake;  // 超车提示文本
    LVGL_SW_text text_lastlaptime;  // 上一圈时间文本
    LVGL_SW_text text_bestlaptime;  // 最佳单圈时间文本
    LVGL_SW_text text_deltafuel;  // 燃料文本


    LVGL_SW_text text_drs;  // DRS状态文本
    LVGL_SW_arc arc_tire_type_now; // 当前轮胎类型图标
    LVGL_SW_arc arc_tire_type_next; // 下一个轮胎类型图标

    // lv_obj_t * arc_tire_type_now; // 当前轮胎类型图标
    // lv_obj_t * arc_tire_type_next; // 下一个轮胎类型图标
    LVGL_SW_text Tire_Type_Now;  // 当前轮胎类型显示对象
    LVGL_SW_text Tire_Type_Next;  // 下一个轮胎类型显示对象



    SW_DATA_TYPE::LVGL_SW_Pannel_Data last_data; // 用于存储上一次显示的数据
    // LVGL_SW_Pannel_Data current_data; // 当前数据存储

    // lv_timer_t* update_timer;
public:

    void setSOCBar(int);
    void setLaptimeData(LVGL_SW_text &text, SW_DATA_TYPE::LapTimeData laptime);
    void setDiffData(LVGL_SW_text &text, SW_DATA_TYPE::DiffLapTimeData diff);
    void setTextSpecialEvents(void);
    void setPannelColor(uint32_t color);

    

    // 设置显示内容
    // 显示圈速数据
    void setLaptime(SW_DATA_TYPE::LapTimeData laptime);
    void setLastLapTime(SW_DATA_TYPE::LapTimeData lastlaptime);
    void setBestLapTime(SW_DATA_TYPE::LapTimeData bestlaptime);

    // 显示速度差数据
    void setDiff_self(SW_DATA_TYPE::DiffLapTimeData diff);
    void setDiff_front(SW_DATA_TYPE::DiffLapTimeData diff);
    void setDiff_after(SW_DATA_TYPE::DiffLapTimeData diff);

    // 显示基本信息
    void setRPM(int rpm);
    void setSpeed(int speed);
    void setGear(SW_DATA_TYPE::GearSet gear);

    void setLaps(SW_DATA_TYPE::LapData laps);
    void setPos(SW_DATA_TYPE::PosData pos);
    void setSOC(int soc);
    void setDeltaFuel(double fuel);

    // 显示轮胎数据
    void setFLWear(int wear);
    void setFLTemperature(int temperature);
    void setFLPressure(double pressure);
    void setFRWear(int wear);
    void setFRTemperature(int temperature);
    void setFRPressure(double pressure);
    void setRLWear(int wear);
    void setRLTemperature(int temperature);
    void setRLPressure(double pressure);
    void setRRWear(int wear);
    void setRRTemperature(int temperature);
    void setRRPressure(double pressure);

    void setTireTypeNow(SW_DATA_TYPE::TireType tire_type);
    void setTireTypeNext(SW_DATA_TYPE::TireType tire_type);

// public:
    // 构造函数
    LVGL_SW_F1_Panel_1();

    // 析构函数
    ~LVGL_SW_F1_Panel_1();



    void SpecialEventsDetect(void);
    // 设置面板颜色
    void setRedFlagPanel(void);
    void setVSCPanel(void);
    void setSCPanel(void);
    void setGreenFlagPanel(void);
    void setYellowFlagPanel(void);
    void setBlueFlagPanel(void);
    void setDefaultPanel(SW_DATA_TYPE::LVGL_SW_Pannel_Data data);

    // void setCurrentData(SW_DATA_TYPE::LVGL_SW_Pannel_Data data);
    
    // 设置超车和DRS状态
    void setOvertakeStatus(SW_DATA_TYPE::OvertakeStatus status);
    void setDrsStatus(SW_DATA_TYPE::DrsStatus status);

    void UpdateAllData(void);

    void TimerUpdate(void);
};



#endif // _LVGL_SW_F1_Pannel_1_H_

