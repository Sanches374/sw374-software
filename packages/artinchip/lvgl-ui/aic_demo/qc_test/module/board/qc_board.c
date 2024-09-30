/*
 * Copyright (c) 2023-2024, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors:  Zequan Liang <zequan.liang@artinchip.com>
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "qc_board.h"

static qc_res_t qc_board_get_chip_id(char *buf, int buf_len);
static qc_module_t *name_to_module(qc_module_t *module_list[], int module_list_num, char *name);
static void _qc_board_env_init(qc_env_init_list_t *list_env);
static qc_board_config_t* _qc_board_tests_create(char *board_describe,
                                          char *chip_intel_list[],
                                          qc_mod_init_base_t *base,
                                          qc_mod_init_list_pos_t *list_pos);
static qc_res_t _qc_board_ops_push(qc_board_config_t* board, qc_exec_ops_order_init_t *order);

extern char *board_script;
extern char *board_chip_intel_list[];
extern qc_env_init_list_t board_list_env[];
extern qc_mod_init_base_t board_init_base[];
extern qc_mod_init_list_pos_t board_init_list_pos[];
extern qc_exec_ops_order_init_t board_exec_order[];

qc_board_config_t* qc_board_tests_create(void)
{
    _qc_board_env_init(board_list_env);
    return _qc_board_tests_create(board_script, board_chip_intel_list, board_init_base, board_init_list_pos);
}

qc_res_t qc_board_ops_push(qc_board_config_t* board)
{
    return _qc_board_ops_push(board, board_exec_order);
}

qc_res_t qc_board_ops_clear(qc_board_config_t* board)
{
    return qc_manager_clear_status(board->manager);
}

static qc_module_t *name_to_module(qc_module_t *module_list[], int module_list_num, char *name)
{
    for (int i = 0; i < module_list_num; i++) {
       if (strncmp(name, module_list[i]->name, strlen(name)) == 0) {
           return module_list[i];
       }
    }
    return NULL;
}

static qc_board_config_t* _qc_board_tests_create(char *board_describe,
                                          char *chip_intel_list[],
                                          qc_mod_init_base_t *base,
                                          qc_mod_init_list_pos_t *list_pos)
{
    int i = 0;

    qc_board_config_t* board = NULL;
    board = (qc_board_config_t *)malloc(sizeof(qc_board_config_t));
    if (board == NULL) {
        return NULL;
    }
    memset(board, 0, sizeof(qc_board_config_t));

    int module_num = 0;
    while (1) {
        if (base[module_num].name != NULL)
            module_num++;
        else
            break;
    }
    qc_module_t **module_list = (qc_module_t **)malloc(sizeof(qc_module_t) * module_num);
    if (module_list == NULL)
        return NULL;
    memset(module_list, 0, sizeof(qc_module_t) * module_num);

    qc_manager_t *mgr = qc_manager_create();
    if (mgr == NULL) {
        free(board);
        free(module_list);
        return NULL;
    }

    /* init chip id */
    qc_board_get_chip_id(board->chip_id, 64);

    /* init chip intel module */
    int chip_list_num = 0;
    while (1) {
        if (chip_intel_list[chip_list_num] != NULL)
            chip_list_num++;
        else
            break;
    }
    for (i = 0; i < chip_list_num; i++) {
        strncpy(board->chip_intel_module_list[i], chip_intel_list[i], DESC_LEN - 1);
        board->chip_intel_module_list_num++;
    }
    if (qc_read_chip_intel_model_file(board->chip_intel_module, DESC_LEN - 1) == QC_RES_INV) {
        strncpy(board->chip_intel_module, chip_intel_list[0], DESC_LEN - 1);
        qc_write_chip_intel_module_file(board->chip_intel_module);
    }

    /* init base */
    for (i = 0; i < module_num; i++) {
        module_list[i] = qc_module_create();
        qc_module_set_name(module_list[i], base[i].name);
        qc_module_set_desc(module_list[i], base[i].desc);
        qc_module_set_log(module_list[i], base[i].log_buf, base[i].log_buf_len);
        if ((strncmp(list_pos[i].name, "SID", strlen("SID")) == 0)) {
            char sid_list_ops_desc[64] = {0};
            snprintf(sid_list_ops_desc, sizeof(sid_list_ops_desc), "%s R comparison", board->chip_intel_module);
            qc_module_set_desc(module_list[i], sid_list_ops_desc);
        }
    }

    /* init list pos */
    qc_module_t *mod = NULL;
    int list_pos_num = 0;
    while (1) {
        if (list_pos[list_pos_num].name != NULL)
            list_pos_num++;
        else
            break;
    }
    for (i = 0; i < list_pos_num; i++) {
        mod = name_to_module(module_list, module_num, list_pos[i].name);
        qc_module_list_ops_set_desc(mod, list_pos[i].list_pos, list_pos[i].list_ops_desc);
        qc_module_list_ops_cmd_append(mod, list_pos[i].list_pos, list_pos[i].list_ops_cmd, list_pos[i].list_ops_cmd_record,
                                        NULL, NULL);
        qc_module_list_ops_set_runtime(mod, list_pos[i].list_pos, list_pos[i].list_ops_runtime);
        /* qc_save special deal with */
        if (strncmp(list_pos[i].name, "SAVE", strlen("SAVE")) == 0 || strncmp(list_pos[i].name, "SID", strlen("SID")) == 0)
            qc_module_list_ops_func_append(mod, list_pos[i].list_pos, list_pos[i].list_ops_func, board,
                                            list_pos[i].list_ops_func_record, NULL, NULL);
        else
            qc_module_list_ops_func_append(mod, list_pos[i].list_pos, list_pos[i].list_ops_func, NULL,
                                        list_pos[i].list_ops_func_record, NULL, NULL);
    }

    for (i = 0; i < module_num; i++) {
        qc_manager_module_append(mgr, module_list[i]);
        qc_module_delete(module_list[i]);
    }

    board->board_describe = board_describe;
    board->manager = mgr;

    return board;
}

static void _qc_board_env_init(qc_env_init_list_t *list_env)
{
    int env_list_num = 0, i = 0;
    while (1) {
        if (list_env[env_list_num].cmd == NULL && list_env[env_list_num].func == NULL)
            break;
        else
            env_list_num++;
    }

    qc_list_ops_func_t func = NULL;
    for (i = 0; i < env_list_num; i++) {
        if (list_env[i].cmd != NULL)
            qc_cmd_exec(list_env[i].cmd, NULL, 0, list_env[i].cmd_runtime);
        if (list_env[i].func != NULL) {
            func = list_env[i].func;
            func(NULL);
        }
    }
}

static qc_res_t _qc_board_ops_push(qc_board_config_t* board, qc_exec_ops_order_init_t *order)
{
    int exec_order_num = 0;
    while (1) {
        if (order[exec_order_num].name != NULL)
            exec_order_num++;
        else
            break;
    }

    for (int i = 0; i < exec_order_num; i++) {
        qc_exec_ops_set_append(board->manager, order[i].name, order[i].order);
    }

    return QC_RES_OK;
}

static qc_res_t qc_board_get_chip_id(char *buf, int buf_len)
{
#if QC_OS_RT_THREAD
    extern qc_res_t qc_read_chip_id_rtos(char *buf, int buf_len);
    return qc_read_chip_id_rtos(buf, buf_len);
#endif

#if QC_OS_RT_LINUX
    extern qc_res_t qc_read_chip_id_linux(char *buf);
    return qc_read_chip_id_linux(buf, buf_len);
#endif

#if QC_OS_RT_THREAD == 0 && QC_OS_RT_LINUX == 0
    strncpy(buf, "XXX", buf_len);
    return QC_RES_OK;
#endif
    return QC_RES_OK;
}

void qc_board_debug(qc_board_config_t *board, qc_debug_t flag)
{
    if (board == NULL)
        return;
    printf("board describe =  %s\n", board->board_describe);
    if (flag & QC_DEBUG_BASE) {
        printf("chip id = %s, chip intel model = %s\n", board->chip_id, board->chip_intel_module);
    }
}
