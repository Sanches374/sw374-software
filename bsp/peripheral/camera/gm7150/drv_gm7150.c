/*
 * Copyright (c) 2024, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors: matteo <duanmt@artinchip.com>
 */

#define LOG_TAG     "gm7150"

#include <drivers/i2c.h>
#include <drivers/pin.h>

#include "aic_core.h"
#include "mpp_types.h"
#include "mpp_img_size.h"
#include "mpp_vin.h"

#include "drv_camera.h"

/* Default format configuration of GM7150 */
#define GM7150_DFT_WIDTH        PAL_WIDTH
#define GM7150_DFT_HEIGHT       PAL_HEIGHT
#define GM7150_DFT_BUS_TYPE     MEDIA_BUS_BT656
#define GM7150_DFT_CODE         MEDIA_BUS_FMT_UYVY8_2X8

#define GM7150_I2C_SLAVE_ID     0x5C // or 0x5D
#define GM7150_CHIP_ID          0x7150

struct gm7150_dev {
    struct rt_device dev;
    struct rt_i2c_bus_device *i2c;
    u32 rst_pin;
    u32 pwdn_pin;

    struct mpp_video_fmt fmt;

    bool streaming;
};

static struct gm7150_dev g_gm7150_dev = {0};

static int gm7150_write_reg(struct rt_i2c_bus_device *i2c, u8 reg, u8 val)
{
    u8 buf[2];
    struct rt_i2c_msg msgs;

    buf[0] = reg;
    buf[1] = val;

    msgs.addr = GM7150_I2C_SLAVE_ID;
    msgs.flags = RT_I2C_WR;
    msgs.buf = buf;
    msgs.len = 2;

    if (rt_i2c_transfer(i2c, &msgs, 1) != 1) {
        LOG_E("%s: error: reg = 0x%x, val = 0x%x", __func__, reg, val);
        return -1;
    }

    return 0;
}

static int gm7150_read_reg(struct rt_i2c_bus_device *i2c, u8 reg, u8 *val)
{
    struct rt_i2c_msg msg[2];
    u8 buf = reg;

    msg[0].addr  = GM7150_I2C_SLAVE_ID;
    msg[0].flags = RT_I2C_WR;
    msg[0].buf   = &buf;
    msg[0].len   = 1;

    msg[1].addr  = GM7150_I2C_SLAVE_ID;
    msg[1].flags = RT_I2C_RD;
    msg[1].buf   = val;
    msg[1].len   = 1;

    if (rt_i2c_transfer(i2c, msg, 2) != 2) {
        LOG_E("%s: error: reg = 0x%x, val = 0x%x", __func__, reg, *val);
        return -1;
    }

    return 0;
}

static void gm7150_select_ch(struct gm7150_dev *sensor, u32 ch)
{
    if (ch)
        gm7150_write_reg(sensor->i2c, 0x00, 0x02);
    else
        gm7150_write_reg(sensor->i2c, 0x00, 0x00);
}

static void gm7150_out_bt656(struct gm7150_dev *sensor)
{
    gm7150_write_reg(sensor->i2c, 0x03, 0x0D);
    gm7150_write_reg(sensor->i2c, 0x11, 0x04);
    gm7150_write_reg(sensor->i2c, 0x12, 0x00);
    gm7150_write_reg(sensor->i2c, 0x13, 0x04);
    gm7150_write_reg(sensor->i2c, 0x14, 0x00);
    gm7150_write_reg(sensor->i2c, 0xA0, 0x55);
    gm7150_write_reg(sensor->i2c, 0xA1, 0xAA);
    gm7150_write_reg(sensor->i2c, 0x69, 0x40);
    gm7150_write_reg(sensor->i2c, 0x6D, 0x90);
}

static void gm7150_cur_status(struct gm7150_dev *sensor)
{
    u8 val = 0, fmt = 0;
    char *formats[] = {"Reserved", "NTSC BT.601", "Reserved", "PAL BT.601",
                       "Reserved", "(M)PAL BT.601", "Reserved", "PAL-N BT.601",
                       "Reserved", "NTSC 4.43 BT.601", "Reserved", "Reserved",
                       "Reserved", "Reserved", "Reserved", "Reserved"};

    gm7150_read_reg(sensor->i2c, 0x88, &val);
    LOG_I("Reg 0x%02x: 0x%02x. Input signal is %s\n", 0x88, val,
          (val & 0x6) == 0x6 ? "valid" : "invalid");

    gm7150_read_reg(sensor->i2c, 0x8C, &val);
    fmt = val & 0xF;
    LOG_I("Reg 0x%02x: 0x%02x. Input format: %s\n", 0x8C, val,
          formats[fmt]);

    if (fmt == 0x1 || fmt == 9)
        g_gm7150_dev.fmt.height = NTSC_HEIGHT;
}

static int gm7150_probe(struct gm7150_dev *sensor)
{
    u8 id_h = 0, id_l = 0;

    aicos_msleep(30);
    rt_pin_write(sensor->pwdn_pin, 0);  //SET PDN LOW BY GPIO OF MCU
    aicos_msleep(10);
    rt_pin_write(sensor->pwdn_pin, 1);   //SET PDN HIGH BY GPIO OF MCU

    aicos_msleep(30);

    if (gm7150_read_reg(sensor->i2c, 0x80, &id_h) ||
        gm7150_read_reg(sensor->i2c, 0x81, &id_l))
        return -1;

    if ((id_h << 8 | id_l) != GM7150_CHIP_ID) {
        LOG_E("Invalid chip ID: %02x %02x\n", id_h, id_l);
        return -1;
    }
    gm7150_select_ch(sensor, 0);
    gm7150_out_bt656(sensor);
    gm7150_cur_status(sensor);

    return 0;
}

static int gm7150_i2c_init(struct gm7150_dev *sensor)
{
    char name[8] = "";

    snprintf(name, 8, "i2c%d", AIC_CAMERA_I2C_CHAN);
    sensor->i2c = rt_i2c_bus_device_find(name);
    if (sensor->i2c == RT_NULL) {
        LOG_E("Failed to open %s", name);
        return -ENODEV;
    }

    return 0;
}

static rt_err_t gm7150_init(rt_device_t dev)
{
    int ret = 0;
    struct gm7150_dev *sensor = &g_gm7150_dev;

    ret = gm7150_i2c_init(sensor);
    if (ret != 0)
        return -RT_EINVAL;

    sensor->fmt.code   = GM7150_DFT_CODE;
    sensor->fmt.width  = GM7150_DFT_WIDTH;
    sensor->fmt.height = GM7150_DFT_HEIGHT;
    sensor->fmt.bus_type = GM7150_DFT_BUS_TYPE;
    sensor->fmt.flags = MEDIA_SIGNAL_HSYNC_ACTIVE_HIGH |
                        MEDIA_SIGNAL_VSYNC_ACTIVE_HIGH |
                        MEDIA_SIGNAL_PCLK_SAMPLE_RISING |
                        MEDIA_SIGNAL_INTERLACED_MODE;

    sensor->pwdn_pin = rt_pin_get(AIC_CAMERA_PWDN_PIN);
    rt_pin_mode(sensor->pwdn_pin, PIN_MODE_OUTPUT);

    ret = gm7150_probe(sensor);
    if (ret)
        return -RT_ERROR;

    LOG_I("GM7150 inited");
    return RT_EOK;
}

static rt_err_t gm7150_open(rt_device_t dev, rt_uint16_t oflag)
{
    return RT_EOK;
}

static rt_err_t gm7150_close(rt_device_t dev)
{
    return RT_EOK;
}

static int gm7150_get_fmt(rt_device_t dev, struct mpp_video_fmt *cfg)
{
    struct gm7150_dev *sensor = (struct gm7150_dev *)dev;

    cfg->code   = sensor->fmt.code;
    cfg->width  = sensor->fmt.width;
    cfg->height = sensor->fmt.height;
    cfg->flags  = sensor->fmt.flags;
    cfg->bus_type = sensor->fmt.bus_type;
    return RT_EOK;
}

static int gm7150_start(rt_device_t dev)
{
    return 0;
}

static int gm7150_stop(rt_device_t dev)
{
    return 0;
}

static rt_err_t gm7150_control(rt_device_t dev, int cmd, void *args)
{
    switch (cmd) {
    case CAMERA_CMD_START:
        return gm7150_start(dev);
    case CAMERA_CMD_STOP:
        return gm7150_stop(dev);
    case CAMERA_CMD_GET_FMT:
        return gm7150_get_fmt(dev, (struct mpp_video_fmt *)args);
    default:
        LOG_I("Unsupported cmd: 0x%x", cmd);
        return -RT_EINVAL;
    }
    return RT_EOK;
}

#ifdef RT_USING_DEVICE_OPS
static const struct rt_device_ops gm7150_ops =
{
    .init = gm7150_init,
    .open = gm7150_open,
    .close = gm7150_close,
    .control = gm7150_control,
};
#endif

int rt_hw_gm7150_init(void)
{
#ifdef RT_USING_DEVICE_OPS
    g_gm7150_dev.dev.ops = &gm7150_ops;
#else
    g_gm7150_dev.dev.init = gm7150_init;
    g_gm7150_dev.dev.open = gm7150_open;
    g_gm7150_dev.dev.close = gm7150_close;
    g_gm7150_dev.dev.control = gm7150_control;
#endif
    g_gm7150_dev.dev.type = RT_Device_Class_CAMERA;

    rt_device_register(&g_gm7150_dev.dev, CAMERA_DEV_NAME, 0);
    return 0;
}
INIT_DEVICE_EXPORT(rt_hw_gm7150_init);
