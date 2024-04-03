/*
 * Copyright (c) 2023-2024, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors:  ArtInChip
 */

#ifndef _QC_TEST_CORE_H_
#define _QC_TEST_CORE_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "qc_list.h"

#define DEBUG_QC

#define SUCCESS                     (1<<0)
#define FAILURE                     (1<<1)

/* exec recode flage */
#define RECORD                      (1<<2)
#define NOT_RECORD                  (1<<3)

/* exec flage */
#define EXEC_CMD                    (1<<6)
#define EXEC_FUNC                   (1<<7)

/* module status */
#define UNEXECUTED                  (1<<8)
#define EXECUTING                   (1<<9)
#define EXECUTED                    (1<<10)

#define UPDATED                     (1<<11)
#define NOT_UPDATED                 (1<<12)

#define EXEC_SETS                   8
#define EXEC_NUM                    5
#define EXEC_LEN                    100
#define NAME_LEN                    30
#define DESC_LEN                    30

#ifdef DEBUG_QC
#define MODULE_BASE                 (1<<0)
#define MODULE_DESC                 (1<<1)
#define MODULE_CMDS                 (1<<2)
#define MODULE_FUNC                 (1<<3)
#define MODULE_EXECS_DESCRIBE       (1<<4)
#define MODULE_RESULT               (1<<5)
#define MODULE_RECORD               (1<<6)
#define MODULE_RUNTIME              (1<<7)
#define MODULE_DEBUG_ALL            (0xFFFF)

#define MANAGER_BASE                (1<<8)
#define MANAGER_MODULE_LIST         (1<<9)
#define MANAGER_EXEC_LIST           (1<<10)
#define MANAGER_DISP_LIST           (1<<11)
#define MANAGER_MODULE_LIST_DETAIL  (1<<12)
#define MANAGER_EXEC_LIST_DETAIL    (1<<13)
#define MANAGER_DISP_LIST_DETAIL    (1<<14)
#define MANAGER_DEBUG_ALL           (0x7F00)
#endif

typedef int (*qc_cmp)(int msh_result);
typedef int (*qc_exec_func)(void *);
typedef void (*qc_lock)(void);

struct qc_module
{
    unsigned int id;
    int status;
    char name[NAME_LEN];
    char desc[DESC_LEN];
    char exec_sets_desc[EXEC_SETS][DESC_LEN];
    char exec_sets_cmd[EXEC_SETS][EXEC_NUM][EXEC_LEN];  /* execute in command mode */
    qc_exec_func exec_sets_func[EXEC_SETS][EXEC_NUM];   /* execute in function mode */
    void *func_para[EXEC_SETS][EXEC_NUM];
    qc_cmp exec_sets_cmp[EXEC_SETS][EXEC_NUM];
    int exec_sets_result[EXEC_SETS];
    int exec_sets_record[EXEC_SETS];
    int now_log_len;
    void *reserve;

    struct qc_list list;
};

struct qc_manager
{
    char chip_id[64 + 4]; /* chip id fmt:44-80-ce-b8-52-2f-c0-0c-06-0c-11-18-09-00-30-04 */
    char name[NAME_LEN];
    int status;
    int progress;
    int success_num;
    int failure_num;
    int success_rate;
    int module_num;
    int exec_num;
    int disp_num;
    qc_lock lock;    /* the way to access a mutex in a blocking manager */
    qc_lock un_lock; /* the way to access a mutex in a blocking manager */
    void *reserve;

    struct qc_list mod_list;
    struct qc_list exec_list;
    struct qc_list disp_list;
};

/* qc module execute list */
struct qc_mod_exec
{
    char module_name[NAME_LEN];
    int sets;
    int exec_flag;

    struct qc_list list;
};

struct qc_disp
{
    /* module data */
    char module_name[NAME_LEN];
    int module_status;
    int exec_sets_result[EXEC_SETS];
    char exec_sets_desc[EXEC_SETS][DESC_LEN];

    /* manager data */
    char manager_name[NAME_LEN];
    int manager_status;
    int progress;
    int success_num;
    int failure_num;
    int success_rate;

    struct qc_list list;
};

struct qc_module *qc_module_create(void);
int qc_module_init(struct qc_module *module);
int qc_module_clear_status(struct qc_module *module);
int qc_module_delete(struct qc_module *module);

int qc_module_add_name(struct qc_module *module, char *name);
int qc_module_add_desc(struct qc_module *module, char *desc);
int qc_module_add_sets_desc(struct qc_module *module, int exec_set, char *sets_desc);
int qc_module_add_cmd(struct qc_module *module, int exec_set, char *cmd, int exec_record);
int qc_module_add_func(struct qc_module *module, int exec_set, qc_exec_func func, void *func_para, int exec_record);
int qc_module_add_cmp(struct qc_module *module, int exec_set, qc_cmp f_cmp);
int qc_module_execute(struct qc_module *module, int exec_set, int exec_flag);

struct qc_manager *qc_manager_create(void);
int qc_manager_init(struct qc_manager *manager);
int qc_manager_clear_status(struct qc_manager *manager);
int qc_manager_delete(struct qc_manager *manager);

int qc_manager_add_name(struct qc_manager *manager, char *name);
int qc_manager_add_module(struct qc_manager *manager, struct qc_module *module);
int qc_manager_get_module(struct qc_manager *manager, char *name, struct qc_module *back);
int qc_manager_del_module(struct qc_manager *manager, char *name);
int qc_manager_add_lock(struct qc_manager *manager, qc_lock lock, qc_lock un_lock);
int qc_manager_update(struct qc_manager *manager, struct qc_module *module);

struct qc_mod_exec *qc_mod_exec_create(void);
int qc_mod_exec_delete(struct qc_mod_exec *exec);
int qc_mod_exec_set(struct qc_mod_exec *exec, char *mod_name, int sets, int exec_flag);
int qc_mod_exec_add_head(struct qc_mod_exec *exec, struct qc_manager *manager);
int qc_mod_exec_add_tail(struct qc_mod_exec *exec, struct qc_manager *manager);
int qc_mod_exec_pop(struct qc_mod_exec *exec, struct qc_manager *manager);

struct qc_disp *qc_disp_create(void);
int qc_disp_delete(struct qc_disp *disp);
int qc_disp_set(struct qc_disp *disp, struct qc_manager *manager, struct qc_module *module);
int qc_disp_add_head(struct qc_disp *disp, struct qc_manager *manager);
int qc_disp_add_tail(struct qc_disp *disp, struct qc_manager *manager);
int qc_disp_pop(struct qc_disp *disp, struct qc_manager *manager);

int qc_msh_exec(char *command, int length);

#ifdef DEBUG_QC
void qc_module_debug(struct qc_module *module, int debug_flag);
void qc_manager_debug(struct qc_manager *manager, int debug_flag);
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _QC_TEST_CORE_H_ */
