/*
 * Copyright (c) 2023-2024, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors:  ArtInChip
 */

#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "lvgl.h"
#include "aic_ui.h"
#include "camera_ui.h"

extern int msh_exec(char *cmd, rt_size_t length);
static void ui_camera_cb(lv_event_t *e);
static int lv_msh_exec(char *command, int length);

lv_obj_t *camera_ui_init(void)
{
    lv_obj_t *camera_ui = lv_obj_create(NULL);
#if defined(AIC_CHIP_D13X) || defined(AIC_CHIP_D21X)
    lv_msh_exec("test_dvp -c 0", rt_strlen("test_dvp -c 0"));
#endif
    lv_obj_add_event_cb(camera_ui, ui_camera_cb, LV_EVENT_ALL, NULL);
    return camera_ui;
}

static void ui_camera_cb(lv_event_t *e)
{
    lv_event_code_t code = (lv_event_code_t)lv_event_get_code(e);
    if (code == LV_EVENT_SCREEN_UNLOAD_START) {
#if defined(AIC_CHIP_D13X) || defined(AIC_CHIP_D21X)
        //release dvp resource
        extern void dvp_thread_stop();
        dvp_thread_stop();
#endif
    }
    if (code == LV_EVENT_DELETE) {;}
    if (code == LV_EVENT_SCREEN_LOADED) {;}
}

static int lv_msh_exec(char *command, int length)
{
    char cmd[256] = {0};
    strncpy(cmd, command, sizeof(cmd) - 1);
    return msh_exec(cmd, strlen(cmd));
}
