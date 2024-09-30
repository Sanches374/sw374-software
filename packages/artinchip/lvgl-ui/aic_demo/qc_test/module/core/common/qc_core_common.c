/*
 * Copyright (c) 2023-2024, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors:  Zequan Liang <zequan.liang@artinchip.com>
 */

#include "../qc_core.h"
#if QC_OS_RT_THREAD == 0 && QC_OS_LINUX == 0
#include <string.h>

void qc_module_list_ops_exec_cmd_common(qc_module_t *module, int list_ops, int list_ops_num, int *cmd_res_list)
{
    char cmd[256] = {0};
    int cmd_return = 0;
    int cmd_result = 0;
    qc_cap_cmp_int_t cmp_int;

    for (int i = 0; i < list_ops_num; i++) {
        strncpy(cmd, module->list_ops_cmd[list_ops][i], sizeof(cmd) - 1);
        cmp_int = module->list_ops_cmp_int[list_ops][i];

        cmd_return = qc_cmd_exec(cmd, NULL, 0, 0);
        cmd_result = cmp_int(cmd_return);

        /* save command run results */
        if (module->list_ops_cmd_record[list_ops][i] == QC_MOD_RECORD) {
            cmd_res_list[i] = cmd_result;
        }
    }
}
#endif
