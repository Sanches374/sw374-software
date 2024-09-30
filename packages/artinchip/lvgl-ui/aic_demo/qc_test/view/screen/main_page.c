/*
 * Copyright (c) 2022-2023, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors:  Zequan Liang <zequan.liang@artinchip.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#if QC_OS_LINUX
#include <pthread.h>
#endif
#if QC_OS_RT_THREAD
#include <rtconfig.h>
#include <rtthread.h>
#include <rtdevice.h>
#endif

#include "lvgl/lvgl.h"
#include "aic_ui.h"

#include "../../module/qc_test.h"

LV_FONT_DECLARE(lv_font_montserrat_24_cp);

static void mgr_lock(void);
static void mgr_un_lock(void);
static void qc_mdelay(int delay_ms);

static int qc_ops_create(void);

static int module_name_to_table_row(char *name);
static void table_cell_update(qc_disp_data_t *disp);
static void mgr_result_label_update(qc_disp_data_t *disp);
static void arc_update(qc_disp_data_t *disp);
static void start_btn_update(int progress);

static void table_init(void);
static void title_init(void);
static void start_btn_init(void);
static void cut_line_init(void);
static void progress_arc_init(void);
static void mgr_result_label_init();
static void top_windows_create(lv_obj_t *parent);

static void table_status_clear(void);
static void arc_status_clear(void);
static void mgr_result_label_clear(void);

static void draw_table_event_cb(lv_event_t * e);
static void reverse_btn_status_cb(lv_timer_t *lv_timer);
static void start_btn_handler(lv_event_t * e);
static void del_top_window_cb(lv_event_t * e);
static void get_chip_intel_model_cb(lv_event_t * e);
static void create_top_window_cb(lv_event_t * e);

static void disable_unused_obj(void);

static void sys_time_update(lv_timer_t *lv_timer);

#if QC_OS_RT_THREAD || QC_OS_LINUX
static void qc_exec_task(void *arg);
#else
static void qc_exec_task(lv_timer_t *lv_timer);
#endif
static void qc_draw_task(lv_timer_t *lv_timer);

#if QC_OS_LINUX
static pthread_mutex_t *mgr_mutex;
#elif QC_OS_RT_THREAD
#define QC_TASK_THREAD_STACK_SIZE 4096
static rt_mutex_t mgr_mutex = NULL;
static struct rt_thread qc_task_thread;
static rt_uint8_t qc_task_thread_stack[QC_TASK_THREAD_STACK_SIZE];
#else
static lv_timer_t *exec_timer;
#endif

static qc_board_config_t *board;
static qc_manager_t *mgr;

static lv_obj_t *start_label;
static lv_obj_t *start_btn;
static lv_obj_t *choose_btn;

static lv_style_t arc_style_main;
static lv_style_t arc_style_indicate;
static lv_style_t arc_style_pass_indicate;
static lv_style_t arc_style_err_indicate;
static lv_obj_t *arc_label;
static lv_obj_t *arc;
static lv_anim_t arc_anim;

static lv_obj_t * table;
static lv_obj_t *mgr_result_label;

static lv_obj_t * rtc_time;
static lv_timer_t *sys_timer;

void lv_qc_test_init(void)
{
    if (qc_ops_create() == QC_RES_INV)
        return;

    /* set the active layer background to white */
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_white(), 0);

    lv_obj_clear_flag(lv_scr_act(), LV_OBJ_FLAG_SCROLLABLE);
    /* set header title */
    title_init();

    table_init();

    start_btn_init();

    mgr_result_label_init();

    cut_line_init();

    progress_arc_init();

    sys_timer = lv_timer_create(sys_time_update, 1000, NULL);
    lv_timer_ready(sys_timer);

    lv_timer_t *draw_timer = lv_timer_create(qc_draw_task, 50, NULL);
    lv_timer_ready(draw_timer);

    lv_timer_create(reverse_btn_status_cb, 100, NULL);

#if QC_OS_RT_THREAD
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
#elif QC_OS_LINUX
    pthread_t task_thread;
    pthread_create(&task_thread, NULL, qc_exec_task, NULL);
    mgr_mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    if (mgr_mutex == NULL) {
        printf("pthread mutex create failed\n");
    }
    pthread_mutex_init(mgr_mutex, NULL);
#else
    exec_timer = lv_timer_create(qc_exec_task, 300, NULL);
#endif

    disable_unused_obj();
}

#if QC_OS_LINUX
static void mgr_lock(void)
{
    if (mgr_mutex) {
        pthread_mutex_lock(mgr_mutex);
    }
}

static void mgr_un_lock(void)
{
    if (mgr_mutex) {
        pthread_mutex_unlock(mgr_mutex);
    }
}

static void qc_mdelay(int delay_ms)
{
    usleep(delay_ms * 1000);
}
#elif QC_OS_RT_THREAD
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
static void qc_mdelay(int delay_ms)
{
    rt_thread_mdelay(delay_ms);
}
#else
static void mgr_lock(void) {;}
static void mgr_un_lock(void) {;}
static void qc_mdelay(int delay_ms) {;}
#endif

static int qc_ops_create(void)
{
    board = qc_board_tests_create();
    if (board == NULL) {
        printf("qc_board_tests_create failed\n");
        return QC_RES_INV;
    }

    mgr = board->manager;

    qc_manager_set_lock(mgr, mgr_lock, mgr_un_lock);

    return QC_RES_OK;
}

static int module_name_to_table_row(char *name)
{
    int num_modules = mgr->module_num;
    static qc_module_t *temp_module = NULL;
    if (temp_module == NULL)
        temp_module = qc_module_create();

    for (int i = 0; i < num_modules; i++) {
        if (qc_manager_module_get_index(mgr, temp_module, i) == QC_RES_INV)
            return -1;

        if (strncmp(name, temp_module->name, strlen(name)) == 0) {
            return i + 1;
        }
    }

    return -1;
}

static void table_cell_update(qc_disp_data_t *disp)
{
    int table_row_pos = -1;
    char failure_item[128] =  {0};

    table_row_pos = module_name_to_table_row(disp->mod_name);
    if (table_row_pos > 0) {
        if (disp->mod_status == QC_MOD_EXECUTING) {
            lv_table_set_cell_value(table, table_row_pos, 3, "Testing");
        } else if (disp->mod_status == QC_MOD_SUCCESS) {
            lv_table_set_cell_value(table, table_row_pos, 3, "Success");
        } else if (disp->mod_status == QC_MOD_FAILURE) {
            lv_table_set_cell_value(table, table_row_pos, 3, "Failure");
        }

        lv_table_set_cell_value(table, table_row_pos, 1, disp->mod_desc);

        for (int i = 0; i < LIST_OPS_NUM; i++) {
            if (disp->list_ops_status[i] == QC_MOD_FAILURE) {
                strcat(failure_item, disp->list_ops_desc[i]);
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

static void mgr_result_label_update(qc_disp_data_t *disp)
{
    char mgr_result_context[100] = {0};

    if (disp->mgr_success_num != 0 || disp->mgr_failure_num != 0) {
        snprintf(mgr_result_context, sizeof(mgr_result_context),
            "Passed items: %d                       Failed items: %d",
            disp->mgr_success_num, disp->mgr_failure_num);
        lv_label_set_text(mgr_result_label, mgr_result_context);
    }
}

static void arc_update(qc_disp_data_t *disp)
{
    static int last_progress = 0;

    lv_anim_set_values(&arc_anim, last_progress, disp->mgr_progress);
    lv_anim_start(&arc_anim);

    last_progress = disp->mgr_progress;

    if (lv_obj_has_flag(arc_label, LV_OBJ_FLAG_HIDDEN))
        lv_obj_clear_flag(arc_label, LV_OBJ_FLAG_HIDDEN);

    if (disp->mgr_progress == 100) {
        if (disp->mgr_failure_num != 0) {
            lv_obj_add_style(arc, &arc_style_err_indicate,
                                     LV_PART_INDICATOR);
            lv_anim_set_values(&arc_anim, disp->mgr_progress, disp->mgr_progress);
            lv_anim_start(&arc_anim);

            lv_label_set_text(arc_label, "#f44336 Failure #"); /* main red */
        } else {
            lv_obj_add_style(arc, &arc_style_pass_indicate,
                                     LV_PART_INDICATOR);
            lv_anim_set_values(&arc_anim, disp->mgr_progress, disp->mgr_progress);
            lv_anim_start(&arc_anim);

            lv_label_set_text(arc_label, "#4caf50 Success #"); /* main gree */
        }
    } else if (disp->mgr_progress > 0 && disp->mgr_progress < 100) {
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

static void title_init(void)
{
    lv_obj_t * top_bg = lv_obj_create(lv_scr_act());
    lv_obj_set_style_bg_color(top_bg, lv_palette_darken(LV_PALETTE_BLUE, 2), 0);
    lv_obj_set_pos(top_bg, 0, 0);
    lv_obj_set_size(top_bg, 1024, 54);
    lv_obj_clear_flag(top_bg, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t * title = lv_label_create(top_bg);
    lv_obj_set_style_text_color(title, lv_palette_lighten(LV_PALETTE_BLUE, 5), 0);
    lv_label_set_text_fmt(title, "ArtInChip QC Test Tool %s", QC_TEST_TOOL_VERSION);
    lv_obj_set_align(title, LV_ALIGN_CENTER);

    lv_obj_t * chip_version = lv_label_create(top_bg);
    lv_obj_set_style_text_color(chip_version, lv_palette_lighten(LV_PALETTE_BLUE, 5), 0);

    lv_label_set_text(chip_version, board->board_describe);
    lv_obj_set_align(chip_version, LV_ALIGN_TOP_LEFT);

    rtc_time = lv_label_create(top_bg);
    lv_obj_set_style_text_color(rtc_time,  lv_palette_lighten(LV_PALETTE_BLUE, 5), 0);
    lv_obj_set_align(rtc_time, LV_ALIGN_RIGHT_MID);
    lv_label_set_text(rtc_time, " ");
}

static void table_init(void)
{
    qc_module_t *temp_module = {0};
    int table_row_pos = {0};
    int table_row_cnt = mgr->module_num;

    table = lv_table_create(lv_scr_act());
    lv_obj_set_scroll_dir(table, LV_DIR_TOP | LV_DIR_BOTTOM | LV_DIR_VER);
    lv_obj_set_scrollbar_mode(table, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_pos(table, 0, 54);
    lv_obj_set_size(table, 680, 544);
    lv_table_set_row_cnt(table, table_row_cnt);
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

    for (int i = 0; i < table_row_cnt; i++) {
        qc_res_t ret = qc_manager_module_get_index(mgr, temp_module, i);
        if (ret == QC_RES_OK) {
            table_row_pos = i + 1;
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

static void start_btn_init(void)
{
    static lv_style_t start_btn_style;

    start_btn = lv_btn_create(lv_scr_act());
    lv_obj_clear_flag(start_btn, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_pos(start_btn, 690, 60);
    lv_obj_set_size(start_btn, 323, 83);

    lv_obj_add_event_cb(start_btn, start_btn_handler, LV_EVENT_ALL, NULL);
    lv_style_init(&start_btn_style);
    lv_style_set_bg_color(&start_btn_style, lv_palette_main(LV_PALETTE_BLUE));
    lv_obj_add_style(start_btn, &start_btn_style, 0);

    start_label = lv_label_create(start_btn);
    lv_obj_clear_flag(start_label, LV_OBJ_FLAG_SCROLLABLE);
    lv_label_set_text(start_label, "Start");
    lv_obj_set_style_text_font(start_label, &lv_font_montserrat_24_cp, 0);
    lv_obj_center(start_label);

    choose_btn = lv_btn_create(lv_scr_act());
    lv_obj_clear_flag(choose_btn, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_pos(choose_btn, 690, 150);
    lv_obj_set_size(choose_btn, 323, 83);

    lv_obj_t *choose_label = lv_label_create(choose_btn);
    lv_obj_clear_flag(choose_label, LV_OBJ_FLAG_SCROLLABLE);
    lv_label_set_text(choose_label, "Choose Model");
    lv_obj_set_style_text_font(choose_label, &lv_font_montserrat_24_cp, 0);
    lv_obj_center(choose_label);

    lv_obj_add_event_cb(start_btn, start_btn_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(choose_btn, create_top_window_cb, LV_EVENT_ALL, NULL);
}

static void cut_line_init(void)
{
    static lv_point_t cut_line_point[] = {{680, 245}, {1020, 245}};
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

static void progress_arc_init(void)
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
    lv_obj_set_pos(arc, 705, 290);
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
    lv_label_set_text(arc_label, " "); /* main blue */
    lv_obj_set_style_text_font(arc_label, &lv_font_montserrat_24_cp, 0);
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
    lv_obj_set_pos(mgr_result_label, 690, 250);
}

static void top_windows_create(lv_obj_t *parent)
{
    lv_obj_t *mask = lv_obj_create(lv_layer_top());
    lv_obj_remove_style_all(mask);
    lv_obj_set_size(mask, 1024, 600);
    lv_obj_set_style_bg_color(mask, lv_palette_main(LV_PALETTE_GREY), 0);
    lv_obj_set_style_bg_opa(mask, LV_OPA_50, 0);

    lv_obj_t *windows = lv_obj_create(mask);
    lv_obj_clear_flag(windows, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(windows, 480, 300);
    lv_obj_set_style_clip_corner(windows, 40, 0);
    lv_obj_set_style_bg_color(windows, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_bg_opa(windows, LV_OPA_100, 0);
    lv_obj_set_style_border_color(windows, lv_palette_main(LV_PALETTE_GREY), 0);
    lv_obj_set_style_border_width(windows, 2, 0);
    lv_obj_center(windows);

    lv_obj_t *title = lv_label_create(windows);
    lv_label_set_text(title, "Choose Chip Model");
    lv_obj_set_size(title, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_align(title, LV_ALIGN_TOP_MID);
    lv_obj_set_pos(title, 0, 10);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_24_cp, 0);
    lv_label_set_recolor(title, true);
    lv_obj_set_style_text_color(title, lv_color_hex(0x393939), 0);

    lv_obj_t *prompt_desc = lv_label_create(windows);
    lv_obj_set_size(prompt_desc, 200, 53);
    lv_label_set_long_mode(prompt_desc, LV_LABEL_LONG_WRAP);
    lv_label_set_text(prompt_desc, "Please select the internal model of the current test chip.");
    lv_obj_set_align(prompt_desc, LV_ALIGN_TOP_LEFT);
    lv_obj_set_pos(prompt_desc, 13, 53);
    lv_label_set_recolor(prompt_desc, true);

    lv_obj_t *chip_model_desc = lv_label_create(windows);
    lv_obj_set_size(chip_model_desc, 200, 53);
    lv_label_set_long_mode(chip_model_desc, LV_LABEL_LONG_WRAP);
    lv_label_set_text_fmt(chip_model_desc, "Please select the internal model of the current #76CB1D %s. #", board->chip_intel_module);
    lv_obj_set_align(chip_model_desc, LV_ALIGN_TOP_LEFT);
    lv_obj_set_pos(chip_model_desc, 13, 120);
    lv_label_set_recolor(chip_model_desc, true);

    lv_obj_t *btn = lv_btn_create(windows);
    lv_obj_set_size(btn, 150, 67);
    lv_obj_set_pos(btn, 13, 191);

    lv_obj_t *btn_label = lv_label_create(btn);
    lv_label_set_text(btn_label, "Apply");
    lv_obj_set_align(btn_label, LV_ALIGN_CENTER);
    lv_obj_set_style_text_font(btn_label, &lv_font_montserrat_24_cp, 0);

    char roller_text[256] = {0};
    for (int i = 0; i < board->chip_intel_module_list_num; i++) {
        strncat(roller_text, board->chip_intel_module_list[i], sizeof(roller_text) - 1);
        if (i != board->chip_intel_module_list_num - 1)
            strncat(roller_text, "\n", sizeof(roller_text) - 1);
    }
    lv_obj_t *roller = lv_roller_create(windows);
    lv_roller_set_options(roller, roller_text, LV_ROLLER_MODE_NORMAL);
    lv_obj_set_size(roller, 190, 205);
    lv_obj_set_pos(roller, 250, 53);
    int set_select = 0;
    for (int i = 0; i < board->chip_intel_module_list_num; i++) {
        if (strncmp(board->chip_intel_module, board->chip_intel_module_list[i], DESC_LEN) == 0)
            set_select = i;
    }
    lv_roller_set_selected(roller, set_select, LV_ANIM_OFF);

    lv_obj_add_event_cb(roller, get_chip_intel_model_cb, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(btn, del_top_window_cb, LV_EVENT_ALL, mask);

    lv_event_send(roller, LV_EVENT_VALUE_CHANGED, NULL);
}

static void sys_time_disabled(void)
{
    lv_obj_add_flag(rtc_time, LV_OBJ_FLAG_HIDDEN);
    lv_timer_pause(sys_timer);
}

static void chip_select_disable(void)
{
    lv_obj_clear_flag(choose_btn, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_bg_color(choose_btn, lv_palette_lighten(LV_PALETTE_GREY, 2), 0);
}

static void disable_unused_obj(void)
{
    qc_module_t *module = qc_module_create();
    if (qc_manager_module_get(mgr, module, "RTC") == QC_RES_INV) {
        sys_time_disabled();
    }

    if (qc_manager_module_get(mgr, module, "SID") == QC_RES_INV) {
        chip_select_disable();
    }
    qc_module_delete(module);
}

static void sys_time_update(lv_timer_t *lv_timer)
{
    time_t now;
    struct tm *local_time;

    now = time(NULL);
    local_time = localtime(&now);

    lv_label_set_text_fmt(rtc_time, "%04d-%02d-%02d  %02d:%02d:%02d", local_time->tm_year+1900, local_time->tm_mon+1, local_time->tm_mday, local_time->tm_hour, local_time->tm_min, local_time->tm_sec);
}

static void reverse_btn_status_cb(lv_timer_t *lv_timer)
{
    if (mgr->progress == 0 || mgr->progress == 100) {
        if (lv_obj_has_flag(choose_btn, LV_OBJ_FLAG_CLICKABLE) == false) {
            lv_obj_add_flag(choose_btn, LV_OBJ_FLAG_CLICKABLE);
            lv_obj_set_style_bg_color(choose_btn, lv_palette_lighten(LV_PALETTE_BLUE, 1), 0);
        }

        if (lv_obj_has_flag(start_btn, LV_OBJ_FLAG_CLICKABLE) == false) {
            lv_obj_add_flag(start_btn, LV_OBJ_FLAG_CLICKABLE);
            lv_obj_set_style_bg_color(start_btn, lv_palette_lighten(LV_PALETTE_BLUE, 1), 0);
        }
    } else {
        if (lv_obj_has_flag(choose_btn, LV_OBJ_FLAG_CLICKABLE) == true) {
            lv_obj_clear_flag(choose_btn, LV_OBJ_FLAG_CLICKABLE);
            lv_obj_set_style_bg_color(choose_btn, lv_palette_lighten(LV_PALETTE_GREY, 2), 0);
        }

        if (lv_obj_has_flag(start_btn, LV_OBJ_FLAG_CLICKABLE) == true) {
            lv_obj_clear_flag(start_btn, LV_OBJ_FLAG_CLICKABLE);
            lv_obj_set_style_bg_color(start_btn, lv_palette_lighten(LV_PALETTE_GREY, 2), 0);
        }
    }
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

static void start_btn_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_PRESSED) {
        if (mgr->exec_num == 0 && mgr->progress == 100) {
            table_status_clear();
            arc_status_clear();
            mgr_result_label_clear();
            qc_board_ops_clear(board);
            qc_board_ops_push(board);
        } else if (mgr->exec_num == 0 && mgr->progress == 0) {
            qc_board_ops_push(board);
        }
    }
}


static void del_top_window_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *mask = lv_event_get_user_data(e);
    char sid_desc[256]= {0};

    if(code == LV_EVENT_CLICKED) {
        qc_module_t *tmp = qc_module_create();
        qc_disp_data_t *disp = qc_disp_data_create();

        lv_obj_del(mask);

        qc_write_chip_intel_module_file(board->chip_intel_module);

        qc_manager_module_get(mgr, tmp, "SID");
        snprintf(sid_desc, sizeof(sid_desc) - 1, "%s R comparison", board->chip_intel_module);
        qc_module_set_desc(tmp, sid_desc);
        qc_manager_module_sync(mgr, tmp);

        qc_disp_data_set(disp, mgr, tmp);
        disp->mod_status = QC_MOD_UNEXECUTED;
        qc_disp_data_append(disp, mgr);

        qc_disp_data_delete(disp);
        qc_module_delete(tmp);
    }
}

static void get_chip_intel_model_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *roller = lv_event_get_target(e);
    char intel_model[DESC_LEN - 1] = {0};

    if(code == LV_EVENT_VALUE_CHANGED) {
        lv_roller_get_selected_str(roller, intel_model, sizeof(intel_model) - 1);
        strncpy(board->chip_intel_module, intel_model, DESC_LEN - 1);
    }
}

static void create_top_window_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_CLICKED) {
        top_windows_create(lv_layer_top());
    }
}

static qc_res_t is_the_final_module(qc_exec_t *exec, qc_module_t *module)
{
    if (strncmp(exec->module_name, "SAVE", strlen("SAVE")) == 0) {
        return QC_RES_OK;
    }
    return QC_RES_INV;
}

static void assuming_the_final_test_is_successful(qc_exec_t *exec, qc_module_t *module)
{
    module->list_ops_status[exec->list_ops_pos] = QC_RES_OK;
    module->status = QC_MOD_SUCCESS;
    qc_manager_update(mgr, module);
    module->status = QC_MOD_UNEXECUTED;
}

static void the_final_test_special_handing(qc_module_t *module)
{
    /* execution result do not match expectations */
    if (module->status == QC_MOD_FAILURE) {
        qc_manager_lock(mgr);
        mgr->success_num -= 1;
        qc_manager_unlock(mgr);

        module->status = QC_MOD_EXECUTING;
        qc_manager_module_sync(mgr, module); /* forcefully overwrite the original test results */
        module->status = QC_MOD_FAILURE;
    }
}

#if QC_OS_RT_THREAD || QC_OS_LINUX
static void qc_exec_task(void *arg)
{
    qc_disp_data_t *disp = qc_disp_data_create();
    qc_exec_t *exec = qc_exec_create();
    qc_module_t *module = qc_module_create();

    while(1) {
        if (qc_exec_pop(exec, mgr) == QC_RES_OK) {
            int ret = qc_manager_module_get(mgr, module, exec->module_name);
            if (ret != QC_RES_OK) {
                continue;
            }

            /* pre final test successful */
            if (is_the_final_module(exec, module) == QC_RES_OK) {
                assuming_the_final_test_is_successful(exec, module);
            }

            qc_module_list_ops_exec(module, exec->list_ops_pos);
            if (module->list_ops_status[exec->list_ops_pos] == QC_MOD_EXECUTING) {
                continue;
            }

            if (is_the_final_module(exec, module) == QC_RES_OK) {
                the_final_test_special_handing(module);
            }

            qc_manager_update(mgr, module);
            qc_disp_data_set(disp, mgr, module);
            qc_disp_data_append(disp, mgr);
        }
        qc_mdelay(10);
    }

    qc_disp_data_delete(disp);
    qc_exec_delete(exec);
    qc_module_delete(module);
}
#else
static void qc_exec_task(lv_timer_t *lv_timer)
{
    static qc_disp_data_t *disp = NULL;
    static qc_exec_t *exec = NULL;
    static qc_module_t *module = NULL;
    static int init = 0;
    if (!init) {
        disp = qc_disp_data_create();
        exec = qc_exec_create();
        module = qc_module_create();
        init = 1;
    }

    if (qc_exec_pop(exec, mgr) == QC_RES_OK) {
        int ret = qc_manager_module_get(mgr, module, exec->module_name);
        if (ret != QC_RES_OK) {
            return;
        }

        /* pre final test successful */
        if (is_the_final_module(exec, module) == QC_RES_OK) {
            assuming_the_final_test_is_successful(exec, module);
        }

        qc_module_list_ops_exec(module, exec->list_ops_pos);
        if (module->list_ops_status[exec->list_ops_pos] == QC_MOD_EXECUTING) {
            return;
        }

        if (is_the_final_module(exec, module) == QC_RES_OK) {
            the_final_test_special_handing(module);
        }

        qc_manager_update(mgr, module);
        qc_disp_data_set(disp, mgr, module);
        qc_disp_data_append(disp, mgr);
    }
    qc_mdelay(10);
}
#endif

static void qc_draw_task(lv_timer_t *lv_timer)
{
    qc_disp_data_t draw_disp = {0};

    if (qc_disp_data_pop(&draw_disp, mgr) == QC_RES_OK) {
        table_cell_update(&draw_disp);
        mgr_result_label_update(&draw_disp);
        arc_update(&draw_disp);
        start_btn_update(draw_disp.mgr_progress);
    }
}
