# 简介
本项目（Steering Wheel 374, sw374）的短期目标是实现一款基于速魔基座的F1方向盘，长期目标是实现以“仪表主控模块”为核心的模块化的多功能方向盘。项目的最终目标是实现一款经济、可靠、模块化、多功能的模拟赛车方向盘。项目规划以模拟赛车仪表(SW374-dashboard)为核心模块，实现屏幕驱动、遥测数据接收等功能；方向盘功能由设计的外接拓展板，实现按键等功能。
目前主要进行了核心模块——Dashboard的开发，主要电子元件包括：主控衡山派、4.3寸LCD裸屏、2912 LEDs灯珠。
主控模块选择了开源的立创开发板——衡山派，主控芯片是匠芯创的D133EBS。在前期选型时根据速魔上市方向盘的IO规划，确定开发平台和所需要的硬件IO，确认项目需要包含显示屏、LED指示灯、按键、编码器等。
屏幕为800×480分辨率的4.3寸屏幕，目前选择的是没有触摸的版本，未来也大概率不会采用触摸方法。一是因为触摸的交互开发复杂，个人暂时没有精力投入到其中，另一个是需要触摸来进行的操作大多可以用其他操作方法有效的替代，没有不得不开发触摸的理由。同尺寸的屏幕有480×272的分辨率，成本更低，立创文档也是以这个分辨率的屏幕为示例，未来可能拓展支持到该屏幕，但短期内没有相关的计划。
LED灯选择的是WS2812B 5050规格，原型开发时候通过打样的电路板实现，但电路板打样贴片麻烦，门槛较高，因此改用了2812的裸板灯带，设计时使用的灯带宽度为12mm、144颗灯珠/米的灯带，但由于灯带太多太密，烙铁焊接时有一定的难度，目前还没有想到更为经济、方便的解决方式。
方向盘规划是通过衡山派40Pin排针连接拓展板，将其他IO外设接入拓展板中，目前已打样了拓展板，准备进行下一步的开发测试。
# 目前实现的软件功能
- 基于LVGL9的屏幕驱动
- WS2812B LEDs 控制
- Wifi连接
- TCPserver获取Simhub发出的遥测数据

# 项目资料获取方式
3D打印模型：Makerworld模拟赛车仪表[makerworld](https://makerworld.com.cn/zh/models/1418063-mo-ni-sai-che-yi-biao-steering-wheel-374#profileId-1539763)
软件开源地址：[Github](https://github.com/Sanches374/sw374-software)
硬件开源地址：（注：目前项目未涉及硬件，后续有硬件需求时在此更新）
交流QQ群号：1054679180

# 感谢列表
[eCrowneEng/ESP-SimHub](https://github.com/eCrowneEng/ESP-SimHub/tree/main/src)提供的通过Wifi网络连接获取Simhub数据的思路。
[Nolimy/steeringWheel_MeterBox_STM32_FreeRTOS](https://github.com/Nolimy/steeringWheel_MeterBox_STM32_FreeRTOS/blob/main/README.md)为本项目的仪表实现方式选择及硬件平台选择提供了参考
[speedim](https://speedim.apexracing.cn/simhub-setup/simhub-setup.html)为Simhub自定义串口的配置文件提供了参考。