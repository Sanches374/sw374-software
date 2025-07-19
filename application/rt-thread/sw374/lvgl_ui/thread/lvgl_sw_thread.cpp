/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <rtthread.h>
#include <stdio.h>

// #include "test_ui.h"
#include "lvgl_sw_thread.h"
#include "LVGL_SW_Base_Panel.h"

#define THREAD_PRIORITY 20
#define THREAD_STACK_SIZE 8192 * 8
#define THREAD_TIMESLICE 5


#include "__dso_handle.h"

static rt_thread_t tid1 = RT_NULL;



void update_simulated_data(LVGL_SW_Base_Panel * SW_pannel) {
    static int frame_count = 0;
    frame_count++;

SW_DATA_TYPE::LVGL_SW_Pannel_Data data = {
        .Laptime = {rand() % 2, 1, rand() % 60, rand() % 1000}, // 模拟单圈时间为1:20.123
        .RPM = 8000 + (rand() % 5000), // 模拟RPM在8000到13000之间
        .Diff_self = {static_cast<SW_DATA_TYPE::DiffLapTimeData::DiffSign>(rand() % 3 - 1), rand() % 2, 1, rand() % 60, rand() % 1000}, // 模拟速度差
        .Diff_front = {SW_DATA_TYPE::DiffLapTimeData::DiffSign::POSITIVE, rand() % 2, 1, rand() % 60, rand() % 1000}, // 模拟前方速度差
        .Diff_after = {SW_DATA_TYPE::DiffLapTimeData::DiffSign::NEGATIVE, 0, 0, rand() % 60, rand() % 1000}, // 模拟后方速度差
        .Speed = 100 + (rand() % 221), // 模拟车速在100到320之间
        .Gear = static_cast<SW_DATA_TYPE::GearSet>((frame_count % 100) %10 - 1), // 模拟档位从0到8，-1表示倒档
        .Laps = {(1 + frame_count % 6300 / 100), 63}, // 模拟完成的圈数
        .Pos = {(1 + frame_count % 4000 / 200), 20}, // 模拟位置
        .SOC = rand() % 101, // 模拟电池电量在0到100之间
        .LastLapTime = {rand() % 2, 1, rand() % 60, rand() % 1000}, // 上一圈时间为1:22.123
        .BestLapTime = {0, 1, rand() % 60, rand() % 1000}, // 最佳单圈时间为1:19.456
        .DeltaFuel = static_cast<double>((((rand() % 100)) / 50) - 1.0),
        .FL_TireWear = rand() % 100, // 模拟前左轮胎磨损
        .FL_TireTemperature = 120 + (rand() % 21), // 模拟前左轮胎温度在80到100之间
        .FL_TirePressure = double(20.5 + (rand() % 151) / 50.0), // 模拟前左轮胎压力在1.5到2.0之间
        .FR_TireWear = rand() % 100, // 模拟前右轮胎磨损
        .FR_TireTemperature = 120 + (rand() % 21), // 模拟前右轮胎温度在80到100之间
        .FR_TirePressure = double(20.5 + (rand() % 151) / 50.0), // 模拟前右轮胎压力在1.5到2.0之间
        .RL_TireWear = rand() % 100, // 模拟后左轮胎磨损
        .RL_TireTemperature = 120 + (rand() % 21), // 模拟后左轮胎温度在80到100之间
        .RL_TirePressure = double(20.5 + (rand() % 151) / 50.0), // 模拟后左轮胎压力在1.5到2.0之间
        .RR_TireWear = rand() % 100, // 模拟后右轮胎磨损
        .RR_TireTemperature = 120 + (rand() % 21), // 模拟后右轮胎温度在80到100之间
        .RR_TirePressure = double(20.5 + (rand() % 151) / 50.0), // 模拟后右轮胎压力在1.5到2.0之间
        .Overtake = static_cast<SW_DATA_TYPE::OvertakeStatus>(rand() % 4),
        .Drs = static_cast<SW_DATA_TYPE::DrsStatus>(rand() % 4),
        .TireType_Now = static_cast<SW_DATA_TYPE::TireType>(rand() % 6), // 模拟当前轮胎类型
        .TireType_Next = static_cast<SW_DATA_TYPE::TireType>(rand() % 6), // 模拟下一个轮胎类型
        .SpecialEventStatusCode = static_cast<SW_DATA_TYPE::SpecialEvent>(rand() % 7)
    };
    /*

    // 更新RPM（模拟发动机转速）
    SW_pannel.setRPM(data.RPM);

    // 更新速度（模拟车速）
    SW_pannel.setSpeed(data.Speed);

    // 更新档位（模拟换挡）
    GearSet gear = static_cast<GearSet>(frame_count % 10 - 1);    // 模拟档位从0到8，-1表示倒档
    SW_pannel.setGear(gear);

    // 更新圈速数据（模拟计时）
    SW_pannel.setLaptime(data.Laptime);
    SW_pannel.setBestLapTime(data.BestLapTime);
    SW_pannel.setLastLapTime(data.LastLapTime);

    // 更新速度差数据
    SW_pannel.setDiff_front(data.Diff_front);
    SW_pannel.setDiff_after(data.Diff_after);
    SW_pannel.setDiff_self(data.Diff_self);

    // 更新电池电量（模拟电池状态）
    SW_pannel.setSOC(data.SOC);

    SW_pannel.setFLWear(data.FL_TireWear);
    SW_pannel.setFRWear(data.FR_TireWear);
    SW_pannel.setRLWear(data.RL_TireWear);
    SW_pannel.setRRWear(data.RR_TireWear);
    SW_pannel.setFLPressure(data.FL_TirePressure);
    SW_pannel.setFRPressure(data.FR_TirePressure);
    SW_pannel.setRLPressure(data.RL_TirePressure);
    SW_pannel.setRRPressure(data.RR_TirePressure);
    SW_pannel.setFLTemperature(data.FL_TireTemperature);
    SW_pannel.setFRTemperature(data.FR_TireTemperature);
    SW_pannel.setRLTemperature(data.RL_TireTemperature);
    SW_pannel.setRRTemperature(data.RR_TireTemperature);
    SW_pannel.setDeltaFuel(data.DeltaFuel);
    SW_pannel.setLaps(data.Laps);
    SW_pannel.setPos(data.Pos);



    switch (frame_count % 400 / 20)
    {
    case 0: // 不操作初始化的界面
        break;
    case 1: // 模拟默认面板
        SW_pannel.setOvertakeAvailable();
        SW_pannel.setDefaultPanel(data);
        break;
    case 2: // 模拟红旗状态
        SW_pannel.setRedFlagPanel();
        break;
    case 3: // 模拟默认面板
        SW_pannel.setDefaultPanel(data);
        break;
    case 4: // 模拟虚拟安全车状态
        SW_pannel.setVSCPanel();
        break;
    case 5: // 模拟默认面板
        SW_pannel.setDefaultPanel(data);
        break;
    case 6: // 模拟安全车状态
        SW_pannel.setSCPanel();
        break;
    case 7: // 模拟默认面板
        SW_pannel.setDefaultPanel(data);
        break;
    case 8: // 模拟绿旗状态
        SW_pannel.setGreenFlagPanel();
        break;
    case 9: // 模拟默认面板
        SW_pannel.setDefaultPanel(data);
        break;
    case 10: // 模拟黄旗状态
        SW_pannel.setYellowFlagPanel();
        break;
    case 11: // 模拟默认面板
        SW_pannel.setDefaultPanel(data);
        break;
    case 12: // 模拟蓝旗状态
        SW_pannel.setBlueFlagPanel();
        break;
    case 13:  // 恢复默认面板
        SW_pannel.setDefaultPanel(data);
        break;
    default:
        break;
    }
    if (frame_count % 10 < 5) {
        SW_pannel.setOvertakeStatus(true); // 模拟超车状态为true
    } else {
        SW_pannel.setOvertakeStatus(false); // 模拟超车状态为false
    }
    */
    SW_pannel->setCurrentData(data);
}



/* 线程1的入口函数 */
static void lvgl_sw_thread_entry(void *parameter)
{
    while (1)
    {
        
        rt_thread_mdelay(5);
        // thread_count++;
        // rt_thread_mdelay(1000);
    }
}

/* 线程示例 */
int lvgl_sw_thread(void)
{
    
    tid1 = rt_thread_create("lvgl_sw_thread",
                            lvgl_sw_thread_entry, RT_NULL,
                            THREAD_STACK_SIZE,
                            THREAD_PRIORITY, THREAD_TIMESLICE);

    /* 如果获得线程控制块，启动这个线程 */
    if (tid1 != RT_NULL)
        rt_thread_startup(tid1);

    return 0;
}







