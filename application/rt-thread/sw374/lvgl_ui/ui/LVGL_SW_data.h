#ifndef _LVGL_SW_DATA_H_
#define _LVGL_SW_DATA_H_

#include "SW_data.h"

// 屏幕所用的数据类型
namespace SW_DATA_TYPE {
    class LVGL_SW_Pannel_Data
    {
    public:
        LapTimeData Laptime = {0,0,0,0};
        int RPM = 0;
        DiffLapTimeData Diff_self = {DiffLapTimeData::DiffSign::NONE,0,0,0,0};
        DiffLapTimeData Diff_front = {DiffLapTimeData::DiffSign::NONE,0,0,0,0}; // 与前一名速度差数据
        DiffLapTimeData Diff_after = {DiffLapTimeData::DiffSign::NONE,0,0,0,0}; // 与后一名速度差数据
        int Speed = 0;
        GearSet Gear = Gear_N;
        LapData Laps = {0, 0};
        PosData Pos = {0, 0};
        int SOC = 0;
        LapTimeData LastLapTime = {0,0,0,0};
        LapTimeData BestLapTime = {0,0,0,0};
        double DeltaFuel = 0.0;

        // 轮胎数据
        int FL_TireWear = 0;
        int FL_TireTemperature = 0;
        double FL_TirePressure = 0.0;
        int FR_TireWear = 0;
        int FR_TireTemperature = 0;
        double FR_TirePressure = 0.0;
        int RL_TireWear = 0;
        int RL_TireTemperature = 0;
        double RL_TirePressure = 0.0;
        int RR_TireWear = 0;
        int RR_TireTemperature = 0;
        double RR_TirePressure = 0.0;

        OvertakeStatus Overtake = OvertakeStatus::OVERTAKE_DISAVAILABLE; // 是否启用超车
        DrsStatus Drs = DrsStatus::DRS_DISAVAILABLE; // 是否启用DRS

        TireType TireType_Now = TireType::UNKNOWN; // 当前轮胎类型
        TireType TireType_Next = TireType::UNKNOWN; // 下一个轮胎类型

        SpecialEvent SpecialEventStatusCode = SpecialEvent::NONE;

    };
}



#endif  // _LVGL_SW_DATA_H_
