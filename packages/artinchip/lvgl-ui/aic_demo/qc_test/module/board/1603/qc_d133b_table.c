/*
 * Copyright (c) 2023-2024, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors:  Zequan Liang <zequan.liang@artinchip.com>
 */
#include "../qc_board.h"
#include "../common/common_test.h"

#if QC_BOARD_D133B
#define BOARD_NAME "D133B"

extern qc_res_t d133b_gpai_0_test(void *param);
extern qc_res_t d133b_gpai_1_test(void *param);
extern qc_res_t d133b_gpai_4_test(void *param);
extern qc_res_t d133b_gpai_5_test(void *param);

qc_mod_init_base_t board_init_base[] = {
    {"SID",  BOARD_NAME"XX R comparison", NULL, 0},
    {"GPIO", "GPIO R-W comparison", NULL, 0},
    {"GPAI", "GPAI Read comparison", NULL, 0},
    {"CARD", "CARD R-W comparison", NULL, 0},
    {"DSPK", "DISP Write comparison", NULL, 0},
    {"UART", "UART R-W comparison", NULL, 0},
    {"SAVE", "SAVE Results to Sd card", NULL, 0},
    {NULL, NULL}
};

qc_mod_init_list_pos_t board_init_list_pos[] = {
    {"SID", BOARD_NAME"XX R comparison", 0, NULL, common_compare_chip_intel_module,QC_MOD_NOT_RECORD, QC_MOD_RECORD, 0},

    {"GPIO", "GROUP2", 0, "test_gpio -t 10 -i PB.8 -o PB.9", NULL, QC_MOD_RECORD, QC_MOD_NOT_RECORD, 0},
    {"GPIO", "GROUP2", 0, "test_gpio -t 10 -i PB.9 -o PB.8", NULL, QC_MOD_RECORD, QC_MOD_NOT_RECORD, 0},
    {"GPIO", "GROUP5", 1, "test_gpio -t 10 -i PB.10 -o PB.11", NULL,QC_MOD_RECORD, QC_MOD_NOT_RECORD, 0},
    {"GPIO", "GROUP5", 1, "test_gpio -t 10 -i PB.11 -o PB.10", NULL,QC_MOD_RECORD, QC_MOD_NOT_RECORD, 0},
    {"GPIO", "GROUP6", 2, "test_gpio -t 10 -i PB.6 -o PB.7", NULL, QC_MOD_RECORD, QC_MOD_NOT_RECORD, 0},
    {"GPIO", "GROUP6", 2, "test_gpio -t 10 -i PB.7 -o PB.6", NULL, QC_MOD_RECORD, QC_MOD_NOT_RECORD, 0},

    {"GPAI", "GPAI0", 0, NULL, d133b_gpai_0_test, QC_MOD_NOT_RECORD, QC_MOD_RECORD, 0},
    {"GPAI", "GPAI1", 1, NULL, d133b_gpai_1_test, QC_MOD_NOT_RECORD, QC_MOD_RECORD, 0},
    {"GPAI", "GPAI4", 2, NULL, d133b_gpai_4_test, QC_MOD_NOT_RECORD, QC_MOD_RECORD, 0},
    {"GPAI", "GPAI5", 3, NULL, d133b_gpai_5_test, QC_MOD_NOT_RECORD, QC_MOD_RECORD, 0},

    {"CARD", "CARD", 0, NULL, common_sdcard_module_test, QC_MOD_NOT_RECORD, QC_MOD_RECORD, 0},

    {"DSPK", "DSPK", 0, "aplay sound0 /data/lvgl_data/qc-test/assets/commond/du.wav", NULL, QC_MOD_RECORD, QC_MOD_NOT_RECORD, 0},

    {"UART", "UART1", 0, NULL, common_uart_test, QC_MOD_RECORD, QC_MOD_NOT_RECORD, 0},

    {"SAVE", "SAVE Udisk", 0, NULL, common_qc_save_chip, QC_MOD_NOT_RECORD, QC_MOD_RECORD, 0},

    {NULL, NULL, 0, NULL, NULL, 0, 0, 0}
};

qc_exec_ops_order_init_t board_exec_order[] = {
    {"SID", 0},
    {"GPIO", 0}, {"GPIO", 1}, {"GPIO", 2},
    {"GPAI", 0}, {"GPAI", 1}, {"GPAI", 2}, {"GPAI", 3},
    {"CARD", 0},
    {"DSPK", 0},
    {"UART", 0},
    {"SAVE", 0},
    {NULL, 0}
};

qc_save_init_t board_save_list[] = {
    {"/sdcard/qc_save/"BOARD_NAME"/", SDCARD_DEV, 0},
    {NULL, 0, 0}
};

qc_env_init_list_t board_list_env[] = {
    {NULL, NULL, 0}
};
#endif
