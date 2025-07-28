#ifndef _74HC165_H
#define _74HC165_H
#include "rtdevice.h"
#include "aic_hal_gpio.h"
#include <rtthread.h>

#define MAX_74HC165_CHIPS 5  // 定义最大支持的 74HC165 芯片数量

#define _74HC165_CLK_GPIO_PIN    rt_pin_get("PD.0")
#define _74HC165_SHLD_GPIO_PIN   rt_pin_get("PD.1")
#define _74HC165_SO_GPIO_PIN   rt_pin_get("PC.6")


#endif