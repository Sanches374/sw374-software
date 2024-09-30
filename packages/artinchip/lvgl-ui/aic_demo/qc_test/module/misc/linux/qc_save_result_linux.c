/*
 * Copyright (c) 2023-2024, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors:  Zequan Liang <zequan.liang@artinchip.com>
 */

#if QC_OS_LINUX
#include <string.h>
#include "qc_misc_linux.h"

/*
 * file.csv name:chipid.csv
 * file.csv context:
 * chip_id,status,success_rate,success_num,failure_num,module_num
 * XXXXXX,XXXXX,XXXXX,XXXXX,XXXXX,XXXXX,XXXXX
 *
 * module_name,status
 * XXXXXX,XXXXX
 * XXXXXX,XXXXX
 * XXXXXX,XXXXX
 * XXXXXX,XXXXX
 *
*/

enum {
    USB_DEV = 0,
    SDCARD_DEV,
};
typedef int qc_save_dev_t;

typedef struct _qc_save {
    qc_board_config_t *board;
    const char *path;
    qc_save_dev_t dev;
    int blk_num;
} qc_save_t;

qc_save_t save_list[] = {
#if QC_BOARD_D133C
    {NULL, "udisk/qc_save/d13c/", USB_DEV, 0},
#endif

#if QC_BOARD_D213C
    {NULL, "udisk/qc_save/d21c/", USB_DEV, 0},
    {NULL, "udisk/qc_save/d21c/", USB_DEV, 1},
#endif
    {NULL, NULL, 0, 0}
};


qc_res_t _qc_save_write_linux(qc_board_config_t *board, const char *path, qc_save_dev_t dev, int blk_num)
{
    return QC_RES_OK;
}

qc_res_t qc_save_write_linux(qc_board_config_t *board)
{
    int save_time = 0;
    while(1) {
        if (strlen(save_list[save_time]) != 0) {
            save_time++;
        } else {
            break;
        }
    }

    int failure_time = 0;
    for (int i = 0;i < save_time; i++) {
        if (_qc_save_write_linux(board, save_list[i].path, save_list[i].dev, save_list[i].blk_num) == QC_RES_INV)
            failure_time++;
    }

    if (failure_time)
        return QC_RES_INV;
    return QC_RES_OK;
}
#endif
