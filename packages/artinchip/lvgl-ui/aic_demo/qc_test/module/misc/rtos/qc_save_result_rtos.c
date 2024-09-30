/*
 * Copyright (c) 2023-2024, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors:  Zequan Liang <zequan.liang@artinchip.com>
 */

#include <string.h>
#include "qc_misc_rtos.h"

/*
 * file.csv name:chipid.csv
 * file.csv context:
 * chip_id,status,success_rate,success_num,QC_RES_INV_num,module_num
 * XXXXXX,XXXXX,XXXXX,XXXXX,XXXXX,XXXXX,XXXXX
 *
 * module_name,status
 * XXXXXX,XXXXX
 * XXXXXX,XXXXX
 * XXXXXX,XXXXX
 * XXXXXX,XXXXX
 *
*/
#if QC_OS_RT_THREAD
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#define PATH_LEN    256
#define CHIP_ID_PREFIX "ID_"

extern qc_save_init_t board_save_list[];

extern qc_res_t qc_save_result_common(qc_board_config_t *board, const char *path);

qc_res_t qc_save_result_rtos(qc_board_config_t *board)
{
    int i = 0;
    int failure_time = 0;

    while (board_save_list[i].path != NULL) {
        if (qc_save_result_common(board, board_save_list[i].path) == QC_RES_INV)
            failure_time++;
        i++;
    }

    if (failure_time)
        return QC_RES_INV;
    return QC_RES_OK;
}

#endif
