/*
 * Copyright (c) 2023-2024, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors:  Zequan Liang <zequan.liang@artinchip.com>
 */

#include "qc_misc_common.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#define PATH_LEN    256
#define CHIP_ID_PREFIX "ID_"

static char* extract_directory(const char* path);
static void set_status(char *status, int failure_num)
{
    if (failure_num == 0) {
        strcpy(status, "SUCCESS");
    } else {
        strcpy(status, "FAILURE");
    }
}

static qc_res_t create_dir(const char *path)
{
    struct stat st = {0};
    char temp_path[PATH_LEN + 1] = {0};
    char *ptr = NULL;

    if (path == NULL) {
        return QC_RES_INV;
    }
    strncpy(temp_path, path, PATH_LEN);
    ptr = temp_path;
#if defined(_WIN32) || defined(_WIN64)
    while ((ptr = strchr(ptr, '\\')) != NULL) {
        *ptr = '\0';
        if (stat(temp_path, &st) != 0) {
            if (mkdir(temp_path) != 0) {
                return QC_RES_INV;
            }
        }
        *ptr++ = '\\';
    }
#else
    while ((ptr = strchr(ptr, '/')) != NULL) {
        *ptr = '\0';
        if (stat(temp_path, &st) != 0) {
            if (mkdir(temp_path, 0755) != 0) {
                return QC_RES_INV;
            }
        }
        *ptr++ = '/';
    }
#endif
    return QC_RES_OK;
}

qc_res_t qc_save_result_common(qc_board_config_t *board, const char *path)
{
    FILE *fp = NULL;
    char file_name[PATH_LEN] = {0};
    char file_path[PATH_LEN] = {0};
    char status[20] = {0};

    if (board == NULL || path == NULL) {
        return QC_RES_INV;
    }

#if defined(_WIN32) || defined(_WIN64)
    snprintf(file_path, sizeof(file_path), "%s%s\\", path, board->chip_intel_module);
#else
    snprintf(file_path, sizeof(file_path), "%s%s/", path, board->chip_intel_module);
#endif
    /* get save file name */
    snprintf(file_name, sizeof(file_name), "%s%s%s.csv", file_path, CHIP_ID_PREFIX, board->chip_id);

    /* check if the file is exist */
    struct stat buffer;
    int file_exist = 0;
    if (stat(file_name, &buffer) == 0) {
        file_exist = 1;
    }

    char *extract_path = extract_directory(file_path);
    create_dir(extract_path);

    if (file_exist == 0) {
        fp = fopen(file_name, "w");
    } else {
        fp = fopen(file_name, "a");
    }
    if (fp == NULL) {
        return QC_RES_INV;
    }

    /* write version */
    if (file_exist == 0)
        fprintf(fp, "Version: %s\n\n", QC_TEST_TOOL_VERSION);
    else
        fprintf(fp, "\n\n");
    /* write head */
    fprintf(fp, "The actual results are as follows:\n");

    /* write head */
    qc_manager_t *mgr = board->manager;
    set_status(status, mgr->failure_num);
    fprintf(fp, "chip_id,status,success_rate,success_num,failure_num,module_num\n");
    fprintf(fp, "%s,%s,%d,%d,%d,%d\n\n",
            board->chip_id, status, mgr->success_rate, mgr->success_num, mgr->failure_num, mgr->module_num);

    /* write module */
    fprintf(fp, "module_name,status\n");
    qc_module_t *mod = qc_module_create();
    for (int i = 0; i < mgr->module_num; i++) {
        qc_manager_module_get_index(mgr, mod, i);
        set_status(status, mod->status != QC_MOD_SUCCESS);
        fprintf(fp, "%s,%s\n", mod->name, status);
    }

    fflush(fp);
    fclose(fp);
    qc_module_delete(mod);

    return QC_RES_OK;
}

qc_res_t qc_read_chip_intel_model_file_common(char *chip_model, int len, const char *file)
{
    FILE *fp = fopen(file, "r");
    if (fp == NULL) {
        return QC_RES_INV;
    }

    if (fgets(chip_model, len, fp) != NULL) {
        size_t n = strlen(chip_model);
        if (n > 0 && chip_model[n-1] == '\n') {
            chip_model[n-1] = '\0';
        }
        fclose(fp);
        return 0;
    } else {
        fclose(fp);
        return QC_RES_INV;
    }

    return QC_RES_OK;
}

qc_res_t qc_write_chip_intel_module_file_common(char *chip_model, const char *file)
{
    char *path = extract_directory(file);
    create_dir(path);

    FILE *fp = fopen(file, "w");
    if (fp == NULL) {
        return QC_RES_INV;
    }

    if (fprintf(fp, "%s", chip_model) >= 0) {
        fclose(fp);
        return QC_RES_OK;
    } else {
        fclose(fp);
        return QC_RES_INV;
    }
    return  QC_RES_OK;
}

static char* extract_directory(const char* path)
{
    const char* last_slash = strrchr(path, '/');
#if defined(_WIN32) || defined(_WIN64)
    const char* last_backslash = strrchr(path, '\\');
    if (last_backslash && (!last_slash || last_backslash > last_slash)) {
        last_slash = last_backslash;
    }
#endif

    if (last_slash) {
        return (char*)path + (path - path) / sizeof(char) * (last_slash - path + 1);
    } else {
        return NULL;
    }
}
