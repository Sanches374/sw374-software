# AWTK 简要说明

本项目的编译工具是scons，依托AIC SDK进行编译。配置AWTK 参数需要在AIC SDK 根目录使用命令

**scons--menuconfig** 进入可视化界面进行配置。具体使用可以参考AIC docs

## 配置说明

配置所在路径：在AIC SDK 根目录使用命令scons--menuconfig 进入可视化界面，选择AWTK GUI

```sh
Local packages options -->
    Third-party packages options-->
    [*] AWTK GUI -->
            AWTK common function config -->
            AWTK cropping config -->
            AWTK Select a demo -->
            AWTK thread config -->
            AWTK input device config -->

```
配置可以分为三块：
-   AWTK 功能启用：这块是最常用的配置
-   AWTK 裁剪配置：这块裁剪掉不需要的功能
-   Demo 选择：目前提供两个demo，两个都是官方的demo
-   线程配置：主要关注线程栈的大小就行，根据自己的应用调整线程栈大小避免溢出
-   输入设配配置：目前仅支持GT911 设备触摸设备输入
---

## 资源配置

`注意：在配置使用了文件系统，需要配置安装资源路径和读取资源路径，这样应用才能正常启动`

安装资源路径：将资源文件直接下载到板子，而不是从SD卡读取资源
```sh
Application options -->
    [*] Using File System Image 0 -->
        (packages/third-party/awtk/user_apps/awtk-demo-chart/rtos_res/)
```

读取资源路径：AWTK读取资源文件的路径，这个必须配置
```sh
Local packages options -->
    Third-party packages options-->
    [*] AWTK GUI -->
            AWTK common function config -->
                (/rodata/res) Set app resource root

```

## 内存配置

在D13x 平台上共有6M 内存可供用户使用。如果实在分配不出更多的内存，可以参考 "./user_apps/awtk-demo-chart/src/application.c" 中设置图片缓冲大小函数 `image_manager_set_max_mem_size_of_cached_images(image_manager(), 1366430)` 和  。图片缓冲区大小对速度的影响很大，需要根据需要评估需要设置多大的缓冲区。如果缓冲区设置过小，便会频繁的读取，释放资源，速度将受到**极大**的影响。

在D21x 平台上面，可以去配置使用的内存：
配置CMA 内存
```sh
Board options -->
    Mem Options  --->
    (0x2000000) CMA mem size
```

特别需要注意的是：在AIC D13x 平台上，内存资源受限比较紧张，默认不适用窗口动画缓存。如果是D21 x平台，内存充足，可以选择打开。

```sh
Local packages options -->
    Third-party packages options-->
    [*] AWTK GUI -->
            AWTK cropping config -->
                （*）Don't use window animation cache and default open don't use dialog hightlighter

```

除了配置安装资源路径和读取资源路径。一般而言，使用`默认配置`即可，不需要修改里面的配置。如果需要修改，相关配置说明可以参考，awtk/docs/porting_common.md 和官方文档

---
### 额外配置说明

#### 开机自启动

如果需要开机自启动，需要选择 ”Self start funcion upon startup“。如果不需要开机自启动，那需要在命令行，输入命令 test_awtk，手动执行应用。如果需要修改执行命令，参考 "awtk-rtos/awtk-port/rk_run.c" 文件

```sh
Local packages options -->
    Third-party packages options-->
    [*] AWTK GUI -->
            AWTK common function config -->
                [*] Self start funcion upon startup

```

#### 硬件图片解码格式支持

注意：在D21x 平台上，PNG格式不支持硬件解码为RGB565格式，JPEG格式仅支持解码为YUV格式。在D13x 平台上PNG格式不支持硬件解码为RGB565格式。如果没有关闭动画功能，由于动画功能格式不支持RGB888，
将会对原图格式为RGB888的图片进行一次转换，转换为RGB565格式。

D21x 平台：
| FRAME BUFFER FORMAT | RGB565  | RGB888/ARGB8888 |
| --------------------| --------| ----------------|
|        PNG          |  不支持  |       支持      |
|       JEPG/JPG      |  不支持  |      不支持     |

D13x 平台：
| FRAME BUFFER FORMAT | RGB565  | RGB888/ARGB8888 |
| --------------------| --------| ----------------|
|        PNG          |  不支持  |       支持      |
|       JEPG/JPG      |   支持   |        支持     |

关于格式描述方式：AWTK 的格式表述和AIC 格式描述的倒转过来的，比如：AIC 格式为RGB888 对应 AWTK 格式 BGR888

#### demo 选择

目前支持的三个demo，一个是 demo chart 一个是official demo、一个是demo basic。如果选择official demo 需要把文件系统配置关掉，以便演示AWTK 不使用文件系统的运行情况。demo basic 也部分依赖demo chart的资源，运行demo basic 需要把demo chart的资源加进来。

```sh
Local packages options -->
    Third-party packages options-->
    [*] AWTK GUI -->
            AWTK cropping config -->
                [] Use files system

```

demo chart：使用到ARGB8888 的图片，如果lcd 格式不是ARGB 8888就会丢失图片透明度，显示就不正常。
official demo：这个demo 所需的资源比较大，只能在D21x 平台上运行

## 编译说明

编译用到的宏定义一部分定义在 ./SConscript中，一部分在Kconfig 在可视化界面中进行配置。配置由相关脚本进行解析，得到全局的编译的宏。

相关宏的说明可以参考：awtk/docs/porting_common.md
参与编译的文件可以参考：awtk/docs/porting_common.md 和 ./SConscript

如果需要编译自己的 app，需要修改SConsript。AWTK Designed 生成的资源文件也需要做出一些调整，将不需要的文件可以删掉，只留下需要的，参考 rtos/res路径下的资源文件。参考代码：

```py
# GetDepend 是从Kconfig获取相关宏
if GetDepend('LPKG_AWTK_USING_DEMOS_CHART'):
    awtk_demo_cwd = cwd + '/user_apps/'
    # add user 3rd
    src += find_sources(awtk_demo_cwd + 'awtk-demo-chart/3rd/awtk-widget-chart-view/src/')
    src += find_sources(awtk_demo_cwd + 'awtk-demo-chart/3rd/awtk-widget-chart-view/src/base/')
    src += find_sources(awtk_demo_cwd + 'awtk-demo-chart/3rd/awtk-widget-chart-view/src/chart_view/')
    src += find_sources(awtk_demo_cwd + 'awtk-demo-chart/3rd/awtk-widget-chart-view/src/pie_slice/')

    # add user src
    src += find_sources(awtk_demo_cwd + 'awtk-demo-chart/src/')
    src += find_sources(awtk_demo_cwd + 'awtk-demo-chart/src/pages/')
    src += find_sources(awtk_demo_cwd + 'awtk-demo-chart/src/common/')

    # install needed res
    if GetDepend('WITH_FS_RES'):
        Mkdir(awtk_demo_cwd + 'awtk-demo-chart/rtos_res/res/assets/default/')
        Install(awtk_demo_cwd + 'awtk-demo-chart/rtos_res/res/assets/default/', awtk_demo_cwd + 'awtk-demo-chart/res/assets/default/raw')
```

## 对接流程

除了图片解码的流程和AWTK 标准流程不一样，其他流程都是按照AWTK 的标准流程进行对接的。图片解码流程做了一些调整，因为硬件解码需要CMA 内存，内存如果不够，系统便会在解码函数处崩溃。关于CMA 内存也可以理解为资源的一部分，资源分配失败就返回。

**AIC 图片硬件解码流程**：

获取资源 + 硬件解码（资源获取函数） -> BITMAP 格式转化（解码函数） -> 加入管理器由AWTK 进行管理（加入管理器函数）

**AWTK 图片软件解码流程**：

获取资源（资源获取函数） -> STB 软件解码 + BITMAP 格式转化（解码函数） -> 加入管理器进行管理（加入管理器函数）

AIC 硬件解码和AWTK 软件解码的区别就是，在获取资源函数的时候就进行硬件解码。如果解码失败，则交由软件再次进行资源获取和解码。
