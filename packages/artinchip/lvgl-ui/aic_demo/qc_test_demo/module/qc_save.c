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
#include <getopt.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "qc_save.h"
#include "qc_ops.h"

#define PATH_LEN    256
#define CHIP_ID_PREFIX "ID_"

static int create_dir(char *path)
{
    struct stat st = {0};
    char temp_path[PATH_LEN + 1] = {0};
    char *ptr = NULL;

    if (path == NULL) {
        printf("create_dir failed, path is null\n");
        return FAILURE;
    }
    strncpy(temp_path, path, PATH_LEN);

    ptr = temp_path + 1;
    while ((ptr = strchr(ptr, '/')) != NULL) {
        *ptr = '\0';
        if (stat(temp_path, &st) != 0) {
            if (mkdir(temp_path, 0755) != 0) {
                printf("Failed to create directory %s\n", temp_path);
                return FAILURE;
            }
        }
        *ptr++ = '/';
    }
    return SUCCESS;
}

int qc_save_read(struct qc_manager *mgr, char *path)
{
    return SUCCESS;
}

int qc_save_write(struct qc_manager *mgr, char *path)
{
    int fd = -1;
    int write_bytes = -1;
    static char line[256] = {0};
    static char status[20] = {0};
    static char file_name[PATH_LEN];
    char *mgr_head = "chip_id,status,success_rate,success_num,failure_num,module_num\n";
    char *module_head = "module_name,status\n";
    struct qc_module *mod = NULL, *next_mod = NULL;

    if (mgr == NULL && (strlen(mgr->chip_id) == 0)) {
        printf("qc_save_write is error, manager is null of chip id error\n");
        return FAILURE;
    }

    if (create_dir(path) == FAILURE) {
        printf("create dir failed, path = %s\n", path);
    }

    /* get save file name */
    snprintf(file_name, sizeof(file_name), "%s%s%s.csv",
             path, CHIP_ID_PREFIX, mgr->chip_id);

    fd = open(file_name, O_WRONLY | O_CREAT);
    if (fd < 0) {
        printf("can't created files, path = %s\n", file_name);
        return FAILURE;
    }

    if (mgr->status == SUCCESS) {
        strcpy(status, "SUCCESS");
    } else {
        strcpy(status, "FAILURE");
    }

    write_bytes = write(fd, mgr_head, strlen(mgr_head));
    if (write_bytes != strlen(mgr_head)) {
        printf("write mgr_head error\n");
        goto qc_write_exit;
    }
    snprintf(line, sizeof(line) - 1,"%s,%s,%d,%d,%d,%d\n\n",
            mgr->chip_id, status, mgr->success_rate, mgr->success_num, mgr->failure_num, mgr->module_num);
    write_bytes = write(fd, line, strlen(line));
    if (write_bytes != strlen(line)) {
        printf("write mgr context error\n");
        goto qc_write_exit;
    }

    if (mgr->module_num <= 0) {
        close(fd);
        goto qc_write_exit;
    }

    write_bytes = write(fd, module_head, strlen(module_head));
    if (write_bytes != strlen(module_head)) {
        printf("write module_head error\n");
        goto qc_write_exit;
    }

    qc_list_for_each_entry_safe(mod, next_mod, &mgr->mod_list, list) {
        if (mod) {
            if (mod->status == SUCCESS) {
                strcpy(status, "SUCCESS");
            } else {
                strcpy(status, "FAILURE");
            }
            snprintf(line, sizeof(line) - 1,"%s,%s\n", mod->name, status);

            write_bytes = write(fd, line, strlen(line));
            if (write_bytes != strlen(line)) {
                printf("write module context error\n");
                goto qc_write_exit;
            }
        }
    }

    close(fd);
    return SUCCESS;

qc_write_exit:
    close(fd);
    return FAILURE;
}

#endif
