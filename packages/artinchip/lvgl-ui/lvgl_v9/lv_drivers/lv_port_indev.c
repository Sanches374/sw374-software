/*
 * Copyright (c) 2024, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors:  Ning Fang <ning.fang@artinchip.com>
 */

#include <rtconfig.h>
#ifdef KERNEL_RTTHREAD
#include <lvgl.h>
#include <stdbool.h>
#include <rtthread.h>
#include <../components/drivers/include/drivers/touch.h>

static lv_indev_state_t last_state = LV_INDEV_STATE_REL;
static rt_int16_t last_x = 0;
static rt_int16_t last_y = 0;
static lv_indev_t *indev_touchpad;

static void input_read(lv_indev_t *indev_drv, lv_indev_data_t *data)
{
    data->point.x = last_x;
    data->point.y = last_y;
    data->state = last_state;
}

void aic_touch_inputevent_cb(rt_int16_t x, rt_int16_t y, rt_uint8_t state)
{
    switch (state)
    {
    case RT_TOUCH_EVENT_UP:
        last_state = LV_INDEV_STATE_RELEASED;
        break;
    case RT_TOUCH_EVENT_MOVE:
    case RT_TOUCH_EVENT_DOWN:
        last_x = x;
        last_y = y;
        last_state = LV_INDEV_STATE_PRESSED;
        break;
#ifdef AIC_MONKEY_TEST
    case RT_TOUCH_MONKEY_TEST:
        last_x = x;
        last_y = y;
        last_state = LV_INDEV_STATE_PRESSED;
        break;
#endif
    }
}

void lv_port_indev_init(void)
{
    indev_touchpad = lv_indev_create();
    lv_indev_set_type(indev_touchpad, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev_touchpad, input_read);
}
#endif
