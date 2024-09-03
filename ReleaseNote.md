
# V1.0.6 #
## 新增 ##
- 新增型号 D215，支持USB扩展屏功能
- LVGL：
  - 新增LVGL官方的music和benchmark demo
  - 新增电梯、咖啡机、蒸箱、86盒 和 USB扩展屏OSD 的demo
  - 支持从内存解码JPG/PNG图片到控件
- MPP：支持从DVP采集数据进行JPG编码，并存储到文件，录像文件支持MP4封装。
- PSRAM：支持封装型号D122BCV1、D122BCV2、TR230、JYX58、DR128、JYX68
- USB：支持WinUSB 1.0；支持UVC & UAC device；
- PM：支持设置唤醒源；支持light sleep mode
- SDMC：支持DDR模式
- WDT：增加寄存器的写保护
- 存储：支持NOR + NAND/NOR方案；NAND的rodata分区也增加坏块管理
- 启动：增加启动动画
- 安全：支持镜像文件中的SPL数据加密
- OneStep：新增 km/bm/ma/mu 命令
- 系统：增加waitqueue API
- 新增器件支持：
  - WiFi：ASR5505
  - 屏幕：axs15231b、nt35560、ST77922
  - DVP CVBS In：GM7150、TP2825/TB9950
  - Touch：cst3240、tw31xx、cst826、gsl3676、st77922
- 新增软件包：aic-authorization、ipmanager
- 新增第三方包：MicroPython
- 支持烤机测试
- test_dvp：支持使用GE进行旋转后再显示
- 增加示例：LwIP HTTP server test、test_gpio_key
- 增加方案配置：D215 demo88 nand/nor

## 优化 ##
- MPP：
  - Player：优化性能，包括解码流程、快速seek处理、内存管理流程等
  - Audio：优化重新播放时的延迟
- DE：优化Scaler效果
- Audio Codec：优化TLV320的播放和录音增益
- OneStep：简化BootLoader的编译方法（lunch清单中隐藏Bootloader配置，不再需要预编译bootloader.bin）
- 许可协议统一使用Apache-2.0
- 系统：
  - 优化memcpy的性能
  - 优化刷Cache的性能
  - package/artinchip中的软件包支持调整优先加载
- USB：
  - MSC支持热插拔，并优化传输的稳定性
  - 优化接入Hub时的处理，支持Hub级联
  - HID设备支持多个report ID
- Touch设备支持五点触摸
- 支持动态的注册模式
- CAP：提升频率的计算精度
- Touch：优化外设驱动的编译管理，简化用户的配置

## 修改 ##
- MPP：改进D21x的PNG解码兼容性
- USB：修正USB1、Hub的数据处理逻辑
- FatFS：分区时给文件夹预留足够的空间
- OTA：全平台打开OTA配置
- BootLoader：D13x中BootLoader默认运行在PSRAM中
- Eclipse：解决Freetype的编译问题
- 系统：
  - Panic时可以打印出完整的寄存器信息
  - 修正负数、浮点数的打印处理
  - 修正部分情况下的栈对齐问题
  - 完善get_tick接口的进位处理
- aicupg：
  - 当检测到SD卡/U盘拔出时重启系统
  - 改善和Win7系统连接的稳定性
- UART：为Rx IO配置上拉电阻
- test_uart：修正接收过长数据时的循环计数溢出问题；流控测试中支持设置波特率；修正线程退出流程
- test_psadc：从syscfg中读取电压参考值


# V1.0.5 #
## 新增 ##
- 调屏：支持和 AiPQ V1.1.1 工具配合使用
- 新增LVGL Demo：
- 支持VSCode 模拟器的工程导入
- 增加压力测试、独立控件、Gif、slide、lv_ffmpeg、DVP回显等参考实现
- MPP Player：支持avi封装格式
- USB Device：增加MTP支持
- USB Host：支持GPT分区格式
- PM：支持休眠时进入DDR自刷新
- aicupg：
  -支持和AiBurn工具配合完成Flash擦写功能
  -支持通过USB/UART获取设备侧的运行log
  -支持用命令进入U-Boot 升级模式
  -烧写时的进度条界面支持90/270度的旋转
- OTA：支持NFTL格式的Data分区升级；支持NOR、MMC介质
- TSen：增加对不同CP测试数据的兼容
- UART：增加流控功能支持；增加时钟分频的最佳匹配算法；增加软件模拟485的模式
- FS：支持生成 stripped FatFS 镜像，减小镜像文件大小
- DFS：SD0设备节点支持GPT分区格式
- GMAC：支持基于CHIP ID生成MAC地址；增加IPv6支持
- I2C：增加中断模式的处理流程；增加slave模式支持
- 新增SPI NAND：F35SQB004G、GD5F1GM7xUExxG、ZB35Q04A
- 新增屏幕：
  - RGB：gc9a01a
  - LVDS：LT8911
  -MIPI DSI：hx8394/ili9881c/st7703
  - i8080：st7789
- 新增CTP：st16xx、axs15260
- 新增第三方包：cJSON-1.7.16、mbedtls
- 新增aicp图片压缩库，压缩率更高
- 新增示例：test-twinkle、spi_flash、test_tsen_htp
- 新增工具支持：elf的size分析、编译时自动检测pinmux冲突、镜像文件大小匹配检查、version命令显示SoC型号
- OneStep：新增list-module命令，可查看当前方案中已打开的模块信息

## 优化 ##
- ADC：优化校准参数的算法
- Audio：优化关闭播放时的噪声；减少处理延迟
- DE：LVDS的双link可以单独配置参数
- PSRAM：优化初始化参数，提升稳定性
- USB：优化ADB写FatFS分区的性能
- SPI NAND：优化Page buf的内存拷贝流程；减少数据传输中的delay；
- SPI：优化DMA传输的结束状态判断
- SFUD：优化参数配置，提升工作频率
- D21x：优化DDR参数提升稳定性和兼容性
- aicupg：提升U盘烧录的兼容性

## 修改 ##
- MPP player：修正多个兼容性问题
- OTA：修正HTTP中的一处内存泄漏
- D12x：修改LDO1x电压为1.2V，稳定性更好
- D12x demo68：显示格式改为RGB565，和LVGL匹配
- aicupg：修正4K page size的处理
- GPAI：周期模式的默认周期改为 18ms
- UART：修正485模式的数据传输错误




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

