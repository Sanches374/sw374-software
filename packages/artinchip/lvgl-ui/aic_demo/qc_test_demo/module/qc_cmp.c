/*
 * Copyright (c) 2023-2024, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors:  ArtInChip
 */

#include "stdlib.h"
#include "string.h"
#include "stdio.h"

#include "qc_cmp.h"

#define GPAI_ERROR_RANG      200
#define RTC_ERROR_RANG       5

struct log_result_entry {
    char *name;
    int (*cmd_result)(int result);
};

static int cmd_result_analysis(int result)
{
    if (result == 0) {
        return SUCCESS;
    }

    return FAILURE;
}

int qc_result_analysis_common(int msh_result, char *name)
{
    int i = 0, num_entries = 0;
    static struct log_result_entry entries[] = {
        {"GPIO", cmd_result_analysis},
        {"GPAI", cmd_result_analysis},
        {"UART", cmd_result_analysis},
        {"CARD", cmd_result_analysis},
        {"CAN", cmd_result_analysis},
        {"USB", cmd_result_analysis},
        {"DSPK", cmd_result_analysis},
        {"MAC", cmd_result_analysis},
        {"RTC", cmd_result_analysis}
    };

    num_entries = sizeof(entries) / sizeof(entries[0]);

    for (i = 0; i < num_entries; i++) {
        if (strcmp(entries[i].name, name) == 0) {
            return entries[i].cmd_result(msh_result);
        }
    }

    return FAILURE;
}
