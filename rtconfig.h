#ifndef RT_CONFIG_H__
#define RT_CONFIG_H__

/* Automatically generated file; DO NOT EDIT. */
/* ArtInChip Luban-Lite SDK Configuration */

/* Project options */

#define PRJ_DEFCONFIG_FILENAME "d13x_demo88-nor_rt-thread_helloworld_defconfig"

/* -- Important: If following options have been changed, you need save & rerun menuconfig before changing any other options. */

#define PRJ_CHIP "d13x"
#define PRJ_BOARD "demo88-nor"
#define PRJ_KERNEL "rt-thread"
#define PRJ_APP "helloworld"
#define PLATFORM_LUBANLITE

/* Chip options */

#define SOC_THEAD_SMART
#define PRJ_CUSTOM_LDS ""
#define AIC_CHIP_D13X
#define CACHE_LINE_SIZE 32
#define CPU_BASE 0x20000000
#define AIC_CMU_DRV
#define AIC_CMU_DRV_V11
#define AIC_CMU_DRV_VER "11"
#define AIC_GPIO_DRV
#define AIC_GPIO_DRV_V11
#define AIC_GPIO_DRV_VER "11"
#define AIC_SYSCFG_DRV
#define AIC_SYSCFG_DRV_V11
#define AIC_SYSCFG_DRV_VER "11"
#define AIC_DMA_DRV
#define AIC_DMA_DRV_V11
#define AIC_DMA_DRV_VER "11"
#define AIC_DMA_CH_NUM 8
#define AIC_DMA_ALIGN_SIZE 8
#define AIC_UART_DRV
#define AIC_UART_DRV_V10
#define AIC_UART_DRV_VER "10"
#define AIC_UART_DEV_NUM 8
#define AIC_I2C_DRV
#define AIC_I2C_DRV_V10
#define AIC_I2C_DRV_VER "10"
#define AIC_I2C_CH_NUM 3
#define AIC_QSPI_DRV
#define AIC_QSPI_DRV_V11
#define AIC_QSPI_DRV_VER "11"
#define AIC_XSPI_DRV
#define AIC_XSPI_DRV_V10
#define AIC_WRI_DRV
#define AIC_WRI_DRV_V11
#define AIC_WRI_DRV_VER "11"
#define AIC_RTC_DRV
#define AIC_RTC_DRV_V11
#define AIC_RTC_DRV_VER "11"
#define AIC_WDT_DRV
#define AIC_WDT_DRV_V10
#define AIC_WDT_DRV_VER "10"
#define AIC_SDMC_DRV
#define AIC_SDMC_DRV_V11
#define AIC_SDMC_DRV_VER "11"
#define AIC_DE_DRV
#define AIC_DE_DRV_V11
#define AIC_DE_DRV_VER "11"
#define AIC_GE_DRV
#define AIC_GE_DRV_V11
#define AIC_GE_DRV_VER "11"
#define AIC_VE_DRV
#define AIC_VE_DRV_V30
#define AIC_VE_DRV_VER "30"
#define AIC_SID_DRV
#define AIC_SID_DRV_V11
#define AIC_SID_DRV_VER "11"
#define AIC_PM_DRV
#define AIC_PM_DRV_V11
#define AIC_PM_DRV_VER "11"

/* Board options */

/* Peripheral Devices: */

/* Interface Related: */

#define AIC_USING_UART0

/* Uart0 Parameter */

#define AIC_CLK_UART0_FREQ 48000000
#define AIC_DEV_UART0_BAUDRATE 115200
#define AIC_DEV_UART0_DATABITS 8
#define AIC_DEV_UART0_STOPBITS 1
#define AIC_DEV_UART0_PARITY 0
#define AIC_DEV_UART0_MODE_RS232
#define AIC_DEV_UART0_MODE 0
#define AIC_USING_I2C2

/* I2c2 Parameter */

#define AIC_DEV_I2C2_MASTER

/* Touch Panel Support */

/* Gt911 touch panel options */

#define AIC_TOUCH_PANEL_GT911
#define AIC_TOUCH_PANEL_GT911_I2C_CHA "i2c2"
#define AIC_TOUCH_PANEL_GT911_RST_PIN "PA.10"
#define AIC_TOUCH_PANEL_GT911_INT_PIN "PA.11"

/* Storage Related: */

#define AIC_USING_QSPI0

/* Qspi0 Parameter */

#define AIC_DEV_QSPI0_MAX_SRC_FREQ_HZ 100000000
#define AIC_QSPI0_DEVICE_SPINOR
#define AIC_QSPI0_DEVICE_SPINOR_FREQ 100000000
#define AIC_USING_SDMC1

/* SDMC1 Parameter */

#define AIC_SDMC1_DRV_PHASE 3
#define AIC_SDMC1_SMP_PHASE 0

/* Analog Related: */


/* MutiMedia Related: */

#define AIC_USING_DE

/* Display Parameter */

#define AIC_DISPLAY_DRV
#define AIC_DISP_DE_DRV
#define AIC_DISP_LVDS_DRV
#define AIC_DISP_LVDS
#define AIC_DI_TYPE 2

/* LVDS interface options */

#define AIC_LVDS_NS
#define AIC_LVDS_MODE 0
#define AIC_LVDS_LINK_0
#define AIC_LVDS_LINK_MODE 0
#define AICFB_RGB565
#define AICFB_FORMAT 0x0e
#define AIC_PAN_DISPLAY
#define AIC_DISP_COLOR_BLOCK
#define AICFB_ROTATE_0
#define AIC_FB_ROTATE_DEGREE 0

/* Display Panels */

#define AIC_SIMPLE_PANEL

/* display timing of simple panel */

#define PANEL_PIXELCLOCK 52
#define PANEL_HACTIVE 1024
#define PANEL_VACTIVE 600
#define PANEL_HBP 160
#define PANEL_HFP 160
#define PANEL_HSW 20
#define PANEL_VBP 12
#define PANEL_VFP 20
#define PANEL_VSW 2
#define AIC_PANEL_ENABLE_GPIO "PE.13"
#define AIC_USING_GE
#define AIC_GE_CMDQ

/* Graphics Engine Parameter */

#define AIC_GE_DITHER
#define AIC_GE_CMDQ_BUF_LENGTH 2048
#define AIC_USING_VE

/* Camera Support */


/* System Related: */

#define AIC_USING_DMA
#define AIC_USING_RTC

/* RTC Parameter */

#define AIC_RTC_CLK_RATE 3276800
#define AIC_USING_WDT

/* Mem Options */

#define AIC_SRAM_TOTAL_SIZE 0x100000
#define AIC_ITCM_SIZE 0x0
#define AIC_DTCM_SIZE 0x0
#define AIC_SRAM_SIZE 0x100000
#define AIC_INTERRUPTSTACK_SIZE 4096
#define AIC_SRAM_S1_SIZE_0K
#define AIC_SRAM_S1_SIZE 0
#define AIC_SRAM_S1_REG_SIZE 0x00
#define AIC_SRAM1_SW_SIZE 0x0
#define AIC_PSRAM_SIZE 0x800000
#define AIC_PSRAM_CMA_EN
#define AIC_PSRAM_SW_SIZE 0x0

/* Clocks options */

#define AIC_CLK_PLL_INT0_FREQ 480000000
#define AIC_CLK_PLL_INT1_FREQ 1200000000
#define AIC_CLK_PLL_FRA0_FREQ 792000000
#define AIC_CLK_PLL_FRA2_FREQ 1188000000
#define AIC_CLK_CPU_FREQ 480000000
#define AIC_CLK_AXI0_FREQ 200000000
#define AIC_CLK_AHB0_FREQ 200000000
#define AIC_CLK_APB0_FREQ 100000000
#define AIC_USING_PM

/* PM configure */

#define AIC_PM_POWER_KEY
#define AIC_PM_POWER_KEY_GPIO "PD.15"
#define AIC_PM_POWER_TOUCH_WAKEUP
#define AIC_PM_POWER_TOUCH_TIME_SLEEP 0
#define AIC_PM_POWER_DEFAULT_LIGHT_MODE

/* Security Related: */

#define AIC_USING_SID
#define AIC_USING_SYSCFG

/* Syscfg Parameter */

#define AIC_SYSCFG_LDO18_ENABLE
#define AIC_SYSCFG_LDO18_VOL_VAL 7

/* Application options */

/* Filesystem related */

#define AIC_USING_FS_IMAGE_0
#define AIC_USING_FS_IMAGE_TYPE_FATFS_FOR_0
#define AIC_FS_IMAGE_DIR_0 "application/os/helloworld/lvgl/meter_demo/lvgl_src/"
#define AIC_FS_IMAGE_NAME_0 "data.fatfs"
#define AIC_FATFS_AUTO_SIZE_FOR_0
#define AIC_USING_FS_IMAGE_1
#define AIC_USING_FS_IMAGE_TYPE_LITTLEFS_FOR_1
#define AIC_FS_IMAGE_DIR_1 "application/os/helloworld/lvgl/base_demo/data_src/"
#define AIC_FS_IMAGE_NAME_1 "data.lfs"
#define AIC_LITTLEFS_BLOCK_SIZE_FOR_1 4096
#define AIC_LITTLEFS_PAGE_SIZE_FOR_1 256
#define AIC_USING_FS_IMAGE_TYPE_FATFS_CLUSTER_SIZE 8

/* lvgl demo select related */

#define AIC_LVGL_DEMO
#define AIC_LVGL_METER_DEMO
#define LV_COLOR_DEPTH 16
#define CACHE_IMG_NUM 8

/* Rt-Thread options */

#define KERNEL_RTTHREAD
#define DRIVER_HAL_EN
#define DRIVER_DRV_EN

/* RT-Thread Kernel */

#define RT_NAME_MAX 16
#define RT_ALIGN_SIZE 4
#define RT_THREAD_PRIORITY_32
#define RT_THREAD_PRIORITY_MAX 32
#define RT_TICK_PER_SECOND 1000
#define RT_USING_OVERFLOW_CHECK
#define RT_USING_HOOK
#define RT_HOOK_USING_FUNC_PTR
#define RT_USING_IDLE_HOOK
#define RT_IDLE_HOOK_LIST_SIZE 4
#define IDLE_THREAD_STACK_SIZE 1024
#define RT_USING_TIMER_SOFT
#define RT_TIMER_THREAD_PRIO 4
#define RT_TIMER_THREAD_STACK_SIZE 512

/* kservice optimization */


/* Inter-Thread communication */

#define RT_USING_SEMAPHORE
#define RT_USING_MUTEX
#define RT_USING_EVENT
#define RT_USING_MAILBOX
#define RT_USING_MESSAGEQUEUE

/* Memory Management */

#define RT_USING_MEMHEAP
#define RT_MEMHEAP_FAST_MODE
#define RT_USING_MEMHEAP_AS_HEAP
#define RT_USING_HEAP

/* Kernel Device Object */

#define RT_USING_DEVICE
#define RT_USING_CONSOLE
#define RT_CONSOLEBUF_SIZE 128
#define RT_CONSOLE_DEVICE_NAME "uart0"
#define RT_VER_NUM 0x40101
#define ARCH_RISCV
#define ARCH_RISCV_FPU
#define ARCH_RISCV_FPU_D
#define ARCH_RISCV32

/* RT-Thread Components */

#define RT_USING_COMPONENTS_INIT
#define RT_USING_USER_MAIN
#define RT_MAIN_THREAD_STACK_SIZE 2048
#define RT_MAIN_THREAD_PRIORITY 10
#define RT_USING_MSH
#define RT_USING_FINSH
#define FINSH_USING_MSH
#define FINSH_THREAD_NAME "tshell"
#define FINSH_PROMPT_NAME "aic"
#define FINSH_THREAD_PRIORITY 20
#define FINSH_THREAD_STACK_SIZE 4096
#define FINSH_USING_HISTORY
#define FINSH_HISTORY_LINES 1
#define FINSH_USING_SYMTAB
#define FINSH_CMD_SIZE 80
#define MSH_USING_BUILT_IN_COMMANDS
#define FINSH_USING_DESCRIPTION
#define FINSH_ARG_MAX 10
#define RT_USING_DFS
#define DFS_USING_POSIX
#define DFS_USING_WORKDIR
#define DFS_FILESYSTEMS_MAX 4
#define DFS_FILESYSTEM_TYPES_MAX 4
#define DFS_FD_MAX 16
#define RT_USING_DFS_MNTTABLE
#define RT_USING_DFS_ELMFAT

/* elm-chan's FatFs, Generic FAT Filesystem Module */

#define RT_DFS_ELM_CODE_PAGE 437
#define RT_DFS_ELM_WORD_ACCESS
#define RT_DFS_ELM_USE_LFN_3
#define RT_DFS_ELM_USE_LFN 3
#define RT_DFS_ELM_LFN_UNICODE_0
#define RT_DFS_ELM_LFN_UNICODE 0
#define RT_DFS_ELM_MAX_LFN 255
#define RT_DFS_ELM_DRIVES 2
#define RT_DFS_ELM_MAX_SECTOR_SIZE 4096
#define RT_DFS_ELM_REENTRANT
#define RT_DFS_ELM_MUTEX_TIMEOUT 3000
#define RT_USING_DFS_ROMFS
#define RT_USING_FAL
#define FAL_DEBUG 0
#define FAL_PART_HAS_TABLE_CFG
#define FAL_USING_SFUD_PORT
#define FAL_USING_NOR_FLASH_DEV_NAME "norflash0"
#define FAL_BLK_DEVICE_RDONLY

/* Device Drivers */

#define RT_USING_DEVICE_IPC
#define RT_USING_SERIAL
#define RT_USING_SERIAL_V1
#define RT_SERIAL_USING_DMA
#define RT_SERIAL_RB_BUFSZ 64
#define RT_USING_I2C
#define RT_USING_I2C_BITOPS
#define RT_USING_PIN
#define RT_USING_ADC
#define RT_USING_PWM
#define RT_USING_MTD_NOR
#define RT_USING_PM
#define PM_TICKLESS_THRESHOLD_TIME 2
#define PM_USING_CUSTOM_CONFIG
#define PM_ENABLE_SUSPEND_SLEEP_MODE
#define PM_ENABLE_THRESHOLD_SLEEP_MODE
#define PM_LIGHT_THRESHOLD_TIME 5
#define PM_DEEP_THRESHOLD_TIME 20
#define PM_STANDBY_THRESHOLD_TIME 100
#define RT_USING_RTC
#define RT_USING_ALARM
#define RT_USING_SDIO
#define RT_SDIO_STACK_SIZE 512
#define RT_SDIO_THREAD_PRIORITY 15
#define RT_MMCSD_STACK_SIZE 8192
#define RT_MMCSD_THREAD_PREORITY 22
#define RT_MMCSD_MAX_PARTITION 16
#define RT_USING_SPI
#define RT_USING_QSPI
#define RT_USING_SFUD
#define RT_SFUD_USING_SFDP
#define RT_SFUD_USING_FLASH_INFO_TABLE
#define RT_SFUD_USING_QSPI
#define RT_SFUD_SPI_MAX_HZ 50000000
#define RT_USING_WDT
#define RT_USING_SENSOR
#define RT_USING_SENSOR_CMD
#define RT_USING_TOUCH
#define RT_TOUCH_PIN_IRQ

/* Using USB */


/* C/C++ and POSIX layer */

#define RT_LIBC_DEFAULT_TIMEZONE 8

/* POSIX (Portable Operating System Interface) layer */

#define RT_USING_POSIX_DELAY
#define RT_USING_POSIX_CLOCK

/* Interprocess Communication (IPC) */


/* Socket is in the 'Network' category */


/* Network */


/* Utilities */

#define RT_USING_ULOG
#define ULOG_OUTPUT_LVL_I
#define ULOG_OUTPUT_LVL 6
#define ULOG_ASSERT_ENABLE
#define ULOG_LINE_BUF_SIZE 128

/* log format */

#define ULOG_USING_COLOR
#define ULOG_OUTPUT_TIME
#define ULOG_TIME_USING_TIMESTAMP
#define ULOG_OUTPUT_LEVEL
#define ULOG_OUTPUT_TAG
#define ULOG_BACKEND_USING_CONSOLE
#define ULOG_USING_FILTER

/* RT-Thread Utestcases */


/* RT-Thread online packages */

/* IoT - internet of things */


/* Wi-Fi */

/* Marvell WiFi */


/* Wiced WiFi */


/* IoT Cloud */


/* security packages */


/* language packages */

/* JSON: JavaScript Object Notation, a lightweight data-interchange format */


/* XML: Extensible Markup Language */


/* multimedia packages */

/* u8g2: a monochrome graphic library */


/* PainterEngine: A cross-platform graphics application framework written in C language */


/* tools packages */


/* system packages */

/* enhanced kernel services */


/* acceleration: Assembly language or algorithmic acceleration packages */


/* CMSIS: ARM Cortex-M Microcontroller Software Interface Standard */


/* Micrium: Micrium software products porting for RT-Thread */


/* peripheral libraries and drivers */


/* Kendryte SDK */


/* AI packages */


/* miscellaneous packages */

/* project laboratory */

/* samples: kernel and components samples */


/* entertainment: terminal games and other interesting software packages */


/* Arduino libraries */


/* Projects */


/* Sensors */


/* Display */


/* Timing */


/* Data Processing */


/* Data Storage */

/* Communication */

/* Device Control */


/* Other */

/* Signal IO */


/* Uncategorized */

/* Local packages options */

/* Third-party packages options */

#define LPKG_USING_CPU_USAGE
#define LPKG_USING_LVGL
#define LPKG_LVGL_PATH "/packages/multimedia/LVGL/LVGL"
#define LPKG_LVGL_THREAD_PRIO 20
#define LPKG_LVGL_THREAD_STACK_SIZE 4096
#define LPKG_LVGL_DISP_REFR_PERIOD 5
#define LPKG_USING_LITTLEFS
#define LPKG_USING_LITTLEFS_LATEST_VERSION
#define LFS_READ_SIZE 256
#define LFS_PROG_SIZE 256
#define LFS_BLOCK_SIZE 4096
#define LFS_CACHE_SIZE 256
#define LFS_BLOCK_CYCLES -1
#define LFS_THREADSAFE
#define LFS_LOOKAHEAD_MAX 128

/* BenchMark Tests */

#define AIC_PRINT_FLOAT_CUSTOM

/* ArtInChip packages options */

#define LPKG_MPP

/* Drivers options */

#define AIC_GPIO_IRQ_DRV_EN
#define AIC_SPINOR_DRV

/* Peripheral */

#define LPKG_USING_SFUD
#define AIC_WIRELESS_LAN
#define WLAN_REALTEK

/* Touch Panel Support */

/* Gt911 touch panel options */

/* External Audio Codec Support */


/* Drivers debug */


/* Drivers examples */


#endif
