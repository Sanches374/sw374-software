/*
 * Copyright (C) 2023-2024 ArtinChip Technology Co., Ltd.
 * Authors:  Ning Fang <ning.fang@artinchip.com>
 */

#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "lvgl.h"
#include "slide_ui.h"
#include "hor_slide_screen.h"
#include "aic_ui.h"

lv_obj_t *scr_slide = NULL;

void slide_ui_init()
{
    scr_slide = hor_screen_init();
    lv_scr_load(scr_slide);
}
