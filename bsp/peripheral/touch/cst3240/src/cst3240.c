/*
 * Copyright (c) 2024, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date              Notes
 * 2024-06-11        the first version
 */
#include <rtthread.h>
#include <rtdevice.h>
#include "touch.h"
#include <rtdbg.h>
#include "cst3240.h"

#define DBG_TAG "cst3240"
#define DBG_LVL DBG_LOG

static struct rt_i2c_client cst3240_client;

static rt_err_t cst3240_write_reg(struct rt_i2c_client *dev, rt_uint8_t *data,
                                rt_uint8_t len)
{
    struct rt_i2c_msg msgs;

    msgs.addr = dev->client_addr;
    msgs.flags = RT_I2C_WR;
    msgs.buf = data;
    msgs.len = len;

    if (rt_i2c_transfer(dev->bus, &msgs, 1) == 1) {
        return RT_EOK;
    } else {
        return -RT_ERROR;
    }
}

static rt_err_t cst3240_read_regs(struct rt_i2c_client *dev, rt_uint8_t *reg,
                                rt_uint8_t *data, rt_uint8_t len)
{
    struct rt_i2c_msg msgs[2];

    msgs[0].addr = dev->client_addr;
    msgs[0].flags = RT_I2C_WR;
    msgs[0].buf = reg;
    msgs[0].len = 2;

    msgs[1].addr = dev->client_addr;
    msgs[1].flags = RT_I2C_RD;
    msgs[1].buf = data;
    msgs[1].len = len;

    if (rt_i2c_transfer(dev->bus, msgs, 2) == 2) {
        return RT_EOK;
    } else {
        return -RT_ERROR;
    }
}

static rt_size_t cst3240_readpoint(struct rt_touch_device *touch, void *data, rt_size_t touch_num)
{
    rt_uint8_t buf[6];
    rt_uint8_t reg[2];
    rt_uint8_t cmd[3];
    struct rt_touch_data *temp_data;
    rt_uint8_t event = RT_TOUCH_EVENT_NONE;
    rt_uint8_t point_num = 0;
    static rt_uint16_t judge = 0;

    temp_data = (struct rt_touch_data *)data;
    temp_data->event = RT_TOUCH_EVENT_NONE;

    reg[0] = (rt_uint8_t)((CST3240_POINT1_REG >> 8) & 0xFF);
    reg[1] = (rt_uint8_t)(CST3240_POINT1_REG & 0xFF);

    cst3240_read_regs(&cst3240_client, reg, buf, 6);

    point_num = buf[5] & 0xF;
    if (!point_num) {
        return 0;
    } else if (point_num >= 1) {
        point_num = 1;
    }
    event = (buf[0] & 0xF) == 0x6 ? 1 : 0;      //0x6:down

    if (event)
        judge++;

    if (event && judge == 1) {
        temp_data->event = RT_TOUCH_EVENT_DOWN;
    } else if (event && judge > 1) {
        temp_data->event = RT_TOUCH_EVENT_MOVE;
    } else if (!event) {
        temp_data->event = RT_TOUCH_EVENT_UP;
        judge = 0;
    } else {
        temp_data->event = RT_TOUCH_EVENT_NONE;
    }

    temp_data->x_coordinate = ((buf[3] >> 4) & 0xF) | (buf[1] << 4);
    temp_data->y_coordinate = (buf[3] & 0x0F) | (buf[2] << 4);

    cmd[0] = (rt_uint8_t)(CST3240_FIXED_REG >> 8);
    cmd[1] = (rt_uint8_t)(CST3240_FIXED_REG & 0xFF);
    cmd[2] = 0xAB;
    cst3240_write_reg(&cst3240_client, cmd, 3);

    return point_num;
}

static rt_err_t cst3240_control(struct rt_touch_device *touch, int cmd, void *arg)
{
    struct rt_touch_info *info;

    switch(cmd)
    {
    case RT_TOUCH_CTRL_GET_ID:
        break;
    case RT_TOUCH_CTRL_GET_INFO:
        info = (struct rt_touch_info *)arg;
        info->point_num = 1;
        info->range_x = touch->info.range_x;
        info->range_y = touch->info.range_y;
        info->type = touch->info.type;
        info->vendor = touch->info.vendor;

        break;
    case RT_TOUCH_CTRL_SET_MODE:
        break;
    case RT_TOUCH_CTRL_SET_X_RANGE:
        break;
    case RT_TOUCH_CTRL_SET_Y_RANGE:
        break;
    case RT_TOUCH_CTRL_SET_X_TO_Y:
        break;
    case RT_TOUCH_CTRL_DISABLE_INT:
        break;
    case RT_TOUCH_CTRL_ENABLE_INT:
        break;
    default:
        break;
    }

    return RT_EOK;
}

const struct rt_touch_ops cst3240_touch_ops =
{
    .touch_readpoint = cst3240_readpoint,
    .touch_control = cst3240_control,
};

int rt_hw_cst3240_init(const char *name, struct rt_touch_config *cfg)
{
    rt_touch_t touch_device = RT_NULL;

    touch_device = (rt_touch_t)rt_calloc(1, sizeof(struct rt_touch_device));
    if (touch_device == RT_NULL) {
        LOG_E("touch device malloc fail");
        return -RT_ENOMEM;
    }
    // rst output 1
    rt_pin_mode(*(rt_uint8_t *)cfg->user_data, PIN_MODE_OUTPUT);
    rt_pin_write(*(rt_uint8_t *)cfg->user_data, PIN_HIGH);
    rt_thread_delay(10);
    // rst output 0
    rt_pin_mode(*(rt_uint8_t *)cfg->user_data, PIN_MODE_OUTPUT);
    rt_pin_write(*(rt_uint8_t *)cfg->user_data, PIN_LOW);
    rt_thread_delay(10);
    // rst output 1
    rt_pin_mode(*(rt_uint8_t *)cfg->user_data, PIN_MODE_OUTPUT);
    rt_pin_write(*(rt_uint8_t *)cfg->user_data, PIN_HIGH);
    rt_thread_delay(50);

    cst3240_client.bus =
        (struct rt_i2c_bus_device *)rt_device_find(cfg->dev_name);
    if (cst3240_client.bus == RT_NULL) {
        LOG_E("Can't find %s device", cfg->dev_name);
        return -RT_ERROR;
    }

    if (rt_device_open((rt_device_t)cst3240_client.bus, RT_DEVICE_FLAG_RDWR) != RT_EOK) {
        LOG_E("open %s device failed", cfg->dev_name);
        return -RT_ERROR;
    }

    cst3240_client.client_addr = CST3240_SLAVE_ADDR;

    /* register touch device */
    touch_device->info.type = RT_TOUCH_TYPE_CAPACITANCE;
    touch_device->info.vendor = RT_TOUCH_VENDOR_UNKNOWN;
    rt_memcpy(&touch_device->config, cfg, sizeof(struct rt_touch_config));
    touch_device->ops = &cst3240_touch_ops;

    if (RT_EOK != rt_hw_touch_register(touch_device, name, RT_DEVICE_FLAG_INT_RX, RT_NULL)) {
        LOG_E("touch device cst3240 init failed !!!");
        return -RT_ERROR;
    }

    LOG_I("touch device cst3240 init success");
    return RT_EOK;

}

static int rt_cst3240_gpio_cfg()
{
    unsigned int g, p;
    long pin;

    // RST
    pin = drv_pin_get(AIC_TOUCH_PANEL_RST_PIN);
    g = GPIO_GROUP(pin);
    p = GPIO_GROUP_PIN(pin);
    hal_gpio_direction_input(g, p);

    // INT
    pin = drv_pin_get(AIC_TOUCH_PANEL_INT_PIN);
    g = GPIO_GROUP(pin);
    p = GPIO_GROUP_PIN(pin);
    hal_gpio_direction_input(g, p);
    hal_gpio_set_irq_mode(g, p, 0);

    return 0;
}

static int rt_hw_cst3240_port(void)
{
    struct rt_touch_config cfg;
    rt_uint8_t rst_pin;

    rt_cst3240_gpio_cfg();

    rst_pin = drv_pin_get(AIC_TOUCH_PANEL_RST_PIN);
    cfg.dev_name = AIC_TOUCH_PANEL_I2C_CHAN;
    cfg.irq_pin.pin = drv_pin_get(AIC_TOUCH_PANEL_INT_PIN);
    cfg.irq_pin.mode = PIN_MODE_INPUT;
    cfg.user_data = &rst_pin;

    rt_hw_cst3240_init("cst3240", &cfg);

    return 0;
}

INIT_DEVICE_EXPORT(rt_hw_cst3240_port);
