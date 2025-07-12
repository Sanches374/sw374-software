#include "LVGL_SW_F1_Panel_1.h"
// using namespace SW_DATA_TYPE;
LVGL_SW_F1_Panel_1::LVGL_SW_F1_Panel_1() :
LVGL_SW_Base_Panel(),
box(2, 2, 796, 476, this->mainscr),
box_laptime(0, 0, 280, 80, this->mainscr),
box_rpm(280, 0, 240, 80, this->mainscr),  // RPM显示框
box_diff(280 + 240, 0, 280, 80, this->mainscr),  // 速度差显示框
box_speed(0, 80, 190, 120, this->mainscr),  // 速度显示框
box_laps(0, 80 + 120, 190, 120, this->mainscr),  // 圈数显示框
box_gear(190 + 120, 80, 180, 240, this->mainscr),  // 档位显示框背景
box_fl(190, 80, 120, 120, this->mainscr),  // 前左轮胎温度显示框
box_fr(190 + 120 + 180, 80, 120, 120, this->mainscr),  // 前右轮胎温度显示框
box_rl(190, 80 + 120, 120, 120, this->mainscr),  // 后左轮胎温度显示框
box_rr(190 + 120 + 180, 80 + 120, 120, 120, this->mainscr),  // 后右轮胎温度显示框
box_pos(190 + 120 + 180 + 120, 80, 190, 240, this->mainscr),  // 位置显示框
box_soc(0, 80 + 120 + 120, 160, 120, this->mainscr),  // 电池电量显示框
box_lastandbeatlaptime(160, 80 + 120 + 120 + 60, 480, 60, this->mainscr),  // 上一圈和最快单圈时间显示框背景
box_overtake(160, 80 + 120 + 120, 480, 60, this->mainscr),  // 超车显示框背景
box_fuel(160 + 480, 80 + 120 + 120, 160, 120, this->mainscr),  // 燃油显示框
box_drs(160 + 200 + 10, 80 + 120 + 120 + 60 + 15, 60, 30, this->mainscr),  // DRS状态文本框
box_socbargroup{
    LVGL_SW_box(0, 80 + 120 + 120 + 120, 800, 40, this->mainscr),  // 电池电量条组
    LVGL_SW_box(40,80+120+120+120,40,40, this->mainscr),
    LVGL_SW_box(80,80+120+120+120,40,40, this->mainscr),
    LVGL_SW_box(120,80+120+120+120,40,40, this->mainscr),
    LVGL_SW_box(160,80+120+120+120,40,40, this->mainscr),
    LVGL_SW_box(200,80+120+120+120,40,40, this->mainscr),
    LVGL_SW_box(240,80+120+120+120,40,40, this->mainscr),
    LVGL_SW_box(280,80+120+120+120,40,40, this->mainscr),
    LVGL_SW_box(320,80+120+120+120,40,40, this->mainscr),
    LVGL_SW_box(360,80+120+120+120,40,40, this->mainscr),
    LVGL_SW_box(400,80+120+120+120,40,40, this->mainscr),
    LVGL_SW_box(440,80+120+120+120,40,40, this->mainscr),
    LVGL_SW_box(480,80+120+120+120,40,40, this->mainscr),
    LVGL_SW_box(520,80+120+120+120,40,40, this->mainscr),
    LVGL_SW_box(560,80+120+120+120,40,40, this->mainscr),
    LVGL_SW_box(600,80+120+120+120,40,40, this->mainscr),
    LVGL_SW_box(640,80+120+120+120,40,40, this->mainscr),
    LVGL_SW_box(680,80+120+120+120,40,40, this->mainscr),
    LVGL_SW_box(720,80+120+120+120,40,40, this->mainscr),
    LVGL_SW_box(760,80+120+120+120,40,40, this->mainscr)
},
text_laptime(0, 0, 280, 80, this->mainscr),  // 单圈时间文本
text_rpm(280, 0, 240, 80, this->mainscr),  // RPM文本
text_diff(280 + 240, 0, 280, 80, this->mainscr),  // 速度差文本
text_diff_front(190 + 120 + 180 + 120, 80, 190, 80, this->mainscr),  // 前方速度差文本
text_diff_after(190 + 120 + 180 + 120, 80 + 160, 190, 80, this->mainscr),  // 后方速度差文本
text_speed(0, 80, 190, 120, this->mainscr),  // 速度文本
text_laps_laps(0, 80 + 120, 140, 120, this->mainscr),  // 圈数文本
text_laps_total(120, 80 + 120 + 30, 70, 120 - 30, this->mainscr),  // 总圈数文本
text_gear(190 + 120, 80, 180, 240, this->mainscr),  // 档位文本
text_fl_wear(190, 80, 120, 120, this->mainscr),  // 前左轮胎磨损文本
text_fr_wear(190 + 120 + 180, 80, 120, 120, this->mainscr),  // 前右轮胎磨损文本
text_rl_wear(190, 80 + 120, 120, 120, this->mainscr),  // 后左轮胎磨损文本
text_rr_wear(190 + 120 + 180, 80 + 120, 120, 120, this->mainscr),  // 后右轮胎磨损文本
text_fl_temp(190, 80, 60, 30, this->mainscr),  // 前左轮胎温度文本
text_fr_temp(190 + 120 + 180 + 60, 80, 60, 30, this->mainscr),  // 前右轮胎温度文本
text_rl_temp(190, 80 + 120 + 120 - 30, 60, 30, this->mainscr),  // 后左轮胎温度文本
text_rr_temp(190 + 120 + 180 + 60, 80 + 120 + 120 - 30, 60, 30, this->mainscr),  // 后右轮胎温度文本
text_fl_pressure(190 + 60, 80 + 120 - 30, 60, 30, this->mainscr),  // 前左轮胎压力文本
text_fr_pressure(190 + 120 + 180, 80 + 120 - 30, 60, 30, this->mainscr),  // 前右轮胎压力文本
text_rl_pressure(190 + 60, 80 + 120, 60, 30, this->mainscr),  // 后左轮胎压力文本
text_rr_pressure(190 + 120 + 180, 80 + 120, 60, 30, this->mainscr),  // 后右轮胎压力文本
text_pos_pos(190 + 120 + 180 + 120, 80, 140, 240, this->mainscr),  // 位置文本
text_pos_total(190 + 120 + 180 + 120 + 120, 80 + 30, 70, 240 - 30, this->mainscr),  // 总位置文本
text_soc(0, 80 + 120 + 120, 160, 120, this->mainscr),  // 电池电量文本
text_overtake(160, 80 + 120 + 120 - 10, 480, 60 + 20, this->mainscr),  // 超车提示文本框
text_lastlaptime(160, 80 + 120 + 120 + 60 - 10, 200, 60 + 20, this->mainscr),  // 上一圈时间文本
text_bestlaptime(160 + 240 + 40, 80 + 120 + 120 + 60 - 10, 200, 60 + 20, this->mainscr),  // 最佳单圈时间文本
text_deltafuel(160 + 480, 80 + 120 + 120, 160, 60, this->mainscr),  // 燃油文本
text_drs(160 + 200 + 10, 80 + 120 + 120 + 60, 60, 60, this->mainscr),  // DRS状态文本
arc_tire_type_now(655, 365, 70, 70, 6, this->mainscr),  // 当前轮胎类型图标
arc_tire_type_next(740, 385, 50, 50, 4, this->mainscr),  // 下一个轮胎类型图标
Tire_Type_Now(640, 350, 100, 100, this->mainscr),  // 当前轮胎类型显示对象
Tire_Type_Next(740, 385, 50, 50, this->mainscr)  // 下一个轮胎类型显示对象
{
    Init_LVGL_SW_box_style();
    Init_LVGL_SW_text_style();
    Init_LVGL_SW_windows_style();

    this->text_bestlaptime.setTextColor(lv_color_hex(0xBF40BF)); // 设置最佳单圈时间文本颜色为紫色
    this->text_rpm.setTextColor(lv_color_hex(LVGL_SW_F1_PANNEL_COLOR_RED)); // 设置RPM文本颜色红色

    for (int i = 0; i < 20; i++) {
        this->box_socbargroup[i].setBoxColor(lv_color_hex(LVGL_SW_F1_PANNEL_COLOR_GRAY)); // 设置SOC条颜色为灰色
    }
    this->text_gear.setFont(&F1_Display_Regular_180px_4bpp); // 设置默认档位字体
    // this->text_overtake.setLabelText(std::string("Overtake")); // 设置超车提示文本
    this->text_lastlaptime.setFont(&F1_Display_Regular_36px_4bpp); // 设置上一圈时间文本字体
    this->text_bestlaptime.setFont(&F1_Display_Regular_36px_4bpp); // 设置最佳单圈时间文本字体
    this->text_fl_pressure.setFont(&F1_Display_Regular_18px_4bpp); // 设置前左轮胎压力文本字体
    this->text_fr_pressure.setFont(&F1_Display_Regular_18px_4bpp); // 设置前右轮胎压力文本字体
    this->text_rl_pressure.setFont(&F1_Display_Regular_18px_4bpp); // 设置后左轮胎压力文本字体
    this->text_rr_pressure.setFont(&F1_Display_Regular_18px_4bpp); // 设置后右轮胎压力文本字体
    this->text_fl_temp.setFont(&F1_Display_Regular_18px_4bpp); // 设置前左轮胎温度文本字体
    this->text_fr_temp.setFont(&F1_Display_Regular_18px_4bpp); // 设置前右轮胎温度文本字体
    this->text_rl_temp.setFont(&F1_Display_Regular_18px_4bpp); // 设置后左轮胎温度文本字体
    this->text_rr_temp.setFont(&F1_Display_Regular_18px_4bpp); // 设置后右轮胎温度文本字体
    this->text_deltafuel.setFont(&F1_Display_Regular_24px_4bpp); // 设置燃料文本字体
    this->text_pos_pos.setFont(&F1_Display_Regular_72px_4bpp); // 设置位置文本字体
    this->text_pos_total.setFont(&F1_Display_Regular_36px_4bpp); // 设置总位置文本字体
    this->text_laps_laps.setFont(&F1_Display_Regular_72px_4bpp); // 设置圈数文本字体
    this->text_laps_total.setFont(&F1_Display_Regular_36px_4bpp); // 设置总圈数文本字体
    this->text_diff_front.setFont(&F1_Display_Regular_36px_4bpp); // 设置前方速度差文本字体
    this->text_diff_after.setFont(&F1_Display_Regular_36px_4bpp); // 设置后方速度差文本字体
    this->text_drs.setFont(&F1_Display_Regular_24px_4bpp); // 设置DRS状态文本字体
    this->text_drs.setLabelText(std::string("DRS")); // 设置DRS状态文本内容
    //this->text_drs.setBoxColor(LVGL_SW_F1_PANNEL_COLOR_BLACK);    // 设置DRS状态文本背景颜色为黑色
    //this->text_drs.setStyle(LVGL_SW_box_style); // 设置DRS状态文本样式
    this->Tire_Type_Next.setFont(&F1_Display_Regular_36px_4bpp); // 设置下一个轮胎类型显示对象字体
    this->arc_tire_type_now.setArcOpa(LV_OPA_TRANSP); // 设置当前轮胎类型图标透明度为0
    this->arc_tire_type_next.setArcOpa(LV_OPA_TRANSP); // 设置下一个轮胎类型图标透明度为0
}

// 析构函数
LVGL_SW_F1_Panel_1::~LVGL_SW_F1_Panel_1() {}

// void LVGL_SW_F1_Panel_1::setCurrentData(SW_DATA_TYPE::LVGL_SW_Pannel_Data data) {
//     this->current_data = data;
// }

void LVGL_SW_F1_Panel_1::setLaptimeData(LVGL_SW_text &text, SW_DATA_TYPE::LapTimeData laptime)
{
    std::string laptimeStr;
    if (laptime.hour == 0)
    {

        laptimeStr = (std::to_string(laptime.minute) + ":" +
            (laptime.second < 10 ? "0" : "") + std::to_string(laptime.second) + "." +
            (laptime.millisecond < 100 ? (laptime.millisecond < 10 ? "00" : "0") : "") + std::to_string(laptime.millisecond));
    }
    else
    {
        // 如果小时部分不为0，则格式化为 "HH:MM:SS"
        laptimeStr = std::to_string(laptime.hour) + ":" +
            (laptime.minute < 10 ? "0" : "") + std::to_string(laptime.minute) + ":" +
            (laptime.second < 10 ? "0" : "") + std::to_string(laptime.second);
    }
    text.setLabelText(laptimeStr);
}

void LVGL_SW_F1_Panel_1::setLaptime(SW_DATA_TYPE::LapTimeData laptime)
{
    if (laptime == this->last_data.Laptime) return; // 如果laptime与上次显示的相同，则不更新
    if (this->last_data.SpecialEventStatusCode == SW_DATA_TYPE::SpecialEvent::NONE)
    {
        if (this->last_data.Laptime.hour == 1 && laptime.hour == 0)
        {
            this->text_laptime.setTextColor(LVGL_SW_F1_PANNEL_COLOR_WHITE);
        }
        if (laptime.hour == 1 && this->last_data.Laptime.hour == 0)
        {
            this->text_laptime.setTextColor(lv_color_hex(LVGL_SW_F1_PANNEL_COLOR_RED)); // 红色
        }
    }
    this->setLaptimeData(this->text_laptime, laptime);
    this->last_data.Laptime = laptime; // 更新最后一次单圈时间

}

void LVGL_SW_F1_Panel_1::setLastLapTime(SW_DATA_TYPE::LapTimeData lastlaptime)
{
    // 如果lastlaptime与上次显示的相同，则不更新
    if (lastlaptime == this->last_data.LastLapTime) return;
    else {
        this->setLaptimeData(this->text_lastlaptime, lastlaptime);
        if (this->last_data.SpecialEventStatusCode == SW_DATA_TYPE::SpecialEvent::NONE) {
            if (this->last_data.LastLapTime < lastlaptime)
            {
                this->text_lastlaptime.setTextColor(LVGL_SW_F1_PANNEL_COLOR_RED); // 红色
            }
            else if (this->last_data.LastLapTime > lastlaptime)
            {
                this->text_lastlaptime.setTextColor(LVGL_SW_F1_PANNEL_COLOR_GREEN); // 绿色
            }
            else
            {
                this->text_lastlaptime.setTextColor(LVGL_SW_F1_PANNEL_COLOR_WHITE); // 白色
            }

        }
        this->last_data.LastLapTime = lastlaptime; // 更新最后一次单圈时间
    }
}

void LVGL_SW_F1_Panel_1::setBestLapTime(SW_DATA_TYPE::LapTimeData bestlaptime)
{
    // 如果bestlaptime与上次显示的相同，则不更新
    if (bestlaptime == this->last_data.BestLapTime) return;
    else {
        this->setLaptimeData(this->text_bestlaptime, bestlaptime);
        this->last_data.BestLapTime = bestlaptime; // 更新最后一次单圈时间
    }
}

void LVGL_SW_F1_Panel_1::setDiffData(LVGL_SW_text &text, SW_DATA_TYPE::DiffLapTimeData diff)
{
    std::string diffStr;
    if (this->last_data.SpecialEventStatusCode == SW_DATA_TYPE::SpecialEvent::NONE)
    {
        switch (diff.diffsign)
        {
        case SW_DATA_TYPE::DiffLapTimeData::DiffSign::NONE:
            text.setLabelText(std::string("0.0"));
            text.setTextColor(lv_color_hex(LVGL_SW_F1_PANNEL_COLOR_WHITE)); // 白色
            return; // 直接返回，不需要继续处理
        case SW_DATA_TYPE::DiffLapTimeData::DiffSign::POSITIVE:
            // 处理正差
            diffStr = "+";
            text.setTextColor(lv_color_hex(LVGL_SW_F1_PANNEL_COLOR_RED)); // 红色
            break;
        case SW_DATA_TYPE::DiffLapTimeData::DiffSign::NEGATIVE:
            // 处理负差
            diffStr = "-";
            text.setTextColor(lv_color_hex(LVGL_SW_F1_PANNEL_COLOR_GREEN)); // 绿色
            break;
        }
    }
    else
    {
        switch (diff.diffsign)
        {
        case SW_DATA_TYPE::DiffLapTimeData::DiffSign::NONE:
            text.setLabelText(std::string("0.0"));
            return; // 直接返回，不需要继续处理
        case SW_DATA_TYPE::DiffLapTimeData::DiffSign::POSITIVE:
            // 处理正差
            diffStr = "+";
            break;
        case SW_DATA_TYPE::DiffLapTimeData::DiffSign::NEGATIVE:
            // 处理负差
            diffStr = "-";
            break;
        }
    }
    text.setLabelText(diffStr);



    if (diff.hour == 0 && diff.minute == 0)
    {
        // 如果小时和分钟部分都为0，则格式化为 "SS.mmm"
        diffStr += (diff.second < 10 ? "0" : "") + std::to_string(diff.second) + "." +
            (diff.millisecond < 100 ? (diff.millisecond < 10 ? "00" : "0") : "") + std::to_string(diff.millisecond);
    }
    else if (diff.hour == 0)
    {
        diffStr += (std::to_string(diff.minute) + ":" +
            (diff.second < 10 ? "0" : "") + std::to_string(diff.second));
    }
    else
    {
        // 如果小时部分不为0，则格式化为 "HH:MM:SS"
        diffStr += std::to_string(diff.hour) + ":" +
            (diff.minute < 10 ? "0" : "") + std::to_string(diff.minute) + ":" +
            (diff.second < 10 ? "0" : "") + std::to_string(diff.second);
    }
    // 设置速度差文本
    text.setLabelText(diffStr);
}

void LVGL_SW_F1_Panel_1::setDiff_self(SW_DATA_TYPE::DiffLapTimeData diff)
{
    if (diff == this->last_data.Diff_self) {
        return; // 如果速度差没有变化，则不更新
    }
    else {
        // 更新速度差数据
        this->setDiffData(this->text_diff, diff);
        this->last_data.Diff_self = diff; // 更新最后一次速度差数据
    }
}

void LVGL_SW_F1_Panel_1::setDiff_front(SW_DATA_TYPE::DiffLapTimeData diff)
{
    if (diff == this->last_data.Diff_front) {
        return; // 如果前方速度差没有变化，则不更新
    }
    else {
        // 更新前方速度差数据
        this->setDiffData(this->text_diff_front, diff);
        this->last_data.Diff_front = diff; // 更新前方车手速度差数据
    }
}
void LVGL_SW_F1_Panel_1::setDiff_after(SW_DATA_TYPE::DiffLapTimeData diff)
{
    if (diff == this->last_data.Diff_after) {
        return; // 如果后方速度差没有变化，则不更新
    }
    else {
        // 更新后方速度差数据
        this->setDiffData(this->text_diff_after, diff);
        this->last_data.Diff_after = diff; // 更新后方车手速度差数据
    }
}

void LVGL_SW_F1_Panel_1::setRPM(int rpm)
{
    if (rpm == this->last_data.RPM) {
        return; // 如果RPM没有变化，则不更新
    }
    else {
        this->text_rpm.setLabelText(std::to_string(rpm));
    }
}


void LVGL_SW_F1_Panel_1::setSpeed(int speed)
{
    if (speed == this->last_data.Speed) {
        return; // 如果速度没有变化，则不更新
    }
    else {
        this->text_speed.setLabelText(std::to_string(speed));
        this->last_data.Speed = speed; // 更新最后一次速度
    }
}

void LVGL_SW_F1_Panel_1::setGear(SW_DATA_TYPE::GearSet gear)
{
    if (gear == this->last_data.Gear) {
        return; // 如果档位没有变化，则不更新
    }
    switch (gear)
    {
    case SW_DATA_TYPE::GearSet::Gear_R:
        this->text_gear.setLabelText(std::string("R"));
        break;
    case SW_DATA_TYPE::GearSet::Gear_N:
        this->text_gear.setLabelText(std::string("N"));
        break;
    default:
        this->text_gear.setLabelText(std::to_string(static_cast<int>(gear)));
        break;
    }
}

void LVGL_SW_F1_Panel_1::setLaps(SW_DATA_TYPE::LapData laps)
{
    if (laps.laps != this->last_data.Laps.laps) {
        this->text_laps_laps.setLabelText(std::to_string(laps.laps));
        this->last_data.Laps.laps = laps.laps; // 更新最后一次圈数
    }
    if (laps.total != this->last_data.Laps.total)
    {
        if (laps.total == -1)
            this->text_laps_total.setLabelText(std::string("/Inf"));
        else
            this->text_laps_total.setLabelText(std::string("/") + std::to_string(laps.total));
        this->last_data.Laps.total = laps.total;

    }


}
void LVGL_SW_F1_Panel_1::setPos(SW_DATA_TYPE::PosData pos)
{
    if (pos.pos != this->last_data.Pos.pos)
    {
        this->text_pos_pos.setLabelText(std::to_string(pos.pos));
        this->last_data.Pos.pos = pos.pos;
    }
    if (pos.total != this->last_data.Pos.total)
    {
        this->text_pos_total.setLabelText(std::string("/") + std::to_string(pos.total));
        this->last_data.Pos.total = pos.total;
    }
}

void LVGL_SW_F1_Panel_1::setSOC(int soc)
{
    if (this->last_data.SOC != soc)
    {
        this->text_soc.setLabelText(std::to_string(soc) + std::string("%"));
        this->setSOCBar(soc);
        this->last_data.SOC = soc;
    }
}

void LVGL_SW_F1_Panel_1::setSOCBar(int soc)
{
    int boxnum = soc / 5; // 假设每5%一个SOC条
    if (boxnum > this->last_data.SOC / 5) {
        // 点亮新条
        for (int i = this->last_data.SOC / 5; i < boxnum; i++) {
            if (i < 0) continue; // 跳过初始值

            // 前4条为红色，其余为绿色
            uint32_t color = (i < 4) ? LVGL_SW_F1_PANNEL_COLOR_RED : LVGL_SW_F1_PANNEL_COLOR_GREEN;
            this->box_socbargroup[i].setBoxColor(lv_color_hex(color));
        }
    } else {
        // 熄灭超出的条
        for (int i = this->last_data.SOC / 5 - 1; i >= boxnum; i--) {
            if (i < 0) break; // 防止越界
            this->box_socbargroup[i].setBoxColor(lv_color_hex(LVGL_SW_F1_PANNEL_COLOR_GRAY)); // 灰色
        }
    }
}



void LVGL_SW_F1_Panel_1::setDeltaFuel(double fuel)
{
    if (fuel - this->last_data.DeltaFuel < EPSINON) {
        return; // 如果燃料没有变化，则不更新
    }
    else {
        this->last_data.DeltaFuel = fuel; // 更新最后一次燃料数据
        if (this->last_data.SpecialEventStatusCode == SW_DATA_TYPE::SpecialEvent::NONE)
        {
            if ((fuel < 0.0)) {
                this->text_deltafuel.setTextColor(lv_color_hex(LVGL_SW_F1_PANNEL_COLOR_RED)); // 红色
                this->text_deltafuel.setLabelText(to_string_with_precision(fuel,2) + std::string(" Laps"));
            }
            else{
                this->text_deltafuel.setTextColor(lv_color_hex(LVGL_SW_F1_PANNEL_COLOR_GREEN)); // 绿色
                this->text_deltafuel.setLabelText(std::string("+") + to_string_with_precision(fuel,2) + std::string(" Laps"));
            }
        }
        else
        {
            if ((fuel < 0.0)) {
                this->text_deltafuel.setLabelText(to_string_with_precision(fuel,2) + std::string(" Laps"));
            }
            else{
                this->text_deltafuel.setLabelText(std::string("+") + to_string_with_precision(fuel,2) + std::string(" Laps"));
            }
        }

    }
}



void LVGL_SW_F1_Panel_1::setFLWear(int wear)
{
    if (wear == this->last_data.FL_TireWear) {
        return; // 如果前左轮胎磨损没有变化，则不更新
    }
    else {
        this->text_fl_wear.setLabelText(std::to_string(wear));
        this->last_data.FL_TireWear = wear; // 更新最后一次前左轮胎磨损数据
    }
}
void LVGL_SW_F1_Panel_1::setFRWear(int wear)
{
if (wear == this->last_data.FR_TireWear) {
        return; // 如果前右轮胎磨损没有变化，则不更新
    }
    else {
        this->text_fr_wear.setLabelText(std::to_string(wear));
        this->last_data.FR_TireWear = wear; // 更新最后一次前右轮胎磨损数据
    }
}
void LVGL_SW_F1_Panel_1::setRLWear(int wear)
{
    if (wear == this->last_data.RL_TireWear) {
        return; // 如果后左轮胎磨损没有变化，则不更新
    }
    else {
        this->text_rl_wear.setLabelText(std::to_string(wear));
        this->last_data.RL_TireWear = wear; // 更新最后一次后左轮胎磨损数据
    }
}
void LVGL_SW_F1_Panel_1::setRRWear(int wear)
{
    if (wear == this->last_data.RR_TireWear) {
        return; // 如果后右轮胎磨损没有变化，则不更新
    }
    else {
        this->text_rr_wear.setLabelText(std::to_string(wear));
        this->last_data.RR_TireWear = wear; // 更新最后一次后右轮胎磨损数据
    }
}
void LVGL_SW_F1_Panel_1::setFLTemperature(int temperature)
{
    if (temperature == this->last_data.FL_TireTemperature) {
        return; // 如果前左轮胎温度没有变化，则不更新
    }
    else {
        this->text_fl_temp.setLabelText(std::to_string(temperature));
        this->last_data.FL_TireTemperature = temperature; // 更新最后一次前左轮胎温度数据
    }
}
void LVGL_SW_F1_Panel_1::setFRTemperature(int temperature)
{
    if (temperature == this->last_data.FR_TireTemperature) {
        return; // 如果前右轮胎温度没有变化，则不更新
    }
    else {
        this->text_fr_temp.setLabelText(std::to_string(temperature));
        this->last_data.FR_TireTemperature = temperature; // 更新最后一次前右轮胎温度数据
    }
}
void LVGL_SW_F1_Panel_1::setRLTemperature(int temperature)
{
    if (temperature == this->last_data.RL_TireTemperature) {
        return; // 如果后左轮胎温度没有变化，则不更新
    }
    else {
        this->text_rl_temp.setLabelText(std::to_string(temperature));
        this->last_data.RL_TireTemperature = temperature; // 更新最后一次后左轮胎温度数据
    }
}
void LVGL_SW_F1_Panel_1::setRRTemperature(int temperature)
{
    if (temperature == this->last_data.RR_TireTemperature) {
        return; // 如果后右轮胎温度没有变化，则不更新
    }
    else {
        this->text_rr_temp.setLabelText(std::to_string(temperature));
        this->last_data.RR_TireTemperature = temperature; // 更新最后一次后右轮胎温度数据
    }
}
void LVGL_SW_F1_Panel_1::setFLPressure(double pressure)
{
    if (fabs(pressure - this->last_data.FL_TirePressure) < EPSINON) {
        return; // 如果前左轮胎压力没有变化，则不更新
    }
    else {
        this->text_fl_pressure.setLabelText(to_string_with_precision(pressure));
        this->last_data.FL_TirePressure = pressure; // 更新最后一次前左轮胎压力数据
    }
}
void LVGL_SW_F1_Panel_1::setFRPressure(double pressure)
{
    if (fabs(pressure - this->last_data.FR_TirePressure) < EPSINON) {
        return; // 如果前右轮胎压力没有变化，则不更新
    }
    else {
        this->text_fr_pressure.setLabelText(to_string_with_precision(pressure));
        this->last_data.FR_TirePressure = pressure; // 更新最后一次前右轮胎压力数据
    }
}
void LVGL_SW_F1_Panel_1::setRLPressure(double pressure)
{
    if (fabs(pressure - this->last_data.RL_TirePressure) < EPSINON) {
        return; // 如果后左轮胎压力没有变化，则不更新
    }
    else {
        this->text_rl_pressure.setLabelText(to_string_with_precision(pressure));
        this->last_data.RL_TirePressure = pressure; // 更新最后一次后左轮胎压力数据
    }
}
void LVGL_SW_F1_Panel_1::setRRPressure(double pressure)
{
    if (fabs(pressure - this->last_data.RR_TirePressure) < EPSINON) {
        return; // 如果后右轮胎压力没有变化，则不更新
    }
    else {
        this->text_rr_pressure.setLabelText(to_string_with_precision(pressure));
        this->last_data.RR_TirePressure = pressure; // 更新最后一次后右轮胎压力数据
    }
}

void LVGL_SW_F1_Panel_1::setTireTypeNow(SW_DATA_TYPE::TireType tire_type)
{
    if (tire_type == this->last_data.TireType_Now) {
        return; // 如果当前轮胎类型没有变化，则不更新
    }

    // 设置当前轮胎类型显示对象的图片
    switch (tire_type)
    {
    case SW_DATA_TYPE::TireType::SOFT:
        this->Tire_Type_Now.setLabelText(std::string("S"));
        this->arc_tire_type_now.setArcOpa(LV_OPA_COVER);
        this->arc_tire_type_now.setArcColor(0xFFFFFF);
        this->Tire_Type_Now.setTextColor(0xFFFFFF);
        break;
    case SW_DATA_TYPE::TireType::MEDIUM:
        this->Tire_Type_Now.setLabelText(std::string("M"));
        this->arc_tire_type_now.setArcOpa(LV_OPA_COVER);
        this->arc_tire_type_now.setArcColor(0xF6D72C);
        this->Tire_Type_Now.setTextColor(0xFFFFFF);
        break;
    case SW_DATA_TYPE::TireType::HARD:
        this->Tire_Type_Now.setLabelText(std::string("H"));
        this->arc_tire_type_now.setArcOpa(LV_OPA_COVER);
        this->arc_tire_type_now.setArcColor(0xFFFFFF);
        this->Tire_Type_Now.setTextColor(0xFFFFFF);
        break;
    case SW_DATA_TYPE::TireType::INTER:
        this->Tire_Type_Now.setLabelText(std::string("I"));
        this->arc_tire_type_now.setArcOpa(LV_OPA_COVER);
        this->arc_tire_type_now.setArcColor(0x6DC253);
        this->Tire_Type_Now.setTextColor(0xFFFFFF);
        break;
    case SW_DATA_TYPE::TireType::WET:
        this->Tire_Type_Now.setLabelText(std::string("w"));
        this->arc_tire_type_now.setArcOpa(LV_OPA_COVER);
        this->arc_tire_type_now.setArcColor(0x0053FF);
        this->Tire_Type_Now.setTextColor(0xFFFFFF);
        break;
    case SW_DATA_TYPE::TireType::UNKNOWN:
        this->Tire_Type_Now.setLabelText(std::string(""));
        this->arc_tire_type_now.setArcOpa(LV_OPA_TRANSP);
    }
    this->last_data.TireType_Now = tire_type;
}

void LVGL_SW_F1_Panel_1::setTireTypeNext(SW_DATA_TYPE::TireType tire_type)
{
    if (tire_type == this->last_data.TireType_Next) {
        return; // 如果当前轮胎类型没有变化，则不更新
    }

    // 设置当前轮胎类型显示对象的图片
    switch (tire_type)
    {
    case SW_DATA_TYPE::TireType::SOFT:
        this->Tire_Type_Next.setLabelText(std::string("S"));
        this->arc_tire_type_next.setArcOpa(LV_OPA_COVER);
        this->arc_tire_type_next.setArcColor(0xFF0000);
        this->Tire_Type_Next.setTextColor(0xFFFFFF);
        break;
    case SW_DATA_TYPE::TireType::MEDIUM:
        this->Tire_Type_Next.setLabelText(std::string("M"));
        this->arc_tire_type_next.setArcOpa(LV_OPA_COVER);
        this->arc_tire_type_next.setArcColor(0xF6D72C);
        this->Tire_Type_Next.setTextColor(0xFFFFFF);
        break;
    case SW_DATA_TYPE::TireType::HARD:
        this->Tire_Type_Next.setLabelText(std::string("H"));
        this->arc_tire_type_next.setArcOpa(LV_OPA_COVER);
        this->arc_tire_type_next.setArcColor(0xFFFFFF);
        this->Tire_Type_Next.setTextColor(0xFFFFFF);
        break;
    case SW_DATA_TYPE::TireType::INTER:
        this->Tire_Type_Next.setLabelText(std::string("I"));
        this->arc_tire_type_next.setArcOpa(LV_OPA_COVER);
        this->arc_tire_type_next.setArcColor(0x6DC253);
        this->Tire_Type_Next.setTextColor(0xFFFFFF);
        break;
    case SW_DATA_TYPE::TireType::WET:
        this->Tire_Type_Next.setLabelText(std::string("w"));
        this->arc_tire_type_next.setArcOpa(LV_OPA_COVER);
        this->arc_tire_type_next.setArcColor(0x0053FF);
        this->Tire_Type_Next.setTextColor(0xFFFFFF);
        break;
    case SW_DATA_TYPE::TireType::UNKNOWN:
        this->Tire_Type_Next.setLabelText(std::string(""));
        this->arc_tire_type_next.setArcOpa(LV_OPA_TRANSP);
    }
    this->last_data.TireType_Next = tire_type;
}




void LVGL_SW_F1_Panel_1::setPannelColor(uint32_t color)
{
    this->box_laptime.setBoxColor(color);
    this->box_rpm.setBoxColor(color);
    this->box_diff.setBoxColor(color);
    this->box_speed.setBoxColor(color);
    this->box_laps.setBoxColor(color);
    this->box_pos.setBoxColor(color);
    this->box_soc.setBoxColor(color);
    this->box_lastandbeatlaptime.setBoxColor(color);
    this->box_fuel.setBoxColor(color);
}

void LVGL_SW_F1_Panel_1::setTextSpecialEvents(void)
{
    if (this->last_data.SpecialEventStatusCode != SW_DATA_TYPE::SpecialEvent::NONE) return; // 如果已经是特殊事件状态，则不需要再次设置
    // 设置特殊事件文本颜色和状态
    // this->last_data.SpecialEvents.status = true;
    this->text_laptime.setTextColor(LVGL_SW_F1_PANNEL_COLOR_WHITE);
    this->text_rpm.setTextColor(LVGL_SW_F1_PANNEL_COLOR_WHITE);
    this->text_diff.setTextColor(LVGL_SW_F1_PANNEL_COLOR_WHITE);
    this->text_speed.setTextColor(LVGL_SW_F1_PANNEL_COLOR_WHITE);
    this->text_laps_laps.setTextColor(LVGL_SW_F1_PANNEL_COLOR_WHITE);
    this->text_laps_total.setTextColor(LVGL_SW_F1_PANNEL_COLOR_WHITE);
    this->text_pos_pos.setTextColor(LVGL_SW_F1_PANNEL_COLOR_WHITE);
    this->text_pos_total.setTextColor(LVGL_SW_F1_PANNEL_COLOR_WHITE);
    this->text_gear.setTextColor(LVGL_SW_F1_PANNEL_COLOR_WHITE);
    this->text_fl_wear.setTextColor(LVGL_SW_F1_PANNEL_COLOR_WHITE);
    this->text_fr_wear.setTextColor(LVGL_SW_F1_PANNEL_COLOR_WHITE);
    this->text_rl_wear.setTextColor(LVGL_SW_F1_PANNEL_COLOR_WHITE);
    this->text_rr_wear.setTextColor(LVGL_SW_F1_PANNEL_COLOR_WHITE);
    this->text_fl_temp.setTextColor(LVGL_SW_F1_PANNEL_COLOR_WHITE);
    this->text_fr_temp.setTextColor(LVGL_SW_F1_PANNEL_COLOR_WHITE);
    this->text_rl_temp.setTextColor(LVGL_SW_F1_PANNEL_COLOR_WHITE);
    this->text_rr_temp.setTextColor(LVGL_SW_F1_PANNEL_COLOR_WHITE);
    this->text_fl_pressure.setTextColor(LVGL_SW_F1_PANNEL_COLOR_WHITE);
    this->text_fr_pressure.setTextColor(LVGL_SW_F1_PANNEL_COLOR_WHITE);
    this->text_rl_pressure.setTextColor(LVGL_SW_F1_PANNEL_COLOR_WHITE);
    this->text_rr_pressure.setTextColor(LVGL_SW_F1_PANNEL_COLOR_WHITE);
    this->text_soc.setTextColor(LVGL_SW_F1_PANNEL_COLOR_WHITE);
    // this->text_overtake.setTextColor(LVGL_SW_F1_PANNEL_COLOR_WHITE);
    this->text_lastlaptime.setTextColor(LVGL_SW_F1_PANNEL_COLOR_WHITE);
    this->text_bestlaptime.setTextColor(LVGL_SW_F1_PANNEL_COLOR_WHITE);
    this->text_deltafuel.setTextColor(LVGL_SW_F1_PANNEL_COLOR_WHITE);
    // this->text_diff_front.setLabelText(std::string("")); // 清空前方速度差文本
    this->text_diff_front.setTextColor(LVGL_SW_F1_PANNEL_COLOR_WHITE);
    // this->text_diff_after.setLabelText(std::string("")); // 清空后方速度差文本
    this->text_diff_after.setTextColor(LVGL_SW_F1_PANNEL_COLOR_WHITE);
    this->text_gear.setTextColor(LVGL_SW_F1_PANNEL_COLOR_WHITE);
}

// enum SpecialEvent {REDFLAG = 1, VSC = 2, SC, GREENFLAG, YELLOWFLAG, BLUEFLAG};

void LVGL_SW_F1_Panel_1::SpecialEventsDetect(void)
{
    if (this->current_data.SpecialEventStatusCode != this->last_data.SpecialEventStatusCode)
    {
        this->last_data.SpecialEventStatusCode = this->current_data.SpecialEventStatusCode;
        switch (this->last_data.SpecialEventStatusCode)
        {
            case SW_DATA_TYPE::SpecialEvent::NONE:
                LV_LOG_USER("None");
                break;
            case SW_DATA_TYPE::SpecialEvent::REDFLAG:
                /* code */
                LV_LOG_USER("Red Flag");
                break;
            case SW_DATA_TYPE::SpecialEvent::VSC:
                LV_LOG_USER("VSC");
                break;
            case SW_DATA_TYPE::SpecialEvent::SC:
                LV_LOG_USER("SC");
                break;
            case SW_DATA_TYPE::SpecialEvent::GREENFLAG:
                LV_LOG_USER("GREENFLAG");
                break;
            case SW_DATA_TYPE::SpecialEvent::YELLOWFLAG:
                LV_LOG_USER("YELLOWFLAG");
                break;
            case SW_DATA_TYPE::SpecialEvent::BLUEFLAG:
                LV_LOG_USER("BLUEFLAG");
                break;
            default:
                break;
        }
    }




}

void LVGL_SW_F1_Panel_1::UpdateAllData(void)
{
    this->setLaptime(this->current_data.Laptime);
    this->setLastLapTime(this->current_data.LastLapTime);
    this->setBestLapTime(this->current_data.BestLapTime);
    this->setDiff_self(this->current_data.Diff_self);
    this->setDiff_front(this->current_data.Diff_front);
    this->setDiff_after(this->current_data.Diff_after);
    this->setRPM(this->current_data.RPM);
    this->setSpeed(this->current_data.Speed);
    this->setGear(this->current_data.Gear);
    this->setLaps(this->current_data.Laps);
    this->setPos(this->current_data.Pos);
    this->setSOC(this->current_data.SOC);
    this->setDeltaFuel(this->current_data.DeltaFuel);
    this->setFLWear(this->current_data.FL_TireWear);
    this->setFRWear(this->current_data.FR_TireWear);
    this->setRLWear(this->current_data.RL_TireWear);
    this->setRRWear(this->current_data.RR_TireWear);
    this->setFLTemperature(this->current_data.FL_TireTemperature);
    this->setFRTemperature(this->current_data.FR_TireTemperature);
    this->setRLTemperature(this->current_data.RL_TireTemperature);
    this->setRRTemperature(this->current_data.RR_TireTemperature);
    this->setFLPressure(this->current_data.FL_TirePressure);
    this->setFRPressure(this->current_data.FR_TirePressure);
    this->setRLPressure(this->current_data.RL_TirePressure);
    this->setRRPressure(this->current_data.RR_TirePressure);

    this->setOvertakeStatus(this->current_data.Overtake); // 设置超车状态
    this->setDrsStatus(this->current_data.Drs); // 设置DRS状态
    this->setTireTypeNow(this->current_data.TireType_Now); // 模拟当前轮胎类型
    this->setTireTypeNext(this->current_data.TireType_Next); // 模拟下一个轮胎类型
    this->SpecialEventsDetect();
    last_data = current_data;

}

void LVGL_SW_F1_Panel_1::TimerUpdate(void)
{
    this->UpdateAllData(); // 更新所有数据
    LV_LOG_USER("Update Data Once.");
}

void LVGL_SW_F1_Panel_1::setRedFlagPanel(void)
{

    this->setPannelColor(LVGL_SW_F1_PANNEL_COLOR_RED); // 红色
    this->setTextSpecialEvents();
}
// void LVGL_SW_F1_Panel_1::setRedPannel()；


void LVGL_SW_F1_Panel_1::setVSCPanel(void)
{
    this->setPannelColor(LVGL_SW_F1_PANNEL_COLOR_YELLOW); // 黄色
    this->setTextSpecialEvents();
}
void LVGL_SW_F1_Panel_1::setSCPanel(void)
{
    this->setPannelColor(LVGL_SW_F1_PANNEL_COLOR_YELLOW); // 黄色
    this->setTextSpecialEvents();
}
void LVGL_SW_F1_Panel_1::setGreenFlagPanel(void)
{
    this->setPannelColor(LVGL_SW_F1_PANNEL_COLOR_GREEN); // 绿色
    this->setTextSpecialEvents();
}
void LVGL_SW_F1_Panel_1::setDefaultPanel(SW_DATA_TYPE::LVGL_SW_Pannel_Data data)
{
    //this->last_data.SpecialEventStatus = false;
    this->setPannelColor(LVGL_SW_F1_PANNEL_COLOR_BLACK); // 黑色
    this->text_bestlaptime.setTextColor(lv_color_hex(0xBF40BF)); // 设置最佳单圈时间文本颜色为紫色
    this->text_rpm.setTextColor(lv_color_hex(LVGL_SW_F1_PANNEL_COLOR_RED)); // 设置RPM文本颜色红色

    this->UpdateAllData();
}

void LVGL_SW_F1_Panel_1::setYellowFlagPanel(void)
{
    this->setPannelColor(LVGL_SW_F1_PANNEL_COLOR_YELLOW); // 黄色
    this->setTextSpecialEvents();
}

void LVGL_SW_F1_Panel_1::setBlueFlagPanel(void)
{
    this->setPannelColor(LVGL_SW_F1_PANNEL_COLOR_BLUE); // 蓝色
    this->setTextSpecialEvents();
}






void LVGL_SW_F1_Panel_1::setOvertakeStatus(SW_DATA_TYPE::OvertakeStatus status)
{
    if (this->last_data.Overtake != status)
    {
        if (this->last_data.Overtake ==  SW_DATA_TYPE::OvertakeStatus::OVERTAKE_DISAVAILABLE)
        {
            this->text_overtake.setLabelText(std::string("Overtake")); // 设置超车提示文本
        }
        switch (status)
        {
        case SW_DATA_TYPE::OvertakeStatus::OVERTAKE_DISAVAILABLE:
            this->text_overtake.setLabelText(std::string("")); // 清空超车提示文本
            this->box_overtake.setBoxColor(0x000000); // 黑色
            break;
        case SW_DATA_TYPE::OvertakeStatus::OVERTAKE_AVAILABLE: ;
        case SW_DATA_TYPE::OvertakeStatus::OVERTAKE_DISABLED:
            this->box_overtake.setBoxColor(0xFFFF3D); // 黄色
            this->text_overtake.setTextColor(lv_color_hex(0x808080)); // 设置超车提示文本颜色

            break;
        case SW_DATA_TYPE::OvertakeStatus::OVERTAKE_ENABLED:
            this->box_overtake.setBoxColor(0x00FF00); // 绿色
            this->text_overtake.setTextColor(lv_color_hex(0xFFFFFF)); // 设置超车提示文本颜色
            break;
        default:
            break;
        }

        this->last_data.Overtake = status; // 更新最后一次超车状态
    }
}

void LVGL_SW_F1_Panel_1::setDrsStatus(SW_DATA_TYPE::DrsStatus status)
{
    if (this->last_data.Drs != status)
    {
        if (this->last_data.Drs == SW_DATA_TYPE::DrsStatus::DRS_DISAVAILABLE)
        {
            this->text_drs.setLabelText(std::string("DRS")); // 设置DRS提示文本
            this->box_drs.setStyle(LVGL_SW_box_style);

        }
        switch (status)
        {
        case SW_DATA_TYPE::DrsStatus::DRS_DISAVAILABLE:
            this->box_drs.setBoxColor(0x000000);    // 设置DRS提示文本框颜色为黑色
            this->text_drs.setLabelText(std::string("")); // 清空DRS提示文本
            this->box_drs.setStyle(LVGL_SW_text_style);
            break;
        case SW_DATA_TYPE::DrsStatus::DRS_AVAILABLE:
            this->box_drs.setBoxColor(0x000000);    // 设置DRS提示文本框颜色为黑色
            this->text_drs.setTextColor(lv_color_hex(0xFFFFFF)); // 设置DRS提示文本颜色为白色
            break;
        case SW_DATA_TYPE::DrsStatus::DRS_DISABLED:
            this->box_drs.setBoxColor(0xFFFF3D); // 设置DRS提示文本框颜色为黄色
            this->text_drs.setTextColor(lv_color_hex(0x808080)); // 设置DRS提示文本颜色为灰色
            break;
        case SW_DATA_TYPE::DrsStatus::DRS_ENABLED:
            this->box_drs.setBoxColor(0x00FF00); // 设置DRS提示文本框颜色为绿色
            this->text_drs.setTextColor(lv_color_hex(0xFFFFFF)); // 设置DRS提示文本颜色为白色
            break;
        default:
            break;
        }

        this->last_data.Drs = status; // 更新最后一次DRS状态
    }
}


// std::string to_string_with_precision(double value, int precision) {
//     std::ostringstream oss;
//     oss << std::fixed << std::setprecision(precision) << value;
//     return oss.str();
// }

std::string ull_to_string(unsigned long long x) {
    if (x == 0) {
        return "0";
    }
    std::string s;
    while (x > 0) {
        s.push_back('0' + static_cast<char>(x % 10));
        x /= 10;
    }
    std::reverse(s.begin(), s.end());
    return s;
}
// static_cast<size_t>(precision)
std::string to_string_with_precision(double value, size_t precision) {
    // 处理NaN
    if (std::isnan(value)) {
        return "nan";
    }
    // 处理无穷大
    if (std::isinf(value)) {
        if (value > 0) {
            return "inf";
        } else {
            return "-inf";
        }
    }

    // 处理负值并记录符号
    bool is_negative = false;
    if (value < 0) {
        is_negative = true;
        value = -value;
    }

    // 计算10^precision因子
    double factor = 1.0;
    for (size_t i = 0; i < precision; ++i) {
        factor *= 10.0;
    }

    // 缩放并四舍五入
    double scaled = static_cast<double>(value) * factor + 0.5;
    unsigned long long x;
    if (scaled >= static_cast<double>(ULLONG_MAX)) {
        x = ULLONG_MAX; // 处理溢出
    } else {
        x = static_cast<unsigned long long>(scaled);
    }

    // 转换为字符串并插入小数点
    std::string s = ull_to_string(x);
    std::string result;

    if (precision == 0) {
        result = s; // 精度为0时直接返回整数部分
    } else {
        if (x == 0) {
            // 处理值为0的情况：生成"0.000...0"
            result = "0." + std::string(precision, '0');
        } else if (s.length() <= precision) {
            // 整数部分不足：添加前导零和小数点
            result = "0." + std::string(precision - s.length(), '0') + s;
        } else {
            // 插入小数点：整数部分.小数部分
            std::string int_part = s.substr(0, s.length() - precision);
            std::string frac_part = s.substr(s.length() - precision);
            result = int_part + "." + frac_part;
        }
    }

    // 添加负号（仅当结果为非零时）
    if (is_negative && x != 0) {
        result = "-" + result;
    }
    return result;
}