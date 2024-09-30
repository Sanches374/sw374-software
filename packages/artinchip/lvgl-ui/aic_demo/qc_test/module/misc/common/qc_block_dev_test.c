/*
 * Copyright (c) 2022-2023, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors:  Zequan Liang <zequan.liang@artinchip.com>
 */

#include <stdio.h>
#include "qc_misc_common.h"

qc_res_t qc_block_dev_test_common(const char *filename, const char *content, int content_len)
{
    FILE *fp;

    fp = fopen(filename, "wb");
    if (fp == NULL) {
        return QC_RES_INV;
    }

    if (fwrite(content, 1, content_len, fp) != content_len) {
        fclose(fp);
        return QC_RES_INV;
    }

    if (fclose(fp) != 0) {
        return QC_RES_INV;
    }

    if (remove(filename) != 0) {
        return QC_RES_INV;
    }
    return QC_RES_OK;
}
