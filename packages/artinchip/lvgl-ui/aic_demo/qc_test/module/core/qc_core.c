/*
 * Copyright (c) 2023-2024, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors:  Zequan Liang <zequan.liang@artinchip.com>
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "qc_core.h"

#define TIMEOUT_MS_NULL    -65550

static int qc_module_find_free_list_ops_cmd(qc_module_t *module, int list_ops);
static int qc_module_find_free_list_ops_func(qc_module_t *module, int list_ops);
static int qc_module_get_list_ops_func_num(qc_module_t *module, int list_ops);
static int qc_module_get_list_ops_cmd_num(qc_module_t *module, int list_ops);
static void qc_module_init_func_res_list(qc_module_t *module, int list_ops, qc_res_t *list);
static void qc_module_init_cmd_res_list(qc_module_t *module, int list_ops, qc_res_t *list);
static qc_res_t qc_module_list_ops_exec_res(qc_res_t *list);
static qc_res_t qc_module_list_ops_exec_func(qc_module_t *module, int list_ops);
static qc_res_t qc_module_list_ops_exec_cmd(qc_module_t *module, int list_ops);
static void qc_module_list_ops_status_update(qc_module_t *module, int list_ops, qc_res_t func_res, qc_res_t cmd_res);
static void qc_module_get_list_ops_record(qc_module_t *module, qc_mod_record_t *list_ops_record);
static void qc_module_status_update(qc_module_t *module);

static void qc_mod_list_del(qc_manager_t *manager);
static void qc_exec_list_del(qc_manager_t *manager);
static void qc_disp_list_del(qc_manager_t *manager);
static qc_res_t qc_manager_check_include_module(qc_manager_t *manager, qc_module_t *module);

static qc_res_t qc_exec_add(qc_exec_t *exec, qc_manager_t *mgr, int append);
static qc_res_t qc_disp_data_add(qc_disp_data_t *disp_data, qc_manager_t *manager, int append);

static unsigned int module_id = 0;
static qc_module_t tmp_module = {0};

int qc_cmd_exec(char *command, char *buf, int buf_len, unsigned int runtime) /* capture output and return result */
{
#if QC_OS_RT_THREAD
    extern int qc_cmd_exec_rtos(char *command);
    return qc_cmd_exec_rtos(command);
#endif

#if QC_OS_LINUX
    extern int qc_cmd_exec_linux(char *command, char *buf, int buf_len, unsigned int runtime);
    return qc_cmd_exec_linux(module, buf, buf_len, runtime);
#endif
    return QC_RES_OK;
}

int qc_cmd_exec_ex(char *command, char *buf,
                   int buf_len, unsigned int runtime,
                   qc_cap_cmp_str_t cmp_str, qc_res_t *cmd_res)
{
#if QC_OS_RT_THREAD
    extern int qc_cmd_exec_rtos_ex(char *command);
    return qc_cmd_exec_rtos_ex(command);
#endif

#if QC_OS_LINUX
    extern int qc_cmd_exec_linux_ex(char *command, char *buf, int buf_len, unsigned int runtime, qc_cap_cmp_str_t cmp_str, qc_res_t *cmd_res);
    return qc_cmd_exec_linux_ex(module, buf, buf_len, runtime, cmp_str, cmd_res);
#endif

    return QC_RES_OK;
}

static int get_module_id(void)
{
    return ++module_id;
}

static int free_module_id(void)
{
    return --module_id;
}

static qc_res_t list_ops_cmp_int_default(int result)
{
    if (result == 0)
        return QC_RES_OK;

    return QC_RES_INV;
}

static qc_res_t list_ops_cmp_str_default(char *str)
{
    int i = 0;
    char lower_str[256] = {0};
    const char *keywords[] ={
        "success",
        "test ok"
    };

    for (i = 0; i < strlen(str) && i < sizeof(lower_str); i++) {
        lower_str[i] = tolower(str[i]);
    }

    for (i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++) {
        if (strstr(lower_str, keywords[i]) != NULL) {
            return QC_RES_OK;
        }
    }

    return QC_RES_INV;
}

qc_module_t *qc_module_create(void)
{
    int i = 0, j = 0;
    qc_module_t *module = NULL;

    module = (qc_module_t *)malloc(sizeof(qc_module_t));
    if (module == NULL) {
        printf("malloc module failed\n");
        return NULL;
    }
    memset(module, 0, sizeof(qc_module_t));

    module->id = get_module_id();
    module->status = QC_MOD_UNEXECUTED;

    for (i = 0; i < LIST_OPS_NUM; i++) {
        module->list_ops_status[i] = QC_MOD_UNEXECUTED;
        module->list_ops_runtime[i] = TIMEOUT_MS_NULL;
    }

    for (i = 0; i < LIST_OPS_NUM; i++) {
        for (j = 0; j < OPS_NUM; j++) {
            module->list_ops_cmd_record[i][j] = QC_MOD_NOT_RECORD;
            module->list_ops_func_record[i][j] = QC_MOD_NOT_RECORD;
            module->list_ops_cmp_int[i][j] = list_ops_cmp_int_default;
            module->list_ops_cmp_str[i][j] = list_ops_cmp_str_default;
        }
    }

    qc_list_init(&module->list);

    return module;
}

qc_res_t qc_module_clear_status(qc_module_t *module)
{
    int i;

    memset(module->log_buf, 0, module->log_buf_len);
    module->status = QC_MOD_UNEXECUTED;

    for (i = 0; i < LIST_OPS_NUM; i++) {
        module->list_ops_status[i] = QC_MOD_UNEXECUTED;
    }

    return QC_RES_OK;
}

qc_res_t qc_module_delete(qc_module_t *module)
{
    if (module)
        free(module);

    free_module_id();
    return QC_RES_OK;
}


qc_res_t qc_module_set_name(qc_module_t *module, char *name)
{
    if (module == NULL || name == NULL)
        return QC_RES_INV;

    strncpy(module->name, name, NAME_LEN -1);
    return QC_RES_OK;
}

qc_res_t qc_module_set_desc(qc_module_t *module, char *desc)
{
    if (module == NULL || desc == NULL)
        return QC_RES_INV;

    strncpy(module->desc, desc, DESC_LEN -1);
    return QC_RES_OK;
}

qc_res_t qc_module_set_log(qc_module_t *module, char *log_buf, int buf_len)
{
    if (module == NULL || log_buf == NULL || buf_len <= 0)
        return QC_RES_INV;

    module->log_buf = log_buf;
    module->log_buf_len = buf_len;
    return QC_RES_OK;
}

qc_res_t qc_module_list_ops_set_desc(qc_module_t *module, int list_ops, char *list_desc)
{
    if (module == NULL || list_ops < 0 || list_ops > LIST_OPS_NUM - 1 || list_desc == NULL)
        return QC_RES_INV;

    strncpy(module->list_ops_desc[list_ops], list_desc, DESC_LEN -1);
    return QC_RES_OK;
}

qc_res_t qc_module_list_ops_set_runtime(qc_module_t *module, int list_ops, int runtime)
{
    if (module == NULL || list_ops < 0 || list_ops > LIST_OPS_NUM - 1 || runtime <= 0)
        return QC_RES_INV;

    module->list_ops_runtime[list_ops] = runtime;
    return QC_RES_OK;
}

qc_res_t qc_module_list_ops_cmd_append(qc_module_t *module, int list_ops, char *cmd,
                                        qc_mod_record_t ops_record, qc_cap_cmp_int_t int_cmp, qc_cap_cmp_str_t str_cmp)
{
    if (module == NULL || list_ops < 0 || list_ops > LIST_OPS_NUM - 1 || cmd == NULL
       || (ops_record != QC_MOD_NOT_RECORD && ops_record != QC_MOD_RECORD))
        return QC_RES_INV;

    int pos = qc_module_find_free_list_ops_cmd(module, list_ops);
    if (pos == -1)
        return QC_RES_INV;

    strncpy(module->list_ops_cmd[list_ops][pos], cmd, OPS_LEN -1);
    module->list_ops_cmd_record[list_ops][pos] = ops_record;

    if (int_cmp != NULL)
        module->list_ops_cmp_int[list_ops][pos] = int_cmp;

    if (str_cmp != NULL)
        module->list_ops_cmp_str[list_ops][pos] = str_cmp;

    return QC_RES_OK;
}

qc_res_t qc_module_list_ops_func_append(qc_module_t *module, int list_ops, qc_list_ops_func_t ops, void *para,
                                        qc_mod_record_t ops_record, qc_cap_cmp_int_t int_cmp, qc_cap_cmp_str_t str_cmp)
{
    if (module == NULL || list_ops < 0 || list_ops > LIST_OPS_NUM - 1
       || (ops_record != QC_MOD_NOT_RECORD && ops_record != QC_MOD_RECORD))
        return QC_RES_INV;

    int pos = qc_module_find_free_list_ops_func(module, list_ops);
    if (pos == -1)
        return QC_RES_INV;

    module->list_ops_func[list_ops][pos] = ops;
    module->list_ops_func_para[list_ops][pos] = para;
    module->list_ops_func_record[list_ops][pos] = ops_record;

    if (int_cmp != NULL)
        module->list_ops_cmp_int[list_ops][pos] = int_cmp;

    if (str_cmp != NULL)
        module->list_ops_cmp_str[list_ops][pos] = str_cmp;

    return QC_RES_OK;
}

qc_res_t qc_module_list_ops_exec(qc_module_t *module, int list_ops)
{
    if (module == NULL || list_ops < 0 || list_ops > LIST_OPS_NUM - 1)
        return QC_RES_INV;

    if (module->status == QC_MOD_SUCCESS || module->status == QC_MOD_FAILURE)
        return QC_RES_OK;

    /* Marking is being executed */
    module->status = QC_MOD_EXECUTING;
    module->list_ops_status[list_ops] = QC_MOD_EXECUTING;

    qc_res_t func_res = qc_module_list_ops_exec_func(module, list_ops);

    qc_res_t cmd_res = qc_module_list_ops_exec_cmd(module, list_ops);

    qc_module_list_ops_status_update(module, list_ops, func_res, cmd_res);

    qc_module_status_update(module);

    return QC_RES_OK;
}

qc_manager_t *qc_manager_create(void)
{
    qc_manager_t *manager = NULL;

    manager = (qc_manager_t *)malloc(sizeof(qc_manager_t ));
    if (manager == NULL) {
        printf("malloc manager failed\n");
        return NULL;
    }
    memset(manager, 0, sizeof(qc_manager_t ));

    qc_list_init(&manager->mod_head);
    qc_list_init(&manager->exec_head);
    qc_list_init(&manager->disp_head);

    return manager;
}

qc_res_t qc_manager_clear_status(qc_manager_t *manager)
{
    qc_module_t *mod = NULL, *next_mod = NULL;

    if (manager == NULL) {
        return QC_RES_INV;
    }

    qc_manager_lock(manager);

    if (manager->exec_num > 0) {
        qc_exec_list_del(manager);
    }

    if (manager->disp_num > 0) {
        qc_disp_list_del(manager);
    }

    if (manager->module_num > 0) {
        qc_list_for_each_entry_safe(mod, next_mod, &manager->mod_head, list) {
            if (mod) {
                qc_module_clear_status(mod);
            }
        }
    }

    manager->progress = 0;
    manager->success_num = 0;
    manager->failure_num = 0;
    manager->success_rate = 0;
    manager->exec_num = 0;
    manager->disp_num = 0;

    qc_manager_unlock(manager);

    return QC_RES_OK;
}

qc_res_t qc_manager_delete(qc_manager_t *manager)
{
    if (manager == NULL) {
        return QC_RES_INV;
    }

    qc_manager_lock(manager);

    qc_mod_list_del(manager);
    qc_exec_list_del(manager);
    qc_disp_list_del(manager);

    qc_manager_unlock(manager);
    free(manager);

    return QC_RES_OK;
}

qc_res_t qc_manager_set_lock(qc_manager_t *manager, qc_lock_t lock, qc_lock_t un_lock)
{
    if (manager == NULL) {
        return QC_RES_INV;
    }

    manager->lock = lock;
    manager->un_lock = un_lock;

    return QC_RES_OK;
}

qc_res_t qc_manager_module_append(qc_manager_t *manager, qc_module_t *module)
{
    if (module == NULL || manager == NULL) {
        return QC_RES_INV;
    }

    qc_module_t *new_module = qc_module_create();
    if (new_module == NULL) {
        return QC_RES_INV;
    }
    memcpy(new_module, module, sizeof(qc_module_t));

    qc_manager_lock(manager);

    qc_list_add_tail(&new_module->list, &manager->mod_head);

    manager->module_num++;

    qc_manager_unlock(manager);

    return QC_RES_OK;
}


qc_res_t qc_manager_module_get(qc_manager_t *manager, qc_module_t *back, char *name)
{
    if (manager == NULL || strlen(name) == 0 || manager->module_num <= 0) {
        return QC_RES_INV;
    }

    qc_manager_lock(manager);

    qc_module_t *mod = NULL, *next_mod = NULL;
    qc_list_for_each_entry_safe(mod, next_mod, &manager->mod_head, list) {
        if (mod) {
            if (strncmp(mod->name, name, strlen(name)) == 0) {
                memcpy(back, mod, sizeof(qc_module_t));
                qc_manager_unlock(manager);
                return QC_RES_OK;
            }
        }
    }

    qc_manager_unlock(manager);
    return QC_RES_INV;
}

qc_res_t qc_manager_module_get_index(qc_manager_t *manager, qc_module_t *back, int index)
{
    if (manager == NULL || index < 0 || index >= manager->module_num) {
        return QC_RES_INV;
    }

    qc_manager_lock(manager);

    int _index = index;
    qc_module_t *mod = NULL, *next_mod = NULL;
    qc_list_for_each_entry_safe(mod, next_mod, &manager->mod_head, list) {
        if (mod) {
            if (_index-- == 0) {
                memcpy(back, mod, sizeof(qc_module_t));
                qc_manager_unlock(manager);
                return QC_RES_OK;
            }
        }
    }

    qc_manager_unlock(manager);
    return QC_RES_INV;
}

qc_res_t qc_manager_module_del(qc_manager_t *manager, char *name)
{
    if (manager == NULL || strlen(name) == 0 || manager->module_num <= 0) {
        return QC_RES_INV;
    }

    qc_manager_lock(manager);

    qc_module_t *mod = NULL, *next_mod = NULL;
    qc_list_for_each_entry_safe(mod, next_mod, &manager->mod_head, list) {
        if (mod) {
            if (strncmp(mod->name, name, strlen(mod->name)) == 0) {
                qc_list_del(&mod->list);
                qc_module_delete(mod);
                manager->module_num--;
            }
        }
    }

    qc_manager_unlock(manager);

    return QC_RES_OK;
}

qc_res_t qc_manager_update(qc_manager_t *manager, qc_module_t *module)
{
    if (manager == NULL || module == NULL) {
        return QC_RES_INV;
    }

    /* check if the module result has been written */
    if (qc_manager_check_include_module(manager, module) == QC_RES_INV) {
        return QC_RES_INV;
    }

    /* check if the module has been update */
    qc_manager_module_get(manager, &tmp_module, module->name);
    if (tmp_module.status == QC_MOD_SUCCESS || tmp_module.status == QC_MOD_FAILURE)
        return QC_RES_INV;

    qc_manager_lock(manager);

    if (module->status == QC_MOD_SUCCESS) {
        manager->success_num++;
        manager->success_rate = (manager->success_num * 100) / manager->module_num;
    } else if (module->status == QC_MOD_FAILURE) {
        manager->failure_num++;
    }

    int module_run_num = manager->success_num + manager->failure_num;
    manager->progress = (module_run_num * 100) / manager->module_num;

    qc_manager_module_sync(manager, module);

    qc_manager_unlock(manager);

    return QC_RES_OK;
}


qc_exec_t *qc_exec_create(void)
{
    qc_exec_t *exec = NULL;

    exec = (qc_exec_t *)malloc(sizeof(qc_exec_t));
    if (exec == NULL) {
        return NULL;
    }
    memset(exec, 0, sizeof(qc_exec_t));

    qc_list_init(&exec->list);

    return exec;
}

qc_res_t qc_exec_delete(qc_exec_t *exec)
{
    if (exec) {
        free(exec);
    }

    return QC_RES_OK;
}

qc_res_t qc_exec_set(qc_exec_t *exec, char *mod_name, int list_ops_pos)
{
    if (exec == NULL || mod_name == NULL) {
        return QC_RES_INV;
    }

    if (list_ops_pos < 0 || list_ops_pos > LIST_OPS_NUM -1) {
        return QC_RES_INV;
    }

    strncpy(exec->module_name, mod_name, NAME_LEN - 1);

    exec->list_ops_pos = list_ops_pos;

    qc_list_init(&exec->list);

    return QC_RES_OK;
}

qc_res_t qc_exec_append(qc_exec_t *exec, qc_manager_t *mgr)
{
    return qc_exec_add(exec, mgr, 1);
}

qc_res_t qc_exec_append_left(qc_exec_t *exec, qc_manager_t *mgr)
{
    return qc_exec_add(exec, mgr, 0);
}

qc_res_t qc_exec_pop(qc_exec_t *exec, qc_manager_t *mgr)
{
    if (mgr == NULL || exec == NULL || mgr->exec_num <= 0) {
        return QC_RES_INV;
    }

    qc_manager_lock(mgr);

    qc_exec_t *list_exec = NULL, *next_list_exec = NULL;
    qc_list_for_each_entry_safe(list_exec, next_list_exec, &mgr->exec_head, list) {
        if (list_exec) {
            memcpy(exec, list_exec, sizeof(qc_exec_t));
            break;
        }
    }

    mgr->exec_num--;
    qc_list_del(&list_exec->list);

    qc_manager_unlock(mgr);

    qc_exec_delete(list_exec);

    return QC_RES_OK;
}

qc_res_t qc_exec_ops_set_append(qc_manager_t *mgr, char *mod_name, int list)
{
    qc_exec_t tmp_exec;
    qc_exec_set(&tmp_exec, mod_name, list);
    return qc_exec_append(&tmp_exec, mgr);
}


qc_disp_data_t *qc_disp_data_create(void)
{
    qc_disp_data_t * disp_data = NULL;

    disp_data = (qc_disp_data_t *)malloc(sizeof(qc_disp_data_t));
    if (disp_data == NULL) {
        return NULL;
    }
    memset(disp_data, 0, sizeof(qc_disp_data_t));

    qc_list_init(&disp_data->list);

    return disp_data;
}

qc_res_t qc_disp_data_delete(qc_disp_data_t *disp_data)
{
    if (disp_data) {
        free(disp_data);
    }

    return QC_RES_OK;
}

qc_res_t qc_disp_data_set(qc_disp_data_t *disp_data, qc_manager_t *manager, qc_module_t *module)
{
    if (disp_data == NULL || manager == NULL || module == NULL) {
        return QC_RES_INV;
    }

    memset(disp_data, 0, sizeof(qc_disp_data_t));
    memcpy(disp_data->mod_name, module->name, strlen(module->name));
    memcpy(disp_data->mod_desc, module->desc, DESC_LEN);
    memcpy(disp_data->list_ops_status, module->list_ops_status, sizeof(int) * LIST_OPS_NUM);
    memcpy(disp_data->list_ops_desc, module->list_ops_desc, sizeof(char) * LIST_OPS_NUM * DESC_LEN);
    disp_data->mod_status = module->status;

    memcpy(disp_data->mgr_name, manager->name, strlen(manager->name));
    disp_data->mgr_progress = manager->progress;
    disp_data->mgr_success_num = manager->success_num;
    disp_data->mgr_failure_num = manager->failure_num;
    disp_data->mgr_success_rate = manager->success_rate;

    qc_list_init(&disp_data->list);

    return QC_RES_OK;
}

qc_res_t qc_disp_data_append(qc_disp_data_t *disp_data, qc_manager_t *manager)
{
    return qc_disp_data_add(disp_data, manager, 1);
}

qc_res_t qc_disp_data_append_left(qc_disp_data_t *disp_data, qc_manager_t *manager)
{
    return qc_disp_data_add(disp_data, manager, 0);
}

qc_res_t qc_disp_data_pop(qc_disp_data_t *disp_data, qc_manager_t *manager)
{
    if (manager == NULL|| manager->disp_num <= 0) {
        return QC_RES_INV;
    }

    qc_manager_lock(manager);

    qc_disp_data_t *list_disp = NULL, *next_list_disp = NULL;
    qc_list_for_each_entry_safe(list_disp, next_list_disp, &manager->disp_head, list) {
        if (list_disp) {
            memcpy(disp_data, list_disp, sizeof(qc_disp_data_t));
            break;
        }
    }

    manager->disp_num--;

    qc_list_del(&list_disp->list);

    qc_manager_unlock(manager);

    qc_disp_data_delete(list_disp);

    return QC_RES_OK;
}

void qc_module_debug(qc_module_t *module, qc_debug_t debug_flag)
{
    if (module == NULL)
        return;

    int i = 0, j = 0;
    printf("module name = %s", module->name);

    if (debug_flag & QC_DEBUG_BASE) {
        printf(" status = %d, desc = %s\n", module->status, module->desc);
    }

    if (debug_flag & QC_DEBUG_CORE) {
        for (i = 0; i < LIST_OPS_NUM; i++) {
            if (strlen(module->list_ops_desc[i]) > 0)
                printf("\tlist_ops_desc[%d] = %s\n", i, module->list_ops_desc[i]);

            if (module->list_ops_status[i] != QC_MOD_UNEXECUTED)
                printf("\tlist_ops_status[%d] = %d\n", i, module->list_ops_status[i]);

            if (module->list_ops_runtime[i] != TIMEOUT_MS_NULL)
                printf("\tlist_ops_runtime[%d] = %d\n", i, module->list_ops_runtime[i]);

            for (j = 0; j < OPS_NUM; j++) {
                if (strlen(module->list_ops_cmd[i][j]) > 0)
                    printf("\tlist_ops_cmd[%d][%d] = %s\n", i, j, module->list_ops_cmd[i][j]);

                if (module->list_ops_func[i][j] != NULL) {
                    printf("\tlist_ops_func[%d][%d] = %p para = %p\n", i, j, module->list_ops_func[i][j], module->list_ops_func_para[i][j]);
                }

                if (module->list_ops_cmp_int[i][j] != list_ops_cmp_int_default) {
                    printf("\tlist_ops_cmp_int[%d][%d] = %p\n", i, j, module->list_ops_cmp_int[i][j]);
                }

                if (module->list_ops_cmp_str[i][j] != list_ops_cmp_str_default) {
                    printf("\tlist_ops_cmp_str[%d][%d] = %p\n", i, j, module->list_ops_cmp_str[i][j]);
                }

                if (module->list_ops_cmd_record[i][j] != QC_MOD_NOT_RECORD) {
                    printf("\tlist_ops_cmd_record[%d][%d] = %d\n", i, j, module->list_ops_cmd_record[i][j]);
                }

                if (module->list_ops_func_record[i][j] != QC_MOD_NOT_RECORD) {
                    printf("\tlist_ops_func_record[%d][%d] = %d\n", i, j, module->list_ops_func_record[i][j]);
                }
            }
        }
    }

    if (debug_flag & QC_DEBUG_DETAIL && module->log_buf) {
        printf("\tlog_buf_len = %d, log_buf = %s\n", module->log_buf_len, module->log_buf);
    }
}

void qc_manager_debug(qc_manager_t *manager, qc_debug_t debug_flag)
{
    if (manager == NULL)
        return;

    printf("manager name = %s\n", manager->name);
    if (debug_flag & QC_DEBUG_BASE) {
        printf("progress = %d, success_num = %d, failure_num  = %d\n",
                manager->progress, manager->success_num, manager->failure_num);
    }

    if (debug_flag & QC_DEBUG_CORE) {
        printf("success_rate = %d module_num = %d, exec_num = %d, disp_num = %d\n",
                manager->success_rate, manager->module_num, manager->exec_num, manager->disp_num);
    }

    qc_module_t *mod = NULL, *next_mod = NULL;
    if (manager->module_num > 0 && debug_flag & QC_DEBUG_DETAIL) {
        qc_list_for_each_entry_safe(mod, next_mod, &manager->mod_head, list) {
            qc_module_debug(mod, QC_DEBUG_ALL);
        }
    }

    qc_exec_t *exec = NULL, *next_exec = NULL;
    if (manager->exec_num > 0 && debug_flag & QC_DEBUG_DETAIL) {
        qc_list_for_each_entry_safe(exec, next_exec, &manager->exec_head, list) {
            qc_exec_debug(exec, QC_DEBUG_ALL);
        }
    }

    qc_disp_data_t *disp_data = NULL, *next_disp = NULL;
    if (manager->exec_num > 0 && debug_flag & QC_DEBUG_DETAIL) {
        qc_list_for_each_entry_safe(disp_data, next_disp, &manager->disp_head, list) {
            qc_disp_data_debug(disp_data, QC_DEBUG_BASE);
        }
    }
}

void qc_exec_debug(qc_exec_t *exec, qc_debug_t debug_flag)
{
    if (exec == NULL)
        return;

    printf("exec name = %s, sets = %d\n", exec->module_name, exec->list_ops_pos);
}

void qc_disp_data_debug(qc_disp_data_t *disp_data, qc_debug_t debug_flag)
{
    if (disp_data == NULL)
        return;

    if (debug_flag & QC_DEBUG_BASE) {
        printf("disp_data mod_name = %s, mod_status = %d, mgr_name = %s, mgr_progress = %d, mgr_success_rate = %d\n",
                disp_data->mod_name, disp_data->mod_status, disp_data->mgr_name,
                disp_data->mgr_progress, disp_data->mgr_success_rate);
    }

    if (debug_flag & QC_DEBUG_CORE) {
        printf("disp_data mgr_status = %d, mod_desc = %s, mgr_success_num = %d, mgr_failure_num = %d\n",
                disp_data->mgr_status, disp_data->mod_desc, disp_data->mgr_success_num, disp_data->mgr_failure_num);
    }

    if (debug_flag & QC_DEBUG_DETAIL) {
        for (int i = 0; i < LIST_OPS_NUM; i++) {
            if (disp_data->list_ops_status[i] == QC_MOD_UNEXECUTED)
                continue;
            printf("disp_data list_ops_status[%d] = %d, list_ops_desc[%d] = %s\n",
                    i, disp_data->list_ops_status[i], i, disp_data->list_ops_desc[i]);
        }
    }
}

static int qc_module_find_free_list_ops_cmd(qc_module_t *module, int list_ops)
{
    for (int pos = 0; pos < OPS_NUM; pos++) {
        if (strlen(module->list_ops_cmd[list_ops][pos]) == 0) {
            return pos;
        }
    }

    return -1;
}

static int qc_module_find_free_list_ops_func(qc_module_t *module, int list_ops)
{
    for (int pos = 0; pos < OPS_NUM; pos++) {
        if (module->list_ops_func[list_ops][pos] == NULL) {
            return pos;
        }
    }

    return -1;
}

static int qc_module_get_list_ops_func_num(qc_module_t *module, int list_ops)
{
    int pos = 0;
    for (pos = 0; pos < OPS_NUM; pos++) {
        if (module->list_ops_func[list_ops][pos] == NULL) {
            break;
        }
    }
    return pos;
}

static int qc_module_get_list_ops_cmd_num(qc_module_t *module, int list_ops)
{
    int pos = 0;
    for (pos = 0; pos < OPS_NUM; pos++) {
        if (strlen(module->list_ops_cmd[list_ops][pos]) == 0) {
            break;
        }
    }
    return pos;
}

static void qc_module_init_cmd_res_list(qc_module_t *module, int list_ops, qc_res_t *list)
{
    for (int i = 0; i < OPS_NUM; i++) {
        if (module->list_ops_cmd_record[list_ops][i] == QC_MOD_RECORD) {
            list[i] = QC_RES_INV;
        } else {
            list[i] = QC_RES_OK;
        }
    }
}

static void qc_module_init_func_res_list(qc_module_t *module, int list_ops, qc_res_t *list)
{
    for (int i = 0; i < OPS_NUM; i++) {
        if (module->list_ops_func_record[list_ops][i] == QC_MOD_RECORD) {
            list[i] = QC_RES_INV;
        } else {
            list[i] = QC_RES_OK;
        }
    }
}

qc_res_t qc_module_list_ops_exec_res(qc_res_t *list)
{
    for (int i = 0; i < OPS_NUM; i++) {
        if (list[i] == QC_RES_INV)
            return QC_RES_INV;
    }

    return QC_RES_OK;
}

static qc_res_t qc_module_list_ops_exec_func(qc_module_t *module, int list_ops)
{
    int list_ops_func_num = 0;
    qc_res_t func_res_list[OPS_NUM] = {0};

    list_ops_func_num = qc_module_get_list_ops_func_num(module, list_ops);

    if (list_ops_func_num == 0)
        return QC_RES_OK;

    qc_module_init_func_res_list(module, list_ops, func_res_list);

    for (int i = 0; i < list_ops_func_num; i++) {
        qc_list_ops_func_t list_ops_func = module->list_ops_func[list_ops][i];
        void *list_ops_func_para = module->list_ops_func_para[list_ops][i];
        qc_res_t result = list_ops_func(list_ops_func_para);

        /* save command run results */
        if (module->list_ops_func_record[list_ops][i] == QC_MOD_RECORD) {
            func_res_list[i] = result;
        }
    }

    return qc_module_list_ops_exec_res(func_res_list);
}

static qc_res_t qc_module_list_ops_exec_cmd(qc_module_t *module, int list_ops)
{
    int list_ops_cmd_num = 0;
    qc_res_t cmd_res_list[OPS_NUM] = {0};

    list_ops_cmd_num = qc_module_get_list_ops_cmd_num(module, list_ops);
    if (list_ops_cmd_num == 0)
        return QC_RES_OK;

    qc_module_init_cmd_res_list(module, list_ops, cmd_res_list);

#if QC_OS_RT_THREAD
    extern void qc_module_list_ops_exec_cmd_rtos(qc_module_t *module, int list_ops, int list_pos_num, int *cmd_res_list);
    qc_module_list_ops_exec_cmd_rtos(module, list_ops, list_ops_cmd_num, cmd_res_list);
#elif QC_OS_LINUX
    extern void qc_module_list_ops_exec_cmd_linux(qc_module_t *module, int list_ops);
    qc_module_list_ops_exec_cmd_linux(module, list_ops, list_ops_cmd_num, cmd_res_list);
#else
    extern void qc_module_list_ops_exec_cmd_common(qc_module_t *module, int list_ops, int list_pos_num, int *cmd_res_list);
    qc_module_list_ops_exec_cmd_common(module, list_ops, list_ops_cmd_num, cmd_res_list);
#endif

    return qc_module_list_ops_exec_res(cmd_res_list);
}

static void qc_module_list_ops_status_update(qc_module_t *module, int list_ops, qc_res_t func_res, qc_res_t cmd_res)
{
    if (func_res == QC_RES_OK && cmd_res == QC_RES_OK)
        module->list_ops_status[list_ops] = QC_MOD_SUCCESS;
    else
        module->list_ops_status[list_ops] = QC_MOD_FAILURE;
}

static void qc_module_get_list_ops_record(qc_module_t *module, qc_mod_record_t *list_ops_record)
{
    for (int i = 0; i < LIST_OPS_NUM; i++) {
        for (int j = 0; j < OPS_NUM; j++) {
            if (module->list_ops_cmd_record[i][j] == QC_MOD_RECORD ||
                module->list_ops_func_record[i][j] == QC_MOD_RECORD) {
                list_ops_record[i] = QC_MOD_RECORD;
                break;
            }
        }
    }
}

static void qc_module_status_update(qc_module_t *module)
{
    int list_ops_failure_num = 0;
    qc_mod_status_t list_ops_record[LIST_OPS_NUM];
    qc_module_get_list_ops_record(module, list_ops_record);

    for (int i = 0; i < LIST_OPS_NUM; i++) {
        if (list_ops_record[i] == QC_MOD_RECORD && module->list_ops_status[i] == QC_MOD_FAILURE)
            list_ops_failure_num++;
        if (list_ops_record[i] == QC_MOD_RECORD && module->list_ops_status[i] == QC_MOD_UNEXECUTED) {
            module->status = QC_MOD_EXECUTING;
            return;
        }
    }

    if (list_ops_failure_num)
        module->status = QC_MOD_FAILURE;
    else
        module->status = QC_MOD_SUCCESS;
}


qc_res_t qc_manager_lock(qc_manager_t *manager)
{
    if (manager->lock) {
        manager->lock();
    }
    return QC_RES_OK;
}

qc_res_t qc_manager_unlock(qc_manager_t *manager)
{
    if (manager->un_lock) {
        manager->un_lock();
    }
    return QC_RES_OK;
}

static void qc_mod_list_del(qc_manager_t *manager)
{
    qc_module_t *mod = NULL, *next_mod = NULL;

    qc_list_for_each_entry_safe(mod, next_mod, &manager->mod_head, list) {
        if (mod) {
            qc_list_del(&mod->list);
            qc_module_delete(mod);
        }
    }
}

static void qc_exec_list_del(qc_manager_t *manager)
{
    qc_exec_t *exec = NULL, *next_exec = NULL;

    qc_list_for_each_entry_safe(exec, next_exec, &manager->mod_head, list) {
        if (exec) {
            qc_list_del(&exec->list);
            qc_exec_delete(exec);
        }
    }
}

static void qc_disp_list_del(qc_manager_t *manager)
{
    qc_disp_data_t *disp_data = NULL, *next_disp = NULL;

    qc_list_for_each_entry_safe(disp_data, next_disp, &manager->disp_head, list) {
        if (disp_data) {
            qc_list_del(&disp_data->list);
            qc_disp_data_delete(disp_data);
        }
    }
}

static qc_res_t qc_manager_check_include_module(qc_manager_t *manager, qc_module_t *module)
{
    qc_module_t *mod = NULL, *next_mod = NULL;

    qc_list_for_each_entry_safe(mod, next_mod, &manager->mod_head, list) {
        if (mod) {
            if (strncmp(mod->name, module->name, strlen(module->name)) == 0) {
                return QC_RES_OK;
            }
        }
    }

    return QC_RES_INV;
}

qc_res_t qc_manager_module_sync(qc_manager_t *manager, qc_module_t *module)
{
    qc_module_t *mod = NULL, *next_mod = NULL;

    qc_list_for_each_entry_safe(mod, next_mod, &manager->mod_head, list) {
        if (mod) {
            if (strncmp(mod->name, module->name, strlen(module->name)) == 0) {
                memcpy(mod, module, sizeof(qc_module_t));
                return QC_RES_OK;
            }
        }
    }
    return QC_RES_INV;
}

static qc_res_t qc_exec_add(qc_exec_t *exec, qc_manager_t *mgr, int append)
{
    if (mgr == NULL || exec == NULL) {
        return QC_RES_INV;
    }

    qc_exec_t *new_exec = qc_exec_create();
    if (new_exec == NULL) {
        return QC_RES_INV;
    }
    memcpy(new_exec, exec, sizeof(qc_exec_t));

    qc_manager_lock(mgr);

    if (append)
        qc_list_add_tail(&new_exec->list, &mgr->exec_head);
    else
        qc_list_add_head(&new_exec->list, &mgr->exec_head);
    mgr->exec_num++;

    qc_manager_unlock(mgr);

    return QC_RES_OK;
}

static qc_res_t qc_disp_data_add(qc_disp_data_t *disp_data, qc_manager_t *manager, int append)
{
    if (manager == NULL || disp_data == NULL) {
        return QC_RES_INV;
    }

    qc_disp_data_t *new_disp = qc_disp_data_create();
    if (new_disp == NULL) {
        return QC_RES_INV;
    }
    memcpy(new_disp, disp_data, sizeof(qc_disp_data_t));

    qc_manager_lock(manager);

    if (append)
        qc_list_add_tail(&new_disp->list, &manager->disp_head);
    else
        qc_list_add_head(&new_disp->list, &manager->disp_head);
    manager->disp_num++;

    qc_manager_unlock(manager);

    return QC_RES_OK;
}
