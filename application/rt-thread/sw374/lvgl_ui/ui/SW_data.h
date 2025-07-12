#ifndef _SW_DATA_H_
#define _SW_DATA_H_

namespace SW_DATA_TYPE {
    enum GearSet {Gear_R = -1, Gear_N = 0, Gear_1 = 1, Gear_2, Gear_3, Gear_4, Gear_5, Gear_6, Gear_7, Gear_8};
    enum TireType {UNKNOWN = 0, HARD = 1, MEDIUM = 2, SOFT = 3, INTER = 4, WET = 5};
    enum SpecialEvent {NONE = 0, REDFLAG = 1, VSC = 2, SC = 3, GREENFLAG = 4, YELLOWFLAG = 5, BLUEFLAG = 6};

    enum OvertakeStatus 
    {
        OVERTAKE_DISAVAILABLE = 0,  // 超车不可用
        OVERTAKE_AVAILABLE = 1,      // 超车可用（无用的状态）
        OVERTAKE_DISABLED = 2,       // 超车可用但未启用
        OVERTAKE_ENABLED = 3         // 超车启用
    };

    enum DrsStatus 
    {
        DRS_DISAVAILABLE = 0,        // DRS不可用 （赛会关闭DRS）
        DRS_AVAILABLE = 1,           // DRS可用 （赛会开启DRS）
        DRS_DISABLED = 2,            // DRS可用但未启用 （过DRS检测线在1s内，DRS激活）
        DRS_ENABLED = 3              // DRS启用 （DRS打开）
    };

    typedef SpecialEvent* SpecialEventStatus;
    // class SpecialEventStatus {
    // public:
    //     // bool status = false;
    //     SpecialEvent * Code = nullptr;
    //     // SpecialEvent isRedFlag = SpecialEvent::REDFLAG; // 是否红旗状态
    //     // SpecialEvent isVSC = SpecialEvent::VSC; // 是否虚拟安全车状态
    //     // SpecialEvent isSC = SpecialEvent::SC; // 是否安全车状态
    //     // SpecialEvent isGreenFlag = SpecialEvent::GREENFLAG; // 是否绿旗状态
    //     // SpecialEvent isYellowFlag = SpecialEvent::YELLOWFLAG; // 是否黄旗状态
    //     // SpecialEvent isBlueFlag = SpecialEvent::BLUEFLAG; // 是否蓝旗状态
    // };

    // 圈速数据
    class LapTimeData {
    public:
        int hour;
        int minute;
        int second;
        int millisecond; // 毫秒部分
        bool operator==(const LapTimeData& other) const {
            return this->hour == other.hour &&
                this->minute == other.minute &&
                this->second == other.second &&
                this->millisecond == other.millisecond;
        }
        bool operator<(const LapTimeData& other) const {
            if (this->hour != other.hour) return this->hour < other.hour;
            if (this->minute != other.minute) return this->minute < other.minute;
            if (this->second != other.second) return this->second < other.second;
            return this->millisecond < other.millisecond;
        }
        bool operator>(const LapTimeData& other) const {
            if (this->hour != other.hour) return this->hour > other.hour;
            if (this->minute != other.minute) return this->minute > other.minute;
            if (this->second != other.second) return this->second > other.second;
            return this->millisecond > other.millisecond;
        }
    };

    // 圈速差数据
    class DiffLapTimeData {
    public:
        enum class DiffSign { NONE = 0, POSITIVE = 1, NEGATIVE = -1 };
        DiffSign diffsign;
        int hour;
        int minute;
        int second;
        int millisecond; // 毫秒部分
        bool operator==(const DiffLapTimeData& other) const {
            return this->diffsign == other.diffsign &&
                this->hour == other.hour &&
                this->minute == other.minute &&
                this->second == other.second &&
                this->millisecond == other.millisecond;
        }
    };

    // 排名数据
    class PosData {
    public:
        int pos; // 排名位置
        int total; // 总的参赛人数
    };

    // 圈数数据
    class LapData {
    public:
        int laps; // 完成的圈数
        int total; // 总的圈数
    };

}



#endif// _SW_DATA_H_