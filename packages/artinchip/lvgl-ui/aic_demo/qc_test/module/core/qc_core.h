/*
 * Copyright (c) 2023-2024, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors:  Zequan Liang <zequan.liang@artinchip.com>
 */

#ifndef _QC_OPS_CORE_H_
#define _QC_OPS_CORE_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "qc_list.h"
#include "../qc_config.h"

#define NAME_LEN                    30
#define DESC_LEN                    30

#define LIST_OPS_NUM                8
#define OPS_NUM                     5
#define OPS_LEN                     100

enum {
    QC_RES_INV = -1,
    QC_RES_OK,
};
typedef int qc_res_t;

enum {
    QC_MOD_NOT_RECORD = 0,
    QC_MOD_RECORD,
};
typedef int qc_mod_record_t;

enum {
    QC_MOD_UNEXECUTED,
    QC_MOD_EXECUTING,
    QC_MOD_SUCCESS,
    QC_MOD_FAILURE,
};
typedef int qc_mod_status_t;

enum {
    QC_DEBUG_BASE = 1,
    QC_DEBUG_CORE = 2,
    QC_DEBUG_DETAIL= 4,
    QC_DEBUG_ALL =  7
};
typedef int qc_debug_t;

typedef qc_res_t (*qc_cap_cmp_int_t)(int);
typedef qc_res_t (*qc_cap_cmp_str_t)(char *);
typedef qc_res_t (*qc_list_ops_func_t)(void *);
typedef void (*qc_lock_t)(void);

typedef struct _qc_module
{
    /* base */
    unsigned int id;
    char name[NAME_LEN];
    char desc[DESC_LEN];
    qc_mod_status_t status;

    /* core */
    char list_ops_desc[LIST_OPS_NUM][DESC_LEN];
    char list_ops_cmd[LIST_OPS_NUM][OPS_NUM][OPS_LEN];
    qc_list_ops_func_t list_ops_func[LIST_OPS_NUM][OPS_NUM];
    void *list_ops_func_para[LIST_OPS_NUM][OPS_NUM];
    qc_cap_cmp_int_t list_ops_cmp_int[LIST_OPS_NUM][OPS_NUM]; /* in the form of a function, there is no need to use a comparision function */
    qc_cap_cmp_str_t list_ops_cmp_str[LIST_OPS_NUM][OPS_NUM];
    qc_mod_record_t list_ops_cmd_record[LIST_OPS_NUM][OPS_NUM];
    qc_mod_record_t list_ops_func_record[LIST_OPS_NUM][OPS_NUM];
    int list_ops_runtime[LIST_OPS_NUM]; /* only used for executing commands on linux */
    qc_mod_status_t list_ops_status[LIST_OPS_NUM];

    /* log relative */
    char *log_buf;
    int log_buf_len;
    void *reserve;

    struct qc_list list;
} qc_module_t;

typedef struct _qc_manager
{
    /* base */
    char name[NAME_LEN];
    int progress;
    int success_num;
    int failure_num;
    int success_rate;
    int module_num;
    int exec_num;
    int disp_num;
    void *reserve;

    /* list */
    struct qc_list mod_head;
    struct qc_list exec_head;
    struct qc_list disp_head;

    qc_lock_t lock;    /* the way to access a mutex in a blocking manager */
    qc_lock_t un_lock; /* the way to access a mutex in a blocking manager */
} qc_manager_t;

typedef struct _qc_exec_t
{
    char module_name[NAME_LEN];
    int list_ops_pos;

    struct qc_list list;
} qc_exec_t;

typedef struct _qc_disp_data_t
{
    /* module data */
    char mod_name[NAME_LEN];
    char mod_desc[DESC_LEN];
    qc_mod_status_t mod_status;
    qc_mod_status_t list_ops_status[LIST_OPS_NUM];
    char list_ops_desc[LIST_OPS_NUM][DESC_LEN];

    /* manager data */
    char mgr_name[NAME_LEN];
    int mgr_status;
    int mgr_progress;
    int mgr_success_num;
    int mgr_failure_num;
    int mgr_success_rate;

    struct qc_list list;
} qc_disp_data_t;

int qc_cmd_exec(char *command, char *buf, int buf_len, unsigned int runtime); /* capture output and return result */
int qc_cmd_exec_ex(char *command, char *buf,
                   int buf_len, unsigned int runtime,
                   qc_cap_cmp_str_t cmp_str, qc_res_t *cmd_res);
qc_module_t *qc_module_create(void);
qc_res_t qc_module_clear_status(qc_module_t *module);
qc_res_t qc_module_delete(qc_module_t *module);

qc_res_t qc_module_set_name(qc_module_t *module, char *name);
qc_res_t qc_module_set_desc(qc_module_t *module, char *desc);
qc_res_t qc_module_set_log(qc_module_t *module, char *log_buf, int buf_len);
qc_res_t qc_module_list_ops_set_desc(qc_module_t *module, int list_pos, char *list_desc);
qc_res_t qc_module_list_ops_set_runtime(qc_module_t *module, int list_ops, int runtime); /* unit: ms, after will exec */
qc_res_t qc_module_list_ops_cmd_append(qc_module_t *module, int list_pos, char *cmd,
                                        qc_mod_record_t ops_record, qc_cap_cmp_int_t int_cmp, qc_cap_cmp_str_t str_cmp);
qc_res_t qc_module_list_ops_func_append(qc_module_t *module, int list_ops, qc_list_ops_func_t ops, void *para,
                                        qc_mod_record_t ops_record, qc_cap_cmp_int_t int_cmp, qc_cap_cmp_str_t str_cmp);
qc_res_t qc_module_list_ops_exec(qc_module_t *module, int list_ops); /* execute in a blocking manner */

qc_manager_t *qc_manager_create(void);
qc_res_t qc_manager_clear_status(qc_manager_t *manager);
qc_res_t qc_manager_delete(qc_manager_t *manager);
qc_res_t qc_manager_set_lock(qc_manager_t *manager, qc_lock_t lock, qc_lock_t un_lock);
qc_res_t qc_manager_lock(qc_manager_t *manager);
qc_res_t qc_manager_unlock(qc_manager_t *manager);

qc_res_t qc_manager_module_append(qc_manager_t *manager, qc_module_t *module);
qc_res_t qc_manager_module_get(qc_manager_t *manager, qc_module_t *back, char *name);
qc_res_t qc_manager_module_get_index(qc_manager_t *manager, qc_module_t *back, int index); /* search by index, 0 is the first to be added */
qc_res_t qc_manager_module_del(qc_manager_t *manager, char *name);
qc_res_t qc_manager_module_sync(qc_manager_t *manager, qc_module_t *module);
qc_res_t qc_manager_update(qc_manager_t *manager, qc_module_t *module);

qc_exec_t *qc_exec_create(void);
qc_res_t qc_exec_delete(qc_exec_t *exec);
qc_res_t qc_exec_set(qc_exec_t *exec, char *mod_name, int list_ops_pos);
qc_res_t qc_exec_append(qc_exec_t *exec, qc_manager_t *mgr);
qc_res_t qc_exec_append_left(qc_exec_t *exec, qc_manager_t *mgr);
qc_res_t qc_exec_pop(qc_exec_t *exec, qc_manager_t *mgr);
qc_res_t qc_exec_ops_set_append(qc_manager_t *mgr, char *mod_name, int list);

qc_disp_data_t *qc_disp_data_create(void);
qc_res_t qc_disp_data_delete(qc_disp_data_t *disp_data);
qc_res_t qc_disp_data_set(qc_disp_data_t *disp_data, qc_manager_t *manager, qc_module_t *module);
qc_res_t qc_disp_data_append(qc_disp_data_t *disp_data, qc_manager_t *manager);
qc_res_t qc_disp_data_append_left(qc_disp_data_t *disp_data, qc_manager_t *manager);
qc_res_t qc_disp_data_pop(qc_disp_data_t *disp_data, qc_manager_t *manager);
qc_res_t qc_disp_data_set_append(qc_disp_data_t *disp_data, qc_manager_t *manager, qc_module_t *module);

void qc_module_debug(qc_module_t *module, qc_debug_t flag);
void qc_manager_debug(qc_manager_t *manager, qc_debug_t flag);
void qc_exec_debug(qc_exec_t *exec, qc_debug_t debug_flag);
void qc_disp_data_debug(qc_disp_data_t *disp, qc_debug_t debug_flag);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _QC_OPS_CORE_H_ */
