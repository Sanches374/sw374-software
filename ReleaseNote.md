# V1.0.4 #
## 新增 ##
- MPP：支持WAV文件解析；支持超短音频的播放
- Display：支持AiPQ调屏工具
- USB：支持MIDI、USB Audio、Device MSC、CDC
- IRQ：D12x、D13x支持中断嵌套、超级中断机制
- UART：支持 485 软件模式；支持DMA方式；支持流控功能
- LwIP：增加网络配置工具；增加PTPD协议支持；支持DHCP server；增加回环测试命令
- 新增PSADC、QEP驱动
- 烧写过程增加图形的进度条界面，进度条效果可配置
- aicupg：支持通过SD卡升级指定分区
- PBP：支持关闭PBP启动过程中的log
- 支持使用PWM实现hrtimer
- D12x支持PM流程
- 增加命令"scons --list-mem"，方便查看当前方案的内容布局
- menuconfig中增加可配置各个section的位置
- OneStep：Windows环境支持mb、mc命令；
- LVGL新增dashboard demo
- 移植Luban中的userid库
- 新增方案：d12x-hmi
- 新增NAND器件：XCSP1AAPK-IT
- 新增第三方包：libmodbus、sqlite、gif、
- 新增示例：test-multipwm、test-qep、test-keyadc
## 优化 ##
- 优化MP4格式的兼容性
- 完善一些特殊包的PPS、Slice处理
- 修正CPU loading的计算
- 优化stop时的资源释放处理
- Kconfig.board：简化依赖关系，每颗SoC只需要维护一份
- Eclipse：支持menuconfig方式进行动态配置SDK
- DMA：优化写内存后的同步处理
- RTP：优化event的上报流程，响应更快；保存校准文件到/data/config目录
- USB：优化UDC的枚举流程，提升兼容性
- SPI：优化多任务并发时的等待机制
- DVP：优化中断的处理流程
- LVGL增加GE透明功能的宏控制
- LVGL旋转支持逆时针、顺时针
- LVGL完善旋转后的搬移处理
- LVGL适配RTP触屏
- 工具链：精简其中的库文件，减小体积
- OneStep：完善Windows环境的路径切换处理
## 修改 ##
- I2C：支持发送restart信号
- CAN：修正工作模式的设置流程
- USB：修正ADB的printf()异常问题；改善ADB连接的稳定性
- GPAI：修正校准参数和计算过程
- NFTL：修正一处内存泄漏
- WiFi：修正rtl8189初始化过程中的异常



# V1.0.3 #
## 新增 ##
- 新增支持FreeRTOS内核
- 新增D12x
- NAND方案：全平台都已支持NFTL
- 支持OTA升级方案
- 新增支持ADB
- 新增支持HID Device，优化Custom IO Demo可接收图片、视频文件
- MPP：D13x、D12x支持MJPEG的解码
- PWM：支持输出指定个数的脉冲信号
- 新增屏支持：MIPI ili9488、LCD st7701s
- 新增WiFi支持：rtl8733、rtl8189
- 新增ESMT等厂家的多款NAND支持
- 新增CTP支持：ft7411、gsl1680，并增加相应的测试示例
- 新增Codec支持：tlv320
- LVGL Demo：支持动态旋转、缩放、任意角度旋转、多国语言、GIF图片
- 新增示例：test_fb、draw_line、test_i2s_loopback
## 优化 ##
- D12x: 仪表盘Demo优化达到58帧/s
- UART烧写：优化速率（最高可达3Mbps）和稳定性
- D21x：功耗优化
- 增强刷Cache时的对齐检查
- 优化调度入口的处理流程
- FATFS：支持sparse格式
- 优化PSRAM的稳定性；将PSRAM初始化统一放在PBP中
- 支持USB3.0的U盘
- 优化Device驱动的Buf性能
## 修改 ##
- CAN：修正HDR参数
- RTP：修正UP事件丢失的问题
- USB：修正U盘压力测试中出错的问题；
- RTP：将校准数据保存到文件中
- 默认关闭PM（功耗管理）功能
- 修改 application/os 目录名称为 application/rt-thread
- cherryUSB升级为v1.0.0版本
- gt911：修正多点触摸时的异常问题
- I2C：修正收发长报文的异常问题
- zlib解压缩时使用轮询方式
- 完善对B帧数据的处理
- 优化单曲循环播放时的切换机制
- Audio：优化播放流程，改善播放数据的完整性；设置最大音量为0db
- PWM：优化默认值、完善shadow寄存器的流程

# V1.0.2 #
## 新增 ##
- 支持动态APP加载
- 电源管理：支持休眠唤醒流程，新增light sleep模式
- 启动：NOR支持XIP模式、支持eMMC启动和烧写
- UI新增支持AWTK
- NAND：支持NTFL、FatFS
- SD卡：支持热插拔
- Audio：支持暂停功能
- Network：新增ping命令、支持MQTT协议
- USB：支持cherryUSB V0.10.0，USB Host和Device功能可用
- SPI：支持Slave模式（仅提供说明给gx客户）、Bit模式
- MPP：支持JPeg video
- Display: 支持SRGB、Gamma调节、PWM背光
- RTP：支持五点校准算法
- TSen：支持温度校准
- GPAI：支持自动校准
- HRTimer：适配D13x
- 支持UART烧写
- 支持OTA升级
- 支持ENV分区
- OneStep新增命令：add-board、rm-board、mc（clean + make）、
- 新增示例：test-tsen、test-gpai、test-rtp、test_gpio、test_i2c、test_ce、test_pm、test_rtc、ge_test
- 新增board：D13x demo88 NOR/NAND、D12x demo68 NOR/NAND



# V1.0.1 #
## 新增 ##
- 添加awtk支持
- 添加OTA功能

# V1.0.0 #
## 新增 ##
- 初始稳定版
- 支持D21X,D13X

