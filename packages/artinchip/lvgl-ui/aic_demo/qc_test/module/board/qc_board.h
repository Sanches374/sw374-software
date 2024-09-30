/*
 * Copyright (c) 2023-2024, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors:  Zequan Liang <zequan.liang@artinchip.com>
 */

#ifndef _QC_BOARD_H_
#define _QC_BOARD_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "../qc_config.h"
#include "../core/qc_core.h"
#include "../misc/qc_misc.h"

typedef struct _qc_board_config
{
    char chip_id[64 + 4]; /* chip id fmt:44-80-ce-b8-52-2f-c0-0c-06-0c-11-18-09-00-30-04 */
    char chip_intel_module[DESC_LEN];
    char chip_intel_module_list[LIST_OPS_NUM][DESC_LEN];
    int chip_intel_module_list_num;

    char *board_describe;
    qc_manager_t *manager;
} qc_board_config_t;

typedef struct _qc_mod_init_base {
    char *name;
    char *desc;
    char *log_buf;
    int log_buf_len;
} qc_mod_init_base_t;

typedef struct _qc_mod_init_list_pos {
    char *name;
    char *list_ops_desc;
    int list_pos;
    char *list_ops_cmd;
    qc_list_ops_func_t list_ops_func;
    qc_mod_record_t list_ops_cmd_record;
    qc_mod_record_t list_ops_func_record;
    unsigned int list_ops_runtime;
} qc_mod_init_list_pos_t;

typedef struct _qc_exec_ops_order_init {
    char *name;
    int order;
} qc_exec_ops_order_init_t;

typedef struct _qc_env_init_list {
    char *cmd;
    qc_list_ops_func_t func;
    unsigned int cmd_runtime;
} qc_env_init_list_t;

enum {
    USB_DEV = 0,
    SDCARD_DEV,
};
typedef int qc_save_dev_t;

typedef struct _qc_save_init {
    const char *path;
    qc_save_dev_t dev;
    int blk_num;
} qc_save_init_t;

qc_board_config_t* qc_board_tests_create(void);
qc_res_t qc_board_ops_push(qc_board_config_t* board);
qc_res_t qc_board_ops_clear(qc_board_config_t* board);
void qc_board_tests_del(qc_board_config_t* board_cfg);
void qc_board_debug(qc_board_config_t *board, qc_debug_t flag);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _QC_BOARD_CFG_H_ */
