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
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/select.h>
#include <sys/time.h>

#include "qc_ops.h"
#include "qc_cmp.h"
#include "qc_list.h"

static unsigned int module_id = 0;

extern int msh_exec(char *cmd, rt_size_t length);

int qc_msh_exec(char *command, int length)
{
    static char cmd[256] = {0};
    strncpy(cmd, command, sizeof(cmd) - 1);
    return msh_exec(cmd, strlen(cmd));
}

static int get_module_id(void)
{
    return ++module_id;
}

static int free_module_id(void)
{
    return --module_id;
}

struct qc_module *qc_module_create(void)
{
    int i = 0;
    struct qc_module *module = NULL;

    module = (struct qc_module *)malloc(sizeof(struct qc_module));
    if (module == NULL) {
        printf("malloc module failed\n");
        return NULL;
    }
    memset(module, 0, sizeof(struct qc_module));

    module->id = get_module_id();
    module->status = UNEXECUTED;

    for (i = 0; i < EXEC_SETS; i++) {
        module->exec_sets_result[i] = UNEXECUTED;
        module->exec_sets_record[i] = NOT_RECORD;
    }

    return module;
}

int qc_module_init(struct qc_module *module)
{
    if (module == NULL) {
        printf("qc module init failed, module is null\n");
        return FAILURE;
    }

    memset(module, 0, sizeof(struct qc_module));

    module->id = get_module_id();
    module->status = UNEXECUTED;

    for (int i = 0; i < EXEC_SETS; i++) {
        module->exec_sets_result[i] = UNEXECUTED;
        module->exec_sets_record[i] = NOT_RECORD;
    }

    return SUCCESS;
}

int qc_module_clear_status(struct qc_module *module)
{
    int i;

    module->now_log_len = 0;
    module->status = UNEXECUTED;

    for (i = 0; i < EXEC_SETS; i++) {
        module->exec_sets_result[i] = UNEXECUTED;
    }

    return SUCCESS;
}

int qc_module_delete(struct qc_module *module)
{
    if (module)
        free(module);

    free_module_id();
    return SUCCESS;
}

int qc_module_add_name(struct qc_module *module, char *name)
{
    if (module == NULL || name == NULL) {
        printf("module add name failed, module or name is null\n");
        return FAILURE;
    }

    int name_len = strlen(name);
    if (name_len > NAME_LEN - 1) {
        printf("name is too long, exceeding size = %d\n", NAME_LEN - 1 - NAME_LEN - 1);
        strncpy(module->name, name, NAME_LEN - 1 - 1);
    }

    strncpy(module->name, name, strlen(name));

    return SUCCESS;
}

int qc_module_add_desc(struct qc_module *module, char *desc)
{
    if (module == NULL || desc == NULL) {
        printf("module add desc failed,  module or desc is null\n");
        return FAILURE;
    }

    int desc_len = strlen(desc);
    if (desc_len > DESC_LEN - 1) {
        printf("desc is too long, exceeding size = %d\n", desc_len - DESC_LEN);
        strncpy(module->desc, desc, DESC_LEN - 1);
    }

    strncpy(module->desc, desc, DESC_LEN - 1);

    return SUCCESS;
}

int qc_module_add_sets_desc(struct qc_module *module, int exec_set, char *sets_desc)
{
    if (exec_set < 0 || exec_set >= EXEC_SETS || module == NULL) {
        printf("module add sets desc failed, incorrect parameter input\n");
        return FAILURE;
    }

    int sets_desc_len = strlen(sets_desc);
    if (sets_desc_len > DESC_LEN - 1) {
        printf("desc is too long, exceeding size = %d\n", sets_desc_len - DESC_LEN - 1);
        strncpy(module->exec_sets_desc[EXEC_SETS - 1], sets_desc, DESC_LEN - 1);
    }

    strncpy(module->exec_sets_desc[exec_set], sets_desc, DESC_LEN - 1);

    return SUCCESS;
}

int qc_module_add_cmd(struct qc_module *module, int exec_set, char *cmd, int exec_record)
{
    if (exec_set < 0 || exec_set >= EXEC_SETS || module == NULL) {
        printf("module add cmd failed, incorrect parameter input\n");
        return FAILURE;
    }

    int cmd_len = strlen(cmd);
    if (cmd_len > EXEC_LEN - 1) {
        printf("cmd is too long, exceeding size = %d\n", cmd_len - EXEC_LEN);
        return FAILURE;
    }

    if (exec_record != RECORD && exec_record != NOT_RECORD) {
        printf("exec_record value is illegality, default use NOT_RECORD, exec_record = %d\n", exec_record);
        exec_record = NOT_RECORD;
    }

    int i;
    for (i = 0; i < EXEC_NUM; i++) {
        if (strlen(module->exec_sets_cmd[exec_set][i]) == 0) {
            break;
        }
    }

    if (i == EXEC_NUM) {
        printf("the current exec cmd set no longer has space, defaults to overwriting the last one\n");
        i = EXEC_NUM -1;
    }

    strncpy(module->exec_sets_cmd[exec_set][i], cmd, EXEC_LEN - 1);
    module->exec_sets_record[exec_set] = exec_record;

    return SUCCESS;
}

int qc_module_add_func(struct qc_module *module, int exec_set,
                       qc_exec_func func, void *func_para, int exec_record)
{
    if (exec_set < 0 || exec_set >= EXEC_SETS ||
        module == NULL || func == NULL) {
        printf("module set sets desc failed, incorrect parameter input\n");
        return FAILURE;
    }

    if (exec_record != RECORD && exec_record != NOT_RECORD) {
        printf("exec_record value is illegality, default use NOT_RECORD, exec_record = %d\n", exec_record);
        exec_record = NOT_RECORD;
    }

    int i;
    for (i = 0; i < EXEC_NUM; i++) {
        if (module->exec_sets_func[exec_set][i] == NULL) {
            break;
        }
    }

    if (i == EXEC_NUM) {
        printf("the current exec func set no longer has space, defaults to overwriting the last one\n");
        i = EXEC_NUM -1;
    }

    module->exec_sets_func[exec_set][i] = func;
    module->func_para[exec_set][i] = func_para;
    module->exec_sets_record[exec_set] = exec_record;

    return SUCCESS;
}

int qc_module_add_runtime(struct qc_module *module, int exec_set, unsigned int runtime)
{
    printf("qc_module_add_runtime did't supported\n");
    return FAILURE;
}

int qc_module_add_cmp(struct qc_module *module, int exec_set, qc_cmp f_cmp)
{
    if (exec_set < 0 || exec_set >= EXEC_SETS ||
        module == NULL || f_cmp == NULL) {
        printf("module add cmp failed, incorrect parameter input\n");
        return FAILURE;
    }

    int i;
    for (i = 0; i < EXEC_NUM; i++) {
        if (module->exec_sets_cmp[exec_set][i] == NULL) {
            break;
        }
    }

    if (i == EXEC_NUM) {
        printf("the current exec func set no longer has space, defaults to overwriting the last one\n");
        i = EXEC_NUM -1;
    }

    module->exec_sets_cmp[exec_set][i] = f_cmp;

    return SUCCESS;
}

static int qc_module_all_cmd_empty(struct qc_module *module)
{
    int i = 0;
    int j = 0;
    int count_empty_cmd = 0;

    for (i = 0; i < EXEC_SETS; i++)
        for (j = 0; j < EXEC_NUM; j++) {
            if (strlen(module->exec_sets_cmd[i][j]) == 0) {
                count_empty_cmd++;
            }
        }

    if (count_empty_cmd == EXEC_SETS * EXEC_NUM) {
        printf("module:%s cmd is empty\n", module->name);
        return SUCCESS;
    }

    return FAILURE;
}

static int  qc_module_status_update(struct qc_module *module)
{
    int recode_num = 0;
    int success_num = 0;
    int failure_num = 0;

    for(int i = 0; i < EXEC_SETS; i++) {
        if (module->exec_sets_record[i] == RECORD) {
            recode_num++;
        }

        if (module->exec_sets_result[i] == SUCCESS) {
            success_num++;
        } else if (module->exec_sets_result[i] == FAILURE) {
            failure_num++;
        }
    }

    if (recode_num == success_num + failure_num) {
        if (recode_num == success_num) {
            module->status = SUCCESS;
        } else {
            module->status = FAILURE;
        }
    } else {
        module->status = EXECUTING;
    }

    return SUCCESS;
}

static void qc_module_exec_result_update(struct qc_module *module, int exec_sets, int cur_exec, int *exec_result)
{
    int i = 0;
    int j = 0;

    for (i = 0; i < cur_exec; i++) {
        /* all exec result is successful */
        if (exec_result[i] == SUCCESS) {
            j++;
        }
    }

    if (j == cur_exec && cur_exec != 0) {
        module->exec_sets_result[exec_sets] = SUCCESS;
    } else {
        module->exec_sets_result[exec_sets] = FAILURE;
    }
}

static int qc_module_cmd_execute(struct qc_module *module, int cmd_sets, int exec_flag)
{
    int cur_cmd = 0;
    char *now_cmd = NULL;
    qc_cmp now_f_cmp = NULL;
    int cmd_result[EXEC_NUM] = {0};

    if (strlen(module->exec_sets_cmd[cmd_sets][0]) == 0) {
        return FAILURE;
    }

    /* take a set of commands to execute */
    while (cur_cmd < EXEC_NUM) {
        now_cmd = module->exec_sets_cmd[cmd_sets][cur_cmd];
        now_f_cmp = module->exec_sets_cmp[cmd_sets][cur_cmd];
        if (strlen(now_cmd) == 0) { /* all cmds have been executed */
            break;
        }

        int msh_exec_result = FAILURE;
        msh_exec_result = qc_msh_exec(now_cmd, strlen(now_cmd));
        if (module->exec_sets_record[cmd_sets] == RECORD) {
            if (now_f_cmp == NULL) {
                cmd_result[cur_cmd] = qc_result_analysis_common(msh_exec_result, module->name);
            } else {
                cmd_result[cur_cmd] = now_f_cmp(msh_exec_result);
            }
        }
        cur_cmd++;
    }

    if (module->exec_sets_record[cmd_sets] == RECORD) {
        qc_module_exec_result_update(module, cmd_sets, cur_cmd, cmd_result);
        qc_module_status_update(module);
    }

    return UPDATED;
}

static int qc_module_all_func_empty(struct qc_module *module)
{
    int count_empty_cmd = 0;

    for (int i = 0; i < EXEC_SETS; i++)
        for (int j = 0; j < EXEC_NUM; j++) {
            if (module->exec_sets_func[i][j] == NULL) {
                count_empty_cmd++;
            }
    }

    if (count_empty_cmd == EXEC_SETS * EXEC_NUM) {
        printf("module:%s func is empty\n", module->name);
        return SUCCESS;
    }

    return FAILURE;
}

static int qc_module_func_execute(struct qc_module *module, int func_set, int exec_flag)
{
    int cur_func = 0;
    int func_result[EXEC_NUM] = {0};
    int exec_result = FAILURE;

    /* take a set of commands to execute */
    while (cur_func < EXEC_NUM) {
        if (module->exec_sets_func[func_set][cur_func] == NULL) {
            break;
        }

        qc_exec_func exec_func = module->exec_sets_func[func_set][cur_func];
        void *exec_para = module->func_para[func_set][cur_func];
        exec_result = exec_func(exec_para);

        /* save command run results */
        if (module->exec_sets_record[func_set] == RECORD) {
            func_result[cur_func] = exec_result;
        }

        cur_func++;
    }

    if (module->exec_sets_record[func_set] == RECORD) {
        qc_module_exec_result_update(module, func_set, cur_func, func_result);
        qc_module_status_update(module);
    }

    return UPDATED;
}

int qc_module_execute(struct qc_module *module, int exec_set, int exec_flag)
{
    if (exec_set < 0 || exec_set >= EXEC_SETS || module == NULL) {
        printf("module set sets desc failed, incorrect parameter input\n");
        return FAILURE;
    }

    if ((exec_flag & EXEC_CMD) && qc_module_all_cmd_empty(module) == SUCCESS) {
        module->status = FAILURE;
        return UPDATED;
    }

    if ((exec_flag & EXEC_FUNC) && qc_module_all_func_empty(module) == SUCCESS) {
        module->status = FAILURE;
        return UPDATED;
    }

    if (exec_flag & EXEC_CMD) {
        return qc_module_cmd_execute(module, exec_set, exec_flag);
    } else if (exec_flag & EXEC_FUNC) {
        return qc_module_func_execute(module, exec_set, exec_flag);
    } else {
       printf("qc exec error, exec_flag illegality, %s exec_flag = %d\n", module->name, exec_flag);
       return FAILURE;
    }
}

static void bin_to_hex(unsigned char byte, char hex_str[2]) {
    static const char *hex_digits = "0123456789abcdef";

    hex_str[0] = hex_digits[(byte >> 4) & 0xF];
    hex_str[1] = hex_digits[byte & 0xF];
}

static unsigned char *get_chip_id(char *chip_id)
{
    static unsigned char chip_id_context[128 / 8] = {0};
    char chip_id_addr[64] = {0};
    char cmd[128] = {0};
    int i = 0, pos = 0, chip_id_len = 128 / 8;

    /* read chip id to chip_id_context */
    snprintf(chip_id_addr, sizeof(chip_id_addr), "%p", (void *)chip_id_context);
    snprintf(cmd, sizeof(cmd), "efuse read %s 10 0x20", chip_id_addr);
    qc_msh_exec(cmd, strlen(cmd));

    /* the chip_id buf context is
     * 44-80-ce-b8-52-2f-c0-0c-06-0c-11-18-09-00-30-04
    */
    for (i = 0; i < chip_id_len; i++) {
        bin_to_hex(chip_id_context[i], (chip_id + pos));
        pos = ((i + 1) * 2) + i;
        chip_id[pos] = '-';
        pos++;
    }
    chip_id[pos - 1] = '\0';

    return chip_id_context;
}

struct qc_manager *qc_manager_create(void)
{
    struct qc_manager *manager = NULL;

    manager = (struct qc_manager *)malloc(sizeof(struct qc_manager));
    if (manager == NULL) {
        printf("malloc manager failed\n");
        return NULL;
    }
    memset(manager, 0, sizeof(struct qc_manager));

    get_chip_id(manager->chip_id);

    qc_list_init(&manager->mod_list);
    qc_list_init(&manager->exec_list);
    qc_list_init(&manager->disp_list);

    return manager;
}

int qc_manager_init(struct qc_manager *manager)
{
    if (manager == NULL) {
        printf("qc manager init failed, manager is null\n");
        return FAILURE;
    }

    memset(manager, 0, sizeof(struct qc_manager));

    get_chip_id(manager->chip_id);

    qc_list_init(&manager->mod_list);
    qc_list_init(&manager->exec_list);
    qc_list_init(&manager->disp_list);

    return SUCCESS;
}

int qc_manager_add_lock(struct qc_manager *manager, qc_lock lock, qc_lock un_lock)
{
    if (manager == NULL) {
        printf("qc manager add lock failed, manager is null\n");
        return FAILURE;
    }

    manager->lock = lock;
    manager->un_lock = un_lock;

    return SUCCESS;
}

static void qc_manager_lock(struct qc_manager *manager)
{
    if (manager->lock) {
        manager->lock();
    }
}

static void qc_manager_unlock(struct qc_manager *manager)
{
    if (manager->un_lock) {
        manager->un_lock();
    }
}

static void qc_mod_list_del(struct qc_manager *manager)
{
    struct qc_module *mod = NULL, *next_mod = NULL;

    qc_list_for_each_entry_safe(mod, next_mod, &manager->mod_list, list) {
        if (mod) {
            qc_list_del(&mod->list);
            qc_module_delete(mod);
        }
    }
}

static void qc_exec_list_del(struct qc_manager *manager)
{
    struct qc_mod_exec *exec = NULL, *next_exec = NULL;

    qc_list_for_each_entry_safe(exec, next_exec, &manager->exec_list, list) {
        if (exec) {
            qc_list_del(&exec->list);
            qc_mod_exec_delete(exec);
        }
    }
}

static void qc_disp_list_del(struct qc_manager *manager)
{
    struct qc_disp *disp = NULL, *next_disp = NULL;

    qc_list_for_each_entry_safe(disp, next_disp, &manager->disp_list, list) {
        if (disp) {
            qc_list_del(&disp->list);
            qc_disp_delete(disp);
        }
    }
}

int qc_manager_clear_status(struct qc_manager *manager)
{
    struct qc_module *mod = NULL, *next_mod = NULL;

    if (manager == NULL) {
        printf("manager clear status failed, manager is null\n");
        return FAILURE;
    }

    qc_manager_lock(manager);

    if (manager->exec_num > 0) {
        qc_exec_list_del(manager);
    }

    if (manager->disp_num > 0) {
        qc_disp_list_del(manager);
    }

    if (manager->module_num > 0) {
        qc_list_for_each_entry_safe(mod, next_mod, &manager->mod_list, list) {
            if (mod) {
                qc_module_clear_status(mod);
            }
        }
    }

    manager->status = 0;
    manager->progress = 0;
    manager->success_num = 0;
    manager->failure_num = 0;
    manager->success_rate = 0;
    manager->exec_num = 0;
    manager->disp_num = 0;

    qc_manager_unlock(manager);

    return SUCCESS;
}

int qc_manager_delete(struct qc_manager *manager)
{
    if (manager == NULL) {
        printf("manager delete failed, manager is null\n");
        return FAILURE;
    }

    qc_manager_lock(manager);

    qc_mod_list_del(manager);
    qc_exec_list_del(manager);
    qc_disp_list_del(manager);

    qc_manager_unlock(manager);
    free(manager);

    return SUCCESS;
}

int qc_manager_add_name(struct qc_manager *manager, char *name)
{
    if (manager == NULL || name == NULL) {
        printf("module add name failed, module or name is null\n");
        return FAILURE;
    }

    qc_manager_lock(manager);

    int name_len = strlen(name);
    if (name_len > NAME_LEN - 1) {
        printf("name is too long, exceeding size = %d\n", NAME_LEN - 1 - NAME_LEN - 1);
        strncpy(manager->name, name, NAME_LEN - 1);
    }

    strncpy(manager->name, name, strlen(name));

    qc_manager_unlock(manager);

    return SUCCESS;
}

int qc_manager_add_module(struct qc_manager *manager, struct qc_module *module)
{
    if (module == NULL || manager == NULL) {
        printf("manager add module failed, module or manager is null\n");
        return FAILURE;
    }

    struct qc_module *new_module = qc_module_create();
    if (new_module == NULL) {
        printf("qc manager add module failed, can't create module\n");
        return FAILURE;
    }
    memcpy(new_module, module, sizeof(struct qc_module));

    qc_manager_lock(manager);

    qc_list_add_tail(&new_module->list, &manager->mod_list);

    manager->module_num++;

    qc_manager_unlock(manager);

    return SUCCESS;
}

int qc_manager_get_module(struct qc_manager *manager, char *name, struct qc_module *back)
{
    struct qc_module *mod = NULL, *next_mod = NULL;
    int find_module = FAILURE;

    if (manager == NULL) {
        printf("manager get module failed, manager is null\n");
        return FAILURE;
    }

    qc_manager_lock(manager);

    if (manager->module_num <= 0) {
        qc_manager_unlock(manager);
        return FAILURE;
    }

    qc_list_for_each_entry_safe(mod, next_mod, &manager->mod_list, list) {
        if (mod) {
            if (strncmp(mod->name, name, strlen(name)) == 0) {
                memcpy(back, mod, sizeof(struct qc_module));
                find_module = SUCCESS;
            }
        }
    }

    qc_manager_unlock(manager);

    return find_module;
}

int qc_manager_del_module(struct qc_manager *manager, char *name)
{
    struct qc_module *mod = NULL, *next_mod = NULL;

    if (manager == NULL) {
        printf("manager del module failed, manager is null\n");
        return FAILURE;
    }

    qc_manager_lock(manager);

    qc_list_for_each_entry_safe(mod, next_mod, &manager->mod_list, list) {
        if (mod) {
            if (strncmp(mod->name, name, strlen(mod->name)) == 0) {
                qc_list_del(&mod->list);
                qc_module_delete(mod);
                manager->module_num--;
            }
        }
    }

    qc_manager_unlock(manager);

    return SUCCESS;
}

static int qc_manager_module_check(struct qc_manager *manager, struct qc_module *module)
{
    struct qc_module *mod = NULL, *next_mod = NULL;

    qc_list_for_each_entry_safe(mod, next_mod, &manager->mod_list, list) {
        if (mod) {
            if (strncmp(mod->name, module->name, strlen(module->name)) == 0) {
                return SUCCESS;
            }
        }
    }

    return FAILURE;
}

static void qc_manager_module_sync(struct qc_manager *manager, struct qc_module *module)
{
    struct qc_module *mod = NULL, *next_mod = NULL;

    qc_list_for_each_entry_safe(mod, next_mod, &manager->mod_list, list) {
        if (mod) {
            if (strncmp(mod->name, module->name, strlen(module->name)) == 0) {
                memcpy(mod, module, sizeof(struct qc_module));
            }
        }
    }
}

int qc_manager_update(struct qc_manager *manager, struct qc_module *module)
{
    int module_run_num = 0;

    if (manager == NULL) {
        printf("manager update failed, manager is null\n");
        return FAILURE;
    }

    /* check if the module result has been written */
    if (qc_manager_module_check(manager, module) == FAILURE) {
        printf("module is not in manager, module name = %s\n", module->name);
        return FAILURE;
    }

    qc_manager_lock(manager);

    if (module->status == SUCCESS) {
        manager->success_num++;
        manager->success_rate = (manager->success_num * 100) / manager->module_num;
    } else if (module->status == FAILURE) {
        manager->failure_num++;
    }

    module_run_num = manager->success_num + manager->failure_num;
    manager->progress = (module_run_num * 100) / manager->module_num;
    if (manager->progress == 100) {
        manager->status = manager->success_rate == 100 ? SUCCESS : FAILURE;
    }

    qc_manager_module_sync(manager, module);

    qc_manager_unlock(manager);

    return SUCCESS;
}


struct qc_mod_exec *qc_mod_exec_create(void)
{
    struct qc_mod_exec *exec = NULL;

    exec = (struct qc_mod_exec *)malloc(sizeof(struct qc_mod_exec));
    if (exec == NULL) {
        printf("qc mod exec create failed, malloc exec failed\n");
        return NULL;
    }
    memset(exec, 0, sizeof(struct qc_mod_exec));

    return exec;
}

static int qc_mod_exec_init(struct qc_mod_exec *exec)
{
    if (exec == NULL) {
        printf("qc mod exec init failed, exec is null\n");
        return FAILURE;
    }
    memset(exec, 0, sizeof(struct qc_mod_exec));

    return SUCCESS;
}

int qc_mod_exec_delete(struct qc_mod_exec *exec)
{
    if (exec) {
        free(exec);
    }

    return SUCCESS;
}

int qc_mod_exec_set(struct qc_mod_exec *exec, char *mod_name, int sets, int exec_flag)
{
    if (exec == NULL || mod_name == NULL) {
        printf("exec add name failed, exec of mod_name is null\n");
        return FAILURE;
    }

    if ((exec_flag & (EXEC_CMD | EXEC_FUNC)) == 0) {
        printf("exec_flag para is illegality\n");
        return FAILURE;
    }

    if (sets < 0 || sets > EXEC_SETS -1) {
        printf("sets para is illegality\n");
        return FAILURE;
    }

    qc_mod_exec_init(exec);

    int name_len = strlen(mod_name);
    if (name_len > NAME_LEN - 1) {
        printf("name is too long, exceeding size = %d\n", NAME_LEN - 1 - NAME_LEN - 1);
        strncpy(exec->module_name, mod_name, NAME_LEN - 1);
    }

    strncpy(exec->module_name, mod_name, strlen(mod_name));

    exec->sets = sets;
    exec->exec_flag = exec_flag;

    return SUCCESS;
}

int qc_mod_exec_add_head(struct qc_mod_exec *exec, struct qc_manager *manager)
{
    if (manager == NULL || exec == NULL) {
        printf("qc mod exec add to head failed, manager is null or exec is null\n");
        return FAILURE;
    }

    struct qc_mod_exec *new_exec = qc_mod_exec_create();
    if (new_exec == NULL) {
        printf("qc mod exec add head failed, can't create exec\n");
        return FAILURE;
    }
    memcpy(new_exec, exec, sizeof(struct qc_mod_exec));

    qc_manager_lock(manager);

    qc_list_add_head(&new_exec->list, &manager->exec_list);

    manager->exec_num++;

    qc_manager_unlock(manager);

    return SUCCESS;
}

int qc_mod_exec_add_tail(struct qc_mod_exec *exec, struct qc_manager *manager)
{
    if (manager == NULL || exec == NULL) {
        printf("qc mod exec add to tail failed, manager is null or exec is null\n");
        return FAILURE;
    }

    struct qc_mod_exec *new_exec = qc_mod_exec_create();
    if (new_exec == NULL) {
        printf("qc mod exec add tail failed, can't create exec\n");
        return FAILURE;
    }
    memcpy(new_exec, exec, sizeof(struct qc_mod_exec));

    qc_manager_lock(manager);

    qc_list_add_tail(&new_exec->list, &manager->exec_list);

    manager->exec_num++;

    qc_manager_unlock(manager);

    return SUCCESS;
}

int qc_mod_exec_pop(struct qc_mod_exec *exec, struct qc_manager *manager)
{
    struct qc_mod_exec *list_exec = NULL, *next_list_exec = NULL;

    if (manager == NULL) {
        printf("qc mod exec pop failed, manager is null\n");
        return FAILURE;
    }

    qc_manager_lock(manager);
    if (manager->exec_num <= 0) {
        qc_manager_unlock(manager);
        return FAILURE;
    }

    qc_list_for_each_entry_safe(list_exec, next_list_exec, &manager->exec_list, list) {
        if (list_exec) {
            memcpy(exec, list_exec, sizeof(struct qc_mod_exec));
            break;
        }
    }

    manager->exec_num--;
    qc_list_del(&list_exec->list);

    qc_manager_unlock(manager);

    qc_mod_exec_delete(list_exec);

    return SUCCESS;
}

struct qc_disp *qc_disp_create(void)
{
    struct qc_disp *disp = NULL;

    disp = (struct qc_disp *)malloc(sizeof(struct qc_disp));
    if (disp == NULL) {
        printf("qc disp create failed, malloc qc_disp failed\n");
        return NULL;
    }
    memset(disp, 0, sizeof(struct qc_disp));

    return disp;
}

static int qc_disp_init(struct qc_disp *disp)
{
    if (disp == NULL) {
        printf("qc disp init failed, disp is null\n");
        return FAILURE;
    }
    memset(disp, 0, sizeof(struct qc_disp));

    return SUCCESS;
}

int qc_disp_delete(struct qc_disp *disp)
{
    if (disp) {
        free(disp);
    }

    return SUCCESS;
}

int qc_disp_set(struct qc_disp *disp, struct qc_manager *manager, struct qc_module *module)
{
    if (disp == NULL || manager == NULL || module == NULL) {
        printf("qc disp set failed, incorrect parameter input\n");
        return FAILURE;
    }

    qc_disp_init(disp);

    memcpy(disp->module_name, module->name, strlen(module->name));
    memcpy(disp->exec_sets_result, module->exec_sets_result, sizeof(int) * EXEC_SETS);
    memcpy(disp->exec_sets_desc, module->exec_sets_desc, sizeof(char) * EXEC_SETS * DESC_LEN);
    disp->module_status = module->status;

    memcpy(disp->manager_name, manager->name, strlen(manager->name));
    disp->progress = manager->progress;
    disp->success_num = manager->success_num;
    disp->failure_num = manager->failure_num;
    disp->success_rate = manager->success_rate;
    disp->manager_status = manager->status;

    return SUCCESS;
}

int qc_disp_add_head(struct qc_disp *disp, struct qc_manager *manager)
{
    if (manager == NULL || disp == NULL) {
        printf("qc disp add head failed, manager is null or disp is null\n");
        return FAILURE;
    }

    struct qc_disp *new_disp = qc_disp_create();
    if (new_disp == NULL) {
        printf("qc mod disp add head failed, can't create disp\n");
        return FAILURE;
    }
    memcpy(new_disp, disp, sizeof(struct qc_disp));

    qc_manager_lock(manager);

    qc_list_add_head(&new_disp->list, &manager->disp_list);

    manager->disp_num++;

    qc_manager_unlock(manager);

    return SUCCESS;
}

int qc_disp_add_tail(struct qc_disp *disp, struct qc_manager *manager)
{
    if (manager == NULL || disp == NULL) {
        printf("qc mod disp add tail failed, manager is null or disp is null\n");
        return FAILURE;
    }

    struct qc_disp *new_disp = qc_disp_create();
    if (new_disp == NULL) {
        printf("qc mod disp add tail failed, can't create disp\n");
        return FAILURE;
    }
    memcpy(new_disp, disp, sizeof(struct qc_disp));

    qc_manager_lock(manager);

    qc_list_add_tail(&new_disp->list, &manager->disp_list);

    manager->disp_num++;

    qc_manager_unlock(manager);

    return SUCCESS;
}

int qc_disp_pop(struct qc_disp *disp, struct qc_manager *manager)
{
    struct qc_disp *list_disp = NULL, *next_list_disp = NULL;

    if (manager == NULL) {
        printf("qc disp pop failed, manager is null\n");
        return FAILURE;
    }

    qc_manager_lock(manager);

    if (manager->disp_num <= 0) {
        qc_manager_unlock(manager);
        return FAILURE;
    }

    qc_list_for_each_entry_safe(list_disp, next_list_disp, &manager->disp_list, list) {
        if (list_disp) {
            memcpy(disp, list_disp, sizeof(struct qc_disp));
            break;
        }
    }

    manager->disp_num--;

    qc_list_del(&list_disp->list);

    qc_manager_unlock(manager);

    qc_disp_delete(list_disp);

    return SUCCESS;
}

#ifdef DEBUG_QC
void qc_module_debug(struct qc_module *module, int debug_flag)
{
    int i = 0, j = 0;

    if (debug_flag & MODULE_BASE) {
        printf("module name = %s, status = %d, id = %u",
            module->name, module->status, module->id);
    }

    if (debug_flag & MODULE_DESC) {
        printf("module name = %s, desc = %s\n",
            module->name, module->desc);
    }

    if (debug_flag & MODULE_CMDS) {
        for (i = 0; i < EXEC_SETS; i++) {
            for (j = 0; j < EXEC_NUM; j++) {
                if (strlen(module->exec_sets_cmd[i][j]) > 0) {
                    printf("module name = %s, exec_cmd[%d][%d] = %s\n",
                        module->name, i, j, module->exec_sets_cmd[i][j]);
                }
            }
        }
    }

    if (debug_flag & MODULE_FUNC) {
        for (i = 0; i < EXEC_SETS; i++) {
            for (j = 0; j < EXEC_NUM; j++) {
            if (module->exec_sets_func[i][j]) {
                    printf("module name = %s, exec_func[%d][%d] = %p\n",
                        module->name, i, j, module->exec_sets_func[i][j]);
                }
            }
        }
    }

    if (debug_flag & MODULE_EXECS_DESCRIBE) {
        for (i = 0; i < EXEC_SETS; i++) {
            if (strlen(module->exec_sets_desc[i]) > 0) {
                printf("module name = %s, desc[%d] = %s\n",
                    module->name, i, module->exec_sets_desc[i]);
            }
        }
    }

    if (debug_flag & MODULE_RESULT) {
        for (i = 0; i < EXEC_SETS; i++) {
            if (module->exec_sets_result[i] != UNEXECUTED) {
                printf("module name = %s, result[%d] = %d\n",
                    module->name, i, module->exec_sets_result[i]);
            }
        }
    }

    if (debug_flag & MODULE_RECORD) {
        for (i = 0; i < EXEC_SETS; i++) {
           if (module->exec_sets_record[i] != NOT_RECORD) {
                printf("module name = %s, record[%d] = %d\n",
                    module->name, i, module->exec_sets_record[i]);
           }
        }
    }
}

void qc_manager_debug(struct qc_manager *manager, int debug_flag)
{
    int i = 0;
    struct qc_mod_exec *exec = NULL, *next_exec = NULL;
    struct qc_module *mod = NULL, *next_mod = NULL;
    struct qc_disp *disp = NULL, *next_disp = NULL;

    if (manager == NULL) {
        printf("manager debug failed, manager is null\n");
    }

    if (debug_flag & MANAGER_BASE) {
        printf("manager name = %s, chip_id = %s, status = %d\n",
                manager->name, manager->chip_id, manager->status);
        printf("progress = %d, success_num = %d, failure_num = %d, success_rate = %d\n",
                manager->progress, manager->success_num,
                manager->failure_num, manager->success_rate);
    }

    if (debug_flag & MANAGER_MODULE_LIST) {
        printf("manager name = %s, module_num = %d\n",
                manager->name, manager->module_num);
        if (manager->module_num > 0) {
            qc_list_for_each_entry_safe(mod, next_mod, &manager->mod_list, list) {
                if (mod) {
                    if (debug_flag & MANAGER_MODULE_LIST_DETAIL) {
                        qc_module_debug(mod, MODULE_DEBUG_ALL);
                    } else {
                        printf("module name = %s\n", mod->name);
                    }
                }
            }
        }
    }

    if (debug_flag & MANAGER_EXEC_LIST) {
        printf("manager name = %s, exec_num = %d\n",
                manager->name, manager->exec_num);
        if (manager->exec_num > 0) {
            qc_list_for_each_entry_safe(exec, next_exec, &manager->exec_list, list) {
                if (exec != NULL) {
                    if (debug_flag & MANAGER_EXEC_LIST_DETAIL) {
                        printf("exec name = %s, sets = %d, exec_flag = %d\n",
                                exec->module_name, exec->sets, exec->exec_flag);
                    } else {
                        printf("exec name = %s \n", exec->module_name);
                    }
                }
            }
        }
    }

    if (debug_flag & MANAGER_DISP_LIST) {
        printf("manager name = %s, disp_num = %d\n",
                manager->name, manager->disp_num);

        if (manager->disp_num > 0) {
            qc_list_for_each_entry_safe(disp, next_disp, &manager->disp_list, list) {
                if (disp != NULL) {
                    if (debug_flag & MANAGER_DISP_LIST_DETAIL) {
                        printf("disp module_name = %s, module_status = %d\n",
                                disp->module_name, disp->module_status);

                        for (i = 0; i < EXEC_SETS; i++) {
                            printf("disp exec_sets_result[%d] = %d, exec_sets_desc[%d] = %s\n",
                                    i, disp->exec_sets_result[i], i, disp->exec_sets_desc[i]);
                        }

                        printf("disp manager name = %s, progress = %d, success_num = %d, failure_num = %d, success_rate = %d\n",
                               disp->manager_name, disp->progress, disp->success_num,
                               disp->failure_num, disp->success_rate);
                    } else {
                        printf("disp module name = %s, manager name = %s\n",
                        disp->module_name, disp->manager_name);
                    }
                }
            }
        }
    }

}
#endif
#endif
