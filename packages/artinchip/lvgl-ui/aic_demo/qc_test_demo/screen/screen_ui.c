/*
 * Copyright (c) 2023-2024, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors:  ArtInChip
 */
#include <rtconfig.h>
#ifdef KERNEL_RTTHREAD
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <rtthread.h>
#include <rtdevice.h>
#include <drivers/rtc.h>
#include "lvgl/lvgl.h"
#include "aic_ui.h"
#include "aic_core.h"
#include "aic_common.h"
#include "lwip/netif.h"

#include "../module/qc_ops.h"
#include "../module/qc_save.h"
#include "../qc_test_demo.h"

#define AIC_GPAI_VOLTAGE_ACCURACY 10000
#define GPAI_VOLTAGE_ERROR 300
#define RTC_TEST_TIME      5
#define QC_TASK_THREAD_STACK_SIZE 4096

static rt_mutex_t mgr_mutex = NULL;
static struct rt_thread qc_task_thread;

static struct qc_manager *mgr;

static lv_obj_t *start_label;
static lv_obj_t *start_btn;

static lv_style_t arc_style_main;
static lv_style_t arc_style_indicate;
static lv_style_t arc_style_pass_indicate;
static lv_style_t arc_style_err_indicate;
static lv_obj_t *arc_label;
static lv_obj_t *arc;
static lv_anim_t arc_anim;

static lv_obj_t * rtc_time;
static lv_obj_t * table;
static lv_obj_t *mgr_result_label;

static struct qc_module *test_modules[8];
static char* module_name[] = {"GPIO", "GPAI", "CARD", "DSPK", "UART", "MAC", "RTC", "USB"};
static char* module_desc[] = {
    "GPIO R-W comparison",
    "GPAI Read comparison",
    "CARD R-W comparison",
    "DISP Write comparison",
    "UART R-W comparison",
    "MAC R-W comparison",
    "RTC R-W comparison",
    "USB R-W comparison"
};

static void mgr_lock(void)
{
    if (mgr_mutex)
        rt_mutex_take(mgr_mutex, RT_WAITING_FOREVER);
}

static void mgr_un_lock(void)
{
    if (mgr_mutex)
        rt_mutex_release(mgr_mutex);
}

static int gpai_0_cmp(int msh_result)
{
    if (abs(msh_result - (2.14 * AIC_GPAI_VOLTAGE_ACCURACY)) > GPAI_VOLTAGE_ERROR) {
        return FAILURE;
    }
    return SUCCESS;
}

static int gpai_1_cmp(int msh_result)
{
    if (abs(msh_result - (1.79 * AIC_GPAI_VOLTAGE_ACCURACY)) > GPAI_VOLTAGE_ERROR) {
        return FAILURE;
    }
    return SUCCESS;
}

static int gpai_2_cmp(int msh_result)
{
    if (abs(msh_result - (1.43 * AIC_GPAI_VOLTAGE_ACCURACY)) > GPAI_VOLTAGE_ERROR) {
        return FAILURE;
    }
    return SUCCESS;
}

static int gpai_3_cmp(int msh_result)
{
    if (abs(msh_result - (1.07 * AIC_GPAI_VOLTAGE_ACCURACY)) > GPAI_VOLTAGE_ERROR) {
        return FAILURE;
    }
    return SUCCESS;
}

static int gpai_4_cmp(int msh_result)
{
    if (abs(msh_result - (0.71 * AIC_GPAI_VOLTAGE_ACCURACY)) > GPAI_VOLTAGE_ERROR) {
        return FAILURE;
    }
    return SUCCESS;
}

static int gpai_5_cmp(int msh_result)
{
    if (abs(msh_result - (0.36 * AIC_GPAI_VOLTAGE_ACCURACY)) > GPAI_VOLTAGE_ERROR) {
        return FAILURE;
    }
    return SUCCESS;
}

static int rtc_module_test(void *param)
{
    int flag = *(int *)param;
    time_t now;
    struct tm *local_time;
    static int last_hour = 0;
    static int last_min = 0;
    static int last_sec = 0;

    if (flag == 1) {
        if (set_date(2024, 3, 20) != RT_EOK) {
            printf("set RTC date failed");
            return FAILURE;
        }
        rt_thread_mdelay(1);
        if (set_time(00, 00, 00) != RT_EOK) {
            printf("set RTC time failed");
            return FAILURE;
        }
    } else if (flag == 2) {
        now = time(RT_NULL);
        local_time = localtime(&now);
        last_hour = local_time->tm_hour;
        last_min = local_time->tm_min;
        last_sec = local_time->tm_sec;
    } else if (flag == 0) {
        now = time(RT_NULL);
        local_time = localtime(&now);
        if (abs(local_time->tm_year + 1900 - 2024) > 0 ||
            abs(local_time->tm_mon + 1 - 3) > 0 ||
            abs(local_time->tm_mday - 20) > 0) {
                return FAILURE;
        } else {
            int now_seconds = local_time->tm_hour *3600 + local_time->tm_min * 60 + local_time->tm_sec;
            int last_seconds = last_hour * 3600 + last_min * 60 + last_sec;
            if (abs(last_seconds - now_seconds) > RTC_TEST_TIME) {
                return FAILURE;
            }
        }
    }
    return SUCCESS;
}

static int mac_test_disp_testing(void *param)
{
    return SUCCESS;
}

static int mac_wait_start_up(void *param)
{
    while (!netif_is_link_up(netif_default)) {
        rt_thread_mdelay(50);
    }
    return SUCCESS;
}

static int write_to_file(const char *filename, const char *content, size_t content_len)
{
    int fd;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

    fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, mode);
    if (fd == -1) {
        printf("error opening file for writing, path = %s\n", filename);
        return FAILURE;
    }

    size_t total_written = 0;
    while (total_written < content_len) {
        ssize_t written = write(fd, content + total_written, content_len - total_written);
        if (written == -1) {
            printf("error writing to file\n");
            close(fd);
            return FAILURE;
        }

        total_written += written;

        if (written == 0 || total_written != content_len) {
            close(fd);
            return FAILURE;
        }
    }

    if (close(fd) == -1) {
        printf("error closing file\n");
        return FAILURE;
    }

    if (unlink(filename) == -1) {
        printf("error unlinking file\n");
        return FAILURE;
    }

    return SUCCESS;
}

const char write_buff[512];
static int sdcard_module_test(void *param)
{
    const char *file_name = "/sdcard/sd_text.txt";
    return write_to_file(file_name, write_buff, sizeof(write_buff));
}

static int usb_module_test(void *param)
{
    const char *file_name = "/udisk/udisk_text.txt";
    return write_to_file(file_name, write_buff, sizeof(write_buff));
}

static int qc_save_chip(struct qc_manager *mgr, struct qc_module *module)
{
    int ret = FAILURE;
    char *qc_save_path = "udisk/qc_save/d13c/";

    ret =  qc_save_write(mgr, qc_save_path);
    if (ret == FAILURE) {
        goto QC_SAVE_FAILED;
    }

    return SUCCESS;

QC_SAVE_FAILED:
    module->exec_sets_result[1] = FAILURE;
    mgr->lock();
    mgr->success_num--;
    module->status = FAILURE;
    mgr->un_lock();
    qc_manager_update(mgr, module);
    return FAILURE;
}

void qc_exec_task(void *arg)
{
    struct qc_disp *disp = qc_disp_create();
    struct qc_mod_exec *exec = qc_mod_exec_create();
    struct qc_module *module = qc_module_create();

    while(1) {
        if (qc_mod_exec_pop(exec, mgr) == SUCCESS) {
            int ret = qc_manager_get_module(mgr, exec->module_name, module);
            if (ret != SUCCESS) {
                continue;
            }

            ret = qc_module_execute(module, exec->sets, exec->exec_flag);
            if (ret != UPDATED) {
                continue;
            }

            qc_manager_update(mgr, module);
            if (mgr->progress == 100) {
                qc_save_chip(mgr, module);
            }
            qc_disp_set(disp, mgr, module);
            qc_disp_add_tail(disp, mgr);
        }
        rt_thread_mdelay(10);
    }

    qc_disp_delete(disp);
    qc_mod_exec_delete(exec);
    qc_module_delete(module);
}

static int module_name_to_table_row(char *name)
{
    int num_modules = sizeof(module_name) / sizeof(module_name[0]);

    for (int i = 0; i < num_modules; i++) {
        if (strncmp(name, module_name[i], strlen(module_name[i])) == 0) {
            return i + 1;
        }
    }

    return -1;
}

static void table_cell_update(struct qc_disp *disp)
{
    int table_row_pos = -1;
    char failure_item[128] =  {0};

    table_row_pos = module_name_to_table_row(disp->module_name);
    if (table_row_pos > 0) {
        if (disp->module_status == EXECUTING) {
            lv_table_set_cell_value(table, table_row_pos, 3, "Testing");
        } else if (disp->module_status == SUCCESS) {
            lv_table_set_cell_value(table, table_row_pos, 3, "Success");
        } else if (disp->module_status == FAILURE) {
            lv_table_set_cell_value(table, table_row_pos, 3, "Failure");
        }

        for (int i = 0; i < EXEC_SETS; i++) {
            if (disp->exec_sets_result[i] == FAILURE) {
                strcat(failure_item, disp->exec_sets_desc[i]);
                strcat(failure_item, " failure\n");
            }
        }

        /* clear the line break on the last line */
        int len = strlen(failure_item);
        if (len > 0) {
            *(failure_item + len - 1) = '\0';
            lv_table_set_cell_value(table, table_row_pos, 2, failure_item);
        }
    }
}

static void mgr_result_label_update(struct qc_disp *disp)
{
    char mgr_result_context[100] = {0};

    if (disp->success_num != 0 || disp->failure_num != 0) {
        snprintf(mgr_result_context, sizeof(mgr_result_context),
            "Passed items: %d                       Failed items: %d",
            disp->success_num, disp->failure_num);
        lv_label_set_text(mgr_result_label, mgr_result_context);
    }
}

static void arc_update(struct qc_disp *disp)
{
    static int last_progress = 0;

    lv_anim_set_values(&arc_anim, last_progress, disp->progress);
    lv_anim_start(&arc_anim);

    last_progress = disp->progress;

    if (lv_obj_has_flag(arc_label, LV_OBJ_FLAG_HIDDEN))
        lv_obj_clear_flag(arc_label, LV_OBJ_FLAG_HIDDEN);

    if (disp->progress == 100) {
        if (disp->failure_num != 0) {
            lv_obj_add_style(arc, &arc_style_err_indicate,
                                     LV_PART_INDICATOR);
            lv_anim_set_values(&arc_anim, disp->progress, disp->progress);
            lv_anim_start(&arc_anim);

            lv_label_set_text(arc_label, "#f44336 Failure #"); /* main red */
        } else {
            lv_obj_add_style(arc, &arc_style_pass_indicate,
                                     LV_PART_INDICATOR);
            lv_anim_set_values(&arc_anim, disp->progress, disp->progress);
            lv_anim_start(&arc_anim);

            lv_label_set_text(arc_label, "#4caf50 Success #"); /* main gree */
        }
    } else if (disp->progress >= 0 && disp->progress < 100) {
            lv_label_set_text(arc_label, "#2196f3 Testing#"); /* main blue */
    }
}

static void start_btn_update(int progress)
{
    if (progress == 100) {
        lv_label_set_text(start_label, "Restart");
    } else if (progress > 0 && progress < 100) {
        lv_label_set_text(start_label, "Testing");
    }
}

static void qc_draw_task(lv_timer_t *lv_timer)
{
    struct qc_disp draw_disp = {0};

    if (qc_disp_pop(&draw_disp, mgr) == SUCCESS) {
        table_cell_update(&draw_disp);
        mgr_result_label_update(&draw_disp);
        arc_update(&draw_disp);
        start_btn_update(draw_disp.progress);
    }
}

static void qc_mod_exec_set_add_tail(char *mod_name, int sets, int exec_flag)
{
    struct qc_mod_exec tmp_exec;
    qc_mod_exec_set(&tmp_exec, mod_name, sets, exec_flag);
    qc_mod_exec_add_tail(&tmp_exec, mgr);
}

static void qc_ops_exec_create()
{
    qc_mod_exec_set_add_tail("RTC", 0, EXEC_FUNC);

    for (int i = 0; i < 6; i++) {
        qc_mod_exec_set_add_tail("GPIO", i, EXEC_CMD);
        qc_mod_exec_set_add_tail("GPAI", i, EXEC_CMD);
    }

    qc_mod_exec_set_add_tail("CARD", 0, EXEC_FUNC);
    qc_mod_exec_set_add_tail("DSPK", 0, EXEC_CMD);
    qc_mod_exec_set_add_tail("UART", 0, EXEC_CMD);
    qc_mod_exec_set_add_tail("UART", 2, EXEC_CMD);
    qc_mod_exec_set_add_tail("UART", 3, EXEC_CMD);
    qc_mod_exec_set_add_tail("UART", 4, EXEC_CMD);
    qc_mod_exec_set_add_tail("MAC", 0, EXEC_FUNC);
    qc_mod_exec_set_add_tail("MAC", 1, EXEC_FUNC);
    qc_mod_exec_set_add_tail("MAC", 2, EXEC_CMD);
    qc_mod_exec_set_add_tail("RTC", 1, EXEC_FUNC);
    qc_mod_exec_set_add_tail("USB", 0, EXEC_FUNC);
}

static struct qc_module *name_to_module(char *name)
{
    for (int i = 0; i < sizeof(module_name) / sizeof(module_name[0]); ++i) {
       if (strncmp(name, module_name[i], strlen(module_name[i])) == 0) {
           return test_modules[i];
       }
    }
    return NULL;
}

static void qc_ops_create(void)
{
    static int write = 1, read_start = 2, read = 0;

    mgr = qc_manager_create();
    qc_manager_add_name(mgr, "lite_mgr");

    for (int i = 0; i < sizeof(test_modules) / sizeof(test_modules[0]); ++i) {
        test_modules[i] = qc_module_create();
    }

    for (int i = 0; i < sizeof(test_modules) / sizeof(test_modules[0]); ++i) {
        qc_module_add_name(test_modules[i], module_name[i]);
        qc_module_add_desc(test_modules[i], module_desc[i]);
    }

    qc_module_add_sets_desc(name_to_module("GPIO"), 0, "GROUP0");
    qc_module_add_sets_desc(name_to_module("GPIO"), 1, "GROUP1");
    qc_module_add_sets_desc(name_to_module("GPIO"), 2, "GROUP2");
    qc_module_add_sets_desc(name_to_module("GPIO"), 3, "GROUP3");
    qc_module_add_sets_desc(name_to_module("GPIO"), 4, "GROUP4");
    qc_module_add_sets_desc(name_to_module("GPIO"), 5, "GROUP5");

    qc_module_add_sets_desc(name_to_module("GPAI"), 0, "GPAI0");
    qc_module_add_sets_desc(name_to_module("GPAI"), 1, "GPAI1");
    qc_module_add_sets_desc(name_to_module("GPAI"), 2, "GPAI2");
    qc_module_add_sets_desc(name_to_module("GPAI"), 3, "GPAI3");
    qc_module_add_sets_desc(name_to_module("GPAI"), 4, "GPAI4");
    qc_module_add_sets_desc(name_to_module("GPAI"), 5, "GPAI5");

    qc_module_add_sets_desc(name_to_module("CARD"), 0, "CARD");
    qc_module_add_sets_desc(name_to_module("DSPK"), 0, "DSPK");
    qc_module_add_sets_desc(name_to_module("MAC"),  2, "MAC0");

    qc_module_add_sets_desc(name_to_module("UART"), 0, "UART0");
    qc_module_add_sets_desc(name_to_module("UART"), 2, "UART2");
    qc_module_add_sets_desc(name_to_module("UART"), 3, "UART5");
    qc_module_add_sets_desc(name_to_module("UART"), 4, "UART7");

    qc_module_add_sets_desc(name_to_module("RTC"),  1, "RTC");
    qc_module_add_sets_desc(name_to_module("USB"),  0, "USB0");
    qc_module_add_sets_desc(name_to_module("USB"),  1, "CHIP SAVE");

    qc_module_add_cmd(name_to_module("GPAI"), 0, "test_gpai -n 1 -c 0 -t 2.5", RECORD);
    qc_module_add_cmp(name_to_module("GPAI"), 0, gpai_0_cmp);
    qc_module_add_cmd(name_to_module("GPAI"), 1, "test_gpai -n 1 -c 1 -t 2.5", RECORD);
    qc_module_add_cmp(name_to_module("GPAI"), 1, gpai_1_cmp);
    qc_module_add_cmd(name_to_module("GPAI"), 2, "test_gpai -n 1 -c 2 -t 2.5", RECORD);
    qc_module_add_cmp(name_to_module("GPAI"), 2, gpai_2_cmp);
    qc_module_add_cmd(name_to_module("GPAI"), 3, "test_gpai -n 1 -c 3 -t 2.5", RECORD);
    qc_module_add_cmp(name_to_module("GPAI"), 3, gpai_3_cmp);
    qc_module_add_cmd(name_to_module("GPAI"), 4, "test_gpai -n 1 -c 4 -t 2.5", RECORD);
    qc_module_add_cmp(name_to_module("GPAI"), 4, gpai_4_cmp);
    qc_module_add_cmd(name_to_module("GPAI"), 5, "test_gpai -n 1 -c 5 -t 2.5", RECORD);
    qc_module_add_cmp(name_to_module("GPAI"), 5, gpai_5_cmp);

    qc_module_add_cmd(name_to_module("GPIO"), 0, "test_gpio -t 10 -i PB.8 -o PB.9", RECORD);
    qc_module_add_cmd(name_to_module("GPIO"), 0, "test_gpio -t 10 -i PB.9 -o PB.8", RECORD);
    qc_module_add_cmd(name_to_module("GPIO"), 1, "test_gpio -t 10 -i PD.9 -o PD.10", RECORD);
    qc_module_add_cmd(name_to_module("GPIO"), 1, "test_gpio -t 10 -i PD.10 -o PD.9", RECORD);
    qc_module_add_cmd(name_to_module("GPIO"), 2, "test_gpio -t 10 -i PD.11 -o PD.12", RECORD);
    qc_module_add_cmd(name_to_module("GPIO"), 2, "test_gpio -t 10 -i PD.12 -o PD.11", RECORD);
    qc_module_add_cmd(name_to_module("GPIO"), 3, "test_gpio -t 10 -i PD.13 -o PD.14", RECORD);
    qc_module_add_cmd(name_to_module("GPIO"), 3, "test_gpio -t 10 -i PD.14 -o PD.13", RECORD);
    qc_module_add_cmd(name_to_module("GPIO"), 4, "test_gpio -t 10 -i PD.15 -o PD.16", RECORD);
    qc_module_add_cmd(name_to_module("GPIO"), 4, "test_gpio -t 10 -i PD.16 -o PD.15", RECORD);
    qc_module_add_cmd(name_to_module("GPIO"), 5, "test_gpio -t 10 -i PE.12 -o PE.13", RECORD);
    qc_module_add_cmd(name_to_module("GPIO"), 5, "test_gpio -t 10 -i PE.13 -o PE.12", RECORD);

    qc_module_add_cmd(name_to_module("DSPK"), 0, "aplay sound0 /data/du.wav", RECORD);

    qc_module_add_func(name_to_module("MAC"), 0, mac_test_disp_testing, NULL, RECORD);
    qc_module_add_func(name_to_module("MAC"), 1, mac_wait_start_up, NULL, RECORD);
    qc_module_add_cmd(name_to_module("MAC"),  2, "test_eth -d 10 -n 10", RECORD);

    qc_module_add_cmd(name_to_module("UART"), 0, "test_uart uart0", RECORD);
    qc_module_add_cmd(name_to_module("UART"), 2, "test_uart uart2", RECORD);
    qc_module_add_cmd(name_to_module("UART"), 3, "test_uart uart5", RECORD);
    qc_module_add_cmd(name_to_module("UART"), 4, "test_uart uart7", RECORD);

    qc_module_add_func(name_to_module("RTC"), 0, rtc_module_test, &read_start, NOT_RECORD);
    qc_module_add_func(name_to_module("RTC"), 1, rtc_module_test, &read, RECORD);

    qc_module_add_func(name_to_module("CARD"), 0, sdcard_module_test, NULL, RECORD);

    qc_module_add_func(name_to_module("USB"), 0, usb_module_test, NULL, RECORD);

    for (int i = 0; i < sizeof(test_modules) / sizeof(test_modules[0]); ++i) {
        qc_manager_add_module(mgr, test_modules[i]);
        qc_module_delete(test_modules[i]);
    }

    qc_manager_add_lock(mgr, mgr_lock, mgr_un_lock);

    rtc_module_test(&write);
}

static void sys_time_update(lv_timer_t *lv_timer)
{
    time_t now;
    struct tm *local_time;

    now = time(RT_NULL);
    local_time = localtime(&now);

    lv_label_set_text_fmt(rtc_time, "%04d-%02d-%02d  %02d:%02d:%02d", local_time->tm_year+1900, local_time->tm_mon+1, local_time->tm_mday, local_time->tm_hour, local_time->tm_min, local_time->tm_sec);
}

static void title_init(void)
{
    lv_obj_t * top_bg = lv_obj_create(lv_scr_act());
    lv_obj_set_style_bg_color(top_bg, lv_palette_darken(LV_PALETTE_BLUE, 3), 0);
    lv_obj_set_pos(top_bg, 0, 0);
    lv_obj_set_size(top_bg, 1024, 54);
    lv_obj_clear_flag(top_bg, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t * title = lv_label_create(top_bg);
    lv_obj_set_style_text_color(title, lv_palette_lighten(LV_PALETTE_BLUE, 5), 0);
    lv_label_set_text(title, "ArtInChip QC Test Tool V0.13");
    lv_obj_set_align(title, LV_ALIGN_CENTER);

    lv_obj_t * chip_version = lv_label_create(top_bg);
    lv_obj_set_style_text_color(chip_version, lv_palette_lighten(LV_PALETTE_BLUE, 5), 0);

    lv_label_set_text(chip_version, "Board: D13C V1.0");
    lv_obj_set_align(chip_version, LV_ALIGN_TOP_LEFT);

    rtc_time = lv_label_create(top_bg);
    lv_obj_set_style_text_color(rtc_time,  lv_palette_lighten(LV_PALETTE_BLUE, 5), 0);
    lv_obj_set_align(rtc_time, LV_ALIGN_RIGHT_MID);
    lv_label_set_text(rtc_time, " ");
}

static void draw_table_event_cb(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    lv_obj_draw_part_dsc_t * dsc = lv_event_get_draw_part_dsc(e);
    /* If the cells are drawn... */
    if(dsc->part == LV_PART_ITEMS) {
        uint32_t row = dsc->id /  lv_table_get_col_cnt(obj);

        /* Make the texts in the first cell center aligned */
        dsc->label_dsc->align = LV_TEXT_ALIGN_CENTER;
        if(row == 0) {
            dsc->rect_dsc->bg_color = lv_color_mix(lv_palette_main(LV_PALETTE_BLUE), dsc->rect_dsc->bg_color, LV_OPA_20);
            dsc->rect_dsc->bg_opa = LV_OPA_COVER;
        }

        /* MAke every 2nd row grayish */
        if((row != 0 && row % 2) == 0) {
            dsc->rect_dsc->bg_color = lv_color_mix(lv_palette_main(LV_PALETTE_GREY), dsc->rect_dsc->bg_color, LV_OPA_10);
            dsc->rect_dsc->bg_opa = LV_OPA_COVER;
        }

    }
}

static void table_init(void)
{
    struct qc_module *temp_module = {0};
    int table_row_pos = {0};

    const char *module_list_name[8] = {
        "GPAI", "DSPK", "GPIO", "RTC",
        "MAC", "UART", "CARD", "USB"
    };

    table = lv_table_create(lv_scr_act());
    lv_obj_set_scroll_dir(table, LV_DIR_TOP | LV_DIR_BOTTOM | LV_DIR_VER);
    lv_obj_set_scrollbar_mode(table, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_pos(table, 0, 54);
    lv_obj_set_size(table, 680, 544);
    lv_table_set_row_cnt(table, 10);
    lv_table_set_col_cnt(table, 4);
    lv_table_set_col_width(table, 0, 113);
    lv_table_set_col_width(table, 1, 227);
    lv_table_set_col_width(table, 2, 227);
    lv_table_set_col_width(table, 3, 113);

    lv_table_set_cell_value(table, 0, 0, "Module");
    lv_table_set_cell_value(table, 0, 1, "Testing item");
    lv_table_set_cell_value(table, 0, 2, "Failure item");
    lv_table_set_cell_value(table, 0, 3, "Status");

    temp_module = qc_module_create();
    if (temp_module  == NULL) {
        printf("qc module create failed, can't create temp module\n");
        return;
    }

    for (int i = 0; i < sizeof(module_list_name) / sizeof(module_list_name[0]); i++) {
        int ret = qc_manager_get_module(mgr, (char *)module_list_name[i], temp_module);
        if (ret == SUCCESS) {
            table_row_pos = module_name_to_table_row(temp_module->name);
            lv_table_set_cell_value(table, table_row_pos, 0, temp_module->name);
            lv_table_set_cell_value(table, table_row_pos, 1, temp_module->desc);
        }
    }

    qc_module_delete(temp_module);
    /*Add an event callback to to apply some custom drawing*/
    lv_obj_add_event_cb(table, draw_table_event_cb, LV_EVENT_DRAW_PART_BEGIN, NULL);
}

static void table_status_clear()
{
    /* due to the unique nature of the table object, delete and recreate the table
     * object to force a refresh of the table.
     */
    if (table) {
        lv_obj_del(table);
    }
    table_init();
}

static void arc_status_clear()
{
    lv_obj_add_style(arc, &arc_style_indicate, LV_PART_INDICATOR);

    lv_anim_set_time(&arc_anim, 0);
    lv_anim_set_values(&arc_anim, 0, 0);
    lv_anim_start(&arc_anim);
    lv_anim_set_time(&arc_anim, 1000);

    lv_obj_add_flag(arc_label, LV_OBJ_FLAG_HIDDEN);
}

static void mgr_result_label_clear()
{
    char mgr_result_context[100];

    snprintf(mgr_result_context, sizeof(mgr_result_context),
           "Passed items: %s                       Failed items: %s",
           "--", "--");
    lv_label_set_text(mgr_result_label, mgr_result_context);
}

static void start_btn_handler(lv_event_t * e)
{
    int qc_exec_empty = 0;
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_PRESSED) {
        if (mgr->exec_num == 0)
            qc_exec_empty = 1;

        if (qc_exec_empty == 1 && mgr->progress == 100) {
            table_status_clear();
            arc_status_clear();
            mgr_result_label_clear();
            qc_manager_clear_status(mgr);
            qc_ops_exec_create();
        } else if (qc_exec_empty == 1 && mgr->progress == 0) {
            qc_ops_exec_create();
        }
    }
}

static void start_btn_init(void)
{
    static lv_style_t start_btn_style;

    start_btn = lv_btn_create(lv_scr_act());
    lv_obj_clear_flag(start_btn, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_pos(start_btn, 690, 70);
    lv_obj_set_size(start_btn, 323, 87);

    lv_obj_add_event_cb(start_btn, start_btn_handler, LV_EVENT_ALL, NULL);
    lv_style_init(&start_btn_style);
    lv_style_set_bg_color(&start_btn_style, lv_palette_main(LV_PALETTE_BLUE));
    lv_obj_add_style(start_btn, &start_btn_style, 0);

    start_label = lv_label_create(start_btn);
    lv_obj_clear_flag(start_label, LV_OBJ_FLAG_SCROLLABLE);
    lv_label_set_text(start_label, "Start");
    lv_obj_set_style_text_font(start_label, &lv_font_montserrat_24, 0);
    lv_obj_center(start_label);
}

static void cut_line_init(void)
{
    static lv_point_t cut_line_point[] = {{680, 190}, {1020, 190}};
    static lv_style_t cut_line_style;

    lv_style_init(&cut_line_style);
    lv_style_set_line_width(&cut_line_style, 2);
    lv_style_set_line_color(&cut_line_style, lv_palette_main(LV_PALETTE_GREY));
    lv_style_set_line_opa(&cut_line_style, LV_OPA_20);
    lv_style_set_line_rounded(&cut_line_style, false);

    static lv_obj_t *cut_line;
    cut_line = lv_line_create(lv_scr_act());
    lv_line_set_points(cut_line, cut_line_point, 2);
    lv_obj_add_style(cut_line, &cut_line_style, 0);
    lv_obj_set_align(cut_line, LV_ALIGN_TOP_LEFT);
}

static void set_arc_value(void *obj, int32_t v)
{
    lv_arc_set_value(obj, v);
}

static void arc_init(void)
{
    lv_style_init(&arc_style_main);
    lv_style_set_arc_width(&arc_style_main, 20);
    lv_style_set_arc_color(&arc_style_main, lv_palette_main(LV_PALETTE_GREY));
    lv_style_set_arc_opa(&arc_style_main, LV_OPA_20);

    lv_style_init(&arc_style_indicate);
    lv_style_set_arc_width(&arc_style_indicate, 20);
    lv_style_set_arc_color(&arc_style_indicate, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_arc_opa(&arc_style_indicate, LV_OPA_COVER);

    lv_style_init(&arc_style_pass_indicate);
    lv_style_set_arc_width(&arc_style_pass_indicate, 20);
    lv_style_set_arc_color(&arc_style_pass_indicate, lv_palette_main(LV_PALETTE_GREEN));
    lv_style_set_arc_opa(&arc_style_pass_indicate, LV_OPA_COVER);

    lv_style_init(&arc_style_err_indicate);
    lv_style_set_arc_width(&arc_style_err_indicate, 20);
    lv_style_set_arc_color(&arc_style_err_indicate, lv_palette_main(LV_PALETTE_RED));
    lv_style_set_arc_opa(&arc_style_err_indicate, LV_OPA_COVER);

    arc = lv_arc_create(lv_scr_act());
    lv_obj_add_style(arc, &arc_style_main, LV_PART_MAIN);
    lv_obj_add_style(arc, &arc_style_indicate, LV_PART_INDICATOR);

    lv_arc_set_rotation(arc, 270);
    lv_arc_set_bg_angles(arc, 0, 360);

    lv_obj_remove_style(arc, NULL, LV_PART_KNOB);   /*Be sure the knob is not displayed*/
    lv_obj_clear_flag(arc, LV_OBJ_FLAG_CLICKABLE);  /*To not allow adjusting by click*/
    lv_obj_set_pos(arc, 705, 250);
    lv_obj_set_size(arc, 295, 350);
    lv_obj_set_align(arc, LV_ALIGN_TOP_LEFT);

    lv_obj_set_style_anim_time(arc, 1000, 0);
    lv_arc_set_mode(arc, LV_ARC_MODE_NORMAL);

    lv_arc_set_range(arc, 0, 100);

    lv_anim_init(&arc_anim);
    lv_anim_set_var(&arc_anim, arc);
    lv_anim_set_exec_cb(&arc_anim, set_arc_value);
    lv_anim_set_time(&arc_anim, 1000);

    arc_label = lv_label_create(arc);
    lv_label_set_recolor(arc_label, true);
    lv_label_set_text(arc_label, "#2196f3 Testing#"); /* main blue */
    lv_obj_set_style_text_font(arc_label, &lv_font_montserrat_24, 0);
    lv_obj_set_size(arc_label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_pos(arc_label, 0, -25);
    lv_obj_set_align(arc_label, LV_ALIGN_CENTER);
    lv_obj_add_flag(arc_label, LV_OBJ_FLAG_HIDDEN);
}

static void mgr_result_label_init()
{
    char mgr_result_context[100];
    mgr_result_label = lv_label_create(lv_scr_act());
    snprintf(mgr_result_context, sizeof(mgr_result_context),
            "Passed items: %s                       Failed items: %s",
            "--", "--");

    lv_label_set_text(mgr_result_label, mgr_result_context);
    lv_obj_set_pos(mgr_result_label, 690, 170);
}

void lv_qc_test_init(void)
{
    static rt_uint8_t qc_task_thread_stack[QC_TASK_THREAD_STACK_SIZE];
    qc_ops_create();

    /* set the active layer background to white */
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_white(), 0);

    /* set header title */
    title_init();

    table_init();

    start_btn_init();

    mgr_result_label_init();

    cut_line_init();

    arc_init();

    lv_timer_t *sys_timer = lv_timer_create(sys_time_update, 1000, NULL);
    lv_timer_ready(sys_timer);

    lv_timer_t *draw_timer = lv_timer_create(qc_draw_task, 50, NULL);
    lv_timer_ready(draw_timer);

    mgr_mutex = rt_mutex_create("qc_mutex", RT_IPC_FLAG_PRIO);
    if (mgr_mutex == NULL) {
        printf("rt mutex create failed\n");
    }

    rt_err_t err;
    err = rt_thread_init(&qc_task_thread, "qc_task",
                                qc_exec_task, (void *)NULL,
                                &qc_task_thread_stack[0], sizeof(qc_task_thread_stack),
                                LPKG_LVGL_THREAD_PRIO, 0);
    if (err != RT_EOK) {
        printf("failed to create qc task thread");
        return;
    }
    (void)err;
    rt_thread_startup(&qc_task_thread);
}
#endif
