/*
 * Copyright (c) 2024, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors: matteo <duanmt@artinchip.com>
 */

#include "aic_core.h"
#include "aic_hal_clk.h"

struct rt_i2c_bus_device *camera_i2c_get(void)
{
    struct rt_i2c_bus_device *i2c = NULL;
    char name[8] = "";

    snprintf(name, 8, "i2c%d", AIC_CAMERA_I2C_CHAN);
    i2c = rt_i2c_bus_device_find(name);
    if (i2c == RT_NULL) {
        LOG_E("Failed to open %s", name);
        return NULL;
    }

    return i2c;
}

void camera_xclk_enable(void)
{
    hal_clk_enable(CLK_OUT1);
}

void camera_xclk_disable(void)
{
    hal_clk_disable(CLK_OUT1);
}

u32 camera_xclk_rate_get(void)
{
#ifdef AIC_CLK_OUT1_FREQ
    return AIC_CLK_OUT1_FREQ;
#else
    return 0;
#endif
}

u32 camera_rst_pin_get(void)
{
    u32 pin = 0;

    pin = rt_pin_get(AIC_CAMERA_RST_PIN);
    if (pin)
        rt_pin_mode(pin, PIN_MODE_OUTPUT);
    else
        LOG_E("Failed to get reset PIN\n");

    return pin;
}

u32 camera_pwdn_pin_get(void)
{
    u32 pin = 0;

    pin = rt_pin_get(AIC_CAMERA_PWDN_PIN);
    if (pin)
        rt_pin_mode(pin, PIN_MODE_OUTPUT);
    else
        LOG_E("Failed to get power down PIN\n");

    return pin;
}
