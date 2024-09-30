/*
 * Copyright (c) 2023-2024, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors:  Zequan Liang <zequan.liang@artinchip.com>
 */

#if QC_OS_LINUX
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/select.h>
#include <sys/time.h>
#include <signal.h>
#include "../qc_core.h"

static int _popen_close_and_get_status(FILE *fp);
static void _kill_process(char *command);
static int _qc_cmd_exec_linux(char *command, char *buf,
                            int buf_len, unsigned int runtime,
                            qc_cap_cmp_str_t cmp_str, qc_res_t *cmd_res);

int qc_cmd_exec_linux(char *command, char *buf,
                        int buf_len, unsigned int runtime)
{
    int _runtime = runtime;
    qc_res_t cmd_res = QC_RES_INV;

    return _qc_cmd_exec_linux(command, buf, buf_len, &_runtime, NULL, &cmd_res);
}

int qc_cmd_exec_linux_ex(char *command, char *buf,
                        int buf_len, unsigned int runtime,
                        qc_cap_cmp_str_t cmp_str, qc_res_t *cmd_res)
{
    int _runtime = runtime;
    qc_res_t cmd_res = QC_RES_INV;

    return _qc_cmd_exec_linux(command, buf, buf_len, &_runtime, cmp_str, &cmd_res);
}

qc_res_t qc_module_list_ops_exec_cmd_linux(qc_module_t *module, int list_ops, int list_pos_num, int *cmd_res_list)
{
    qc_res_t cmd_str_res = QC_RES_INV;
    qc_cmp_int_t cmp_int = NULL;
    qc_cmp_str_t cmp_str = NULL;
    qc_list_ops_cmd_t cmd[128] = {0};
    int cmd_return = 0;
    int cmd_result = 0;
    int runtime = -1;

    runtime = module->list_ops_runtime[list_ops];
    for (int i = 0; i < list_pos_func_num; i++) {
        strncpy(cmd, module->list_ops_cmd[list_ops][i], sizeof(cmd));
        cmp_int = module->list_ops_cmp_int[list_ops][i];
        cmp_str = module->list_ops_cmp_str[list_ops][i];

        cmd_return = _qc_cmd_exec_linux(cmd, module->log_buf, module->log_buf_len, &runtime, cmp_str, &cmd_str_res);
        if (cmd_str_res != QC_RES_OK)
            cmd_result = cmp_int(cmd_return);

        /* save command run results */
        if (module->list_ops_cmd_record[list_ops][i] == QC_MOD_RECORD) {
            /* due to abnormal process shutdown, the return value is uncertain and
             * the command is not used to return the value.
            */
            cmd_res_list[i] = cmd_str_res;
        }
    }
}

static int _popen_close_and_get_status(FILE *fp)
{
    int status;
    int exit_status;
    if (fp)
        status = pclose(fp);

    if (_WIFEXITED(status)) {
        exit_status = _WEXITSTATUS(status);
    } else {
        exit_status = _WTERMSIG(status);
    }
    return exit_status;
}

static void _kill_process(char *command)
{
    char kill_cmd[256] = {0};
    snprintf(kill_cmd, sizeof(kill_cmd), "pkill -f \"*%s\"", command);
    system(kill_cmd);
}

static int _qc_cmd_exec_linux(char *command, char *buf,
                            int buf_len, unsigned int *runtime,
                            qc_cap_cmp_str_t cmp_str, qc_res_t *cmd_res)
{
    if (strlen(command) == 0)
        return QC_RES_INV;

    *cmd_res = QC_RES_INV;
    int *remain_time_ms = runtime;
    int loop = 0;
    if (*remain_time_ms <= 0)
        loop = 1;

    FILE *fp = popen(command, "r");
    if (fp == NULL)
        return QC_RES_INV;

    int fd = fileno(fp);
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    if (fd < 0) {
        pclose(fp);
        return QC_RES_INV;
    }

    char buffer[256] = {0};
    int remain_buf_len = buf_len;
    while(1) {
        struct timeval start, end;
        gettimeofday(&start, NULL);

        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(fd, &read_fds);

        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 50000;
        int result = select(fd + 1, &read_fds, NULL, NULL, &timeout);
        if (result) {
            if (fgets(buffer, sizeof(buffer), fp) != NULL) {
                if (buf != NULL && remain_buf_len > 0)
                    strncat(buf, buffer, buf_len);
                    remain_buf_len -= strlen(buffer);
                if (cmp_str != NULL && cmp_str(buffer) == QC_RES_OK) {
                    *cmd_res = QC_RES_OK;
                    return _popen_close_and_get_status();
                }
            }
        } else if (result == -1) {
            break;
        } /* result == 0, timeout */

        if (loop == 1)
            continue;

        gettimeofday(&end, NULL);
        int timeout_ms = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000;
        *remain_time_ms -= (int)timeout_ms;
        if (*remain_time_ms <= 0) {
            _kill_process(command);
            break;
        }
    }

    return _popen_close_and_get_status();
}

#endif
