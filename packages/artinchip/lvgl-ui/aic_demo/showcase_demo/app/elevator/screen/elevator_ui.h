/*
 * Copyright (C) 2023-2024 ArtinChip Technology Co., Ltd.
 * Authors:  Keliang Liu <keliang.liu@artinchip.com>
 *           Huahui Mai <huahui.mai@artinchip.com>
 */

#ifndef _AIC_DEMO_ELEVATOR_UI_H
#define _AIC_DEMO_ELEVATOR_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#define AIC_ELEVATOR_INSIDE      1
#define LV_ELEVATOR_UART_COMMAND 1

#define ELEVATOR_MAX_LEVEL 36

lv_obj_t * elevator_ui_init();

#ifdef LV_ELEVATOR_UART_COMMAND
int inside_up_down_ioctl(void *data, unsigned int data_len);
int inside_level_ioctl(void *data, unsigned int data_len);
int replayer_ioctl(void *data, unsigned int data_len);
#endif

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* ELEVATOR_UI_H */

