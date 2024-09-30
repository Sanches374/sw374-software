/*
 * Copyright (c) 2023-2024, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors:  Zequan Liang <zequan.liang@artinchip.com>
 */
#include "../qc_board.h"
#include "../common/common_test.h"
#if QC_BOARD_D133C
#define BOARD_NAME "D133C"

extern qc_res_t d133c_gpai_0_test(void *param);
extern qc_res_t d133c_gpai_1_test(void *param);
extern qc_res_t d133c_gpai_2_test(void *param);
extern qc_res_t d133c_gpai_3_test(void *param);
extern qc_res_t d133c_gpai_4_test(void *param);
extern qc_res_t d133c_gpai_5_test(void *param);

qc_mod_init_base_t board_init_base[] = {
    {"SID",  BOARD_NAME"XX R comparison", NULL, 0},
    {"GPIO", "GPIO R-W comparison", NULL, 0},
    {"GPAI", "GPAI Read comparison", NULL, 0},
    {"CARD", "CARD R-W comparison", NULL, 0},
    {"DSPK", "DSPK Write comparison", NULL, 0},
    {"UART", "UART R-W comparison", NULL, 0},
    {"MAC", "MAC R-W comparison", NULL, 0},
    {"RTC", "RTC R-W comparison", NULL, 0},
    {"USB", "USB R-W comparison", NULL, 0},
    {"SAVE", "SAVE Results to Udisk", NULL, 0},
    {NULL, NULL}
};

qc_mod_init_list_pos_t board_init_list_pos[] = {
    {"SID", BOARD_NAME"XX R comparison", 0, NULL, common_compare_chip_intel_module, QC_MOD_NOT_RECORD, QC_MOD_RECORD, 0},

    {"GPIO", "GROUP0", 0, "test_gpio -t 10 -i PB.8 -o PB.9", NULL, QC_MOD_RECORD, QC_MOD_NOT_RECORD, 0},
    {"GPIO", "GROUP0", 0, "test_gpio -t 10 -i PB.9 -o PB.8", NULL, QC_MOD_RECORD, QC_MOD_NOT_RECORD, 0},
    {"GPIO", "GROUP1", 1, "test_gpio -t 10 -i PD.9 -o PD.10", NULL, QC_MOD_RECORD, QC_MOD_NOT_RECORD, 0},
    {"GPIO", "GROUP1", 1, "test_gpio -t 10 -i PD.10 -o PD.9", NULL, QC_MOD_RECORD, QC_MOD_NOT_RECORD, 0},
    {"GPIO", "GROUP2", 2, "test_gpio -t 10 -i PD.11 -o PD.12", NULL, QC_MOD_RECORD, QC_MOD_NOT_RECORD, 0},
    {"GPIO", "GROUP2", 2, "test_gpio -t 10 -i PD.12 -o PD.11", NULL, QC_MOD_RECORD, QC_MOD_NOT_RECORD, 0},
    {"GPIO", "GROUP3", 3, "test_gpio -t 10 -i PD.13 -o PD.14", NULL, QC_MOD_RECORD, QC_MOD_NOT_RECORD, 0},
    {"GPIO", "GROUP3", 3, "test_gpio -t 10 -i PD.14 -o PD.13", NULL, QC_MOD_RECORD, QC_MOD_NOT_RECORD, 0},
    {"GPIO", "GROUP4", 4, "test_gpio -t 10 -i PD.15 -o PD.16", NULL, QC_MOD_RECORD, QC_MOD_NOT_RECORD, 0},
    {"GPIO", "GROUP4", 4, "test_gpio -t 10 -i PD.16 -o PD.15", NULL, QC_MOD_RECORD, QC_MOD_NOT_RECORD, 0},
    {"GPIO", "GROUP5", 5, "test_gpio -t 10 -i PE.12 -o PE.13", NULL, QC_MOD_RECORD, QC_MOD_NOT_RECORD, 0},
    {"GPIO", "GROUP5", 5, "test_gpio -t 10 -i PE.13 -o PE.12", NULL, QC_MOD_RECORD, QC_MOD_NOT_RECORD, 0},

    {"GPAI", "GPAI0", 0, NULL, d133c_gpai_0_test, QC_MOD_NOT_RECORD, QC_MOD_RECORD, 0},
    {"GPAI", "GPAI1", 1, NULL, d133c_gpai_1_test, QC_MOD_NOT_RECORD, QC_MOD_RECORD, 0},
    {"GPAI", "GPAI2", 2, NULL, d133c_gpai_2_test, QC_MOD_NOT_RECORD, QC_MOD_RECORD, 0},
    {"GPAI", "GPAI3", 3, NULL, d133c_gpai_3_test, QC_MOD_NOT_RECORD, QC_MOD_RECORD, 0},
    {"GPAI", "GPAI4", 4, NULL, d133c_gpai_4_test, QC_MOD_NOT_RECORD, QC_MOD_RECORD, 0},
    {"GPAI", "GPAI5", 5, NULL, d133c_gpai_5_test, QC_MOD_NOT_RECORD, QC_MOD_RECORD, 0},

    {"CARD", "CARD", 0, NULL, common_sdcard_module_test, QC_MOD_NOT_RECORD, QC_MOD_RECORD, 0},

    {"DSPK", "DSPK", 0, "aplay sound0 /data/lvgl_data/qc-test/assets/commond/du.wav", NULL, QC_MOD_RECORD, QC_MOD_NOT_RECORD, 0},

    {"RTC", "RTC", 0, NULL, common_rtc_module_test_start, QC_MOD_NOT_RECORD, QC_MOD_RECORD, 0},
    {"RTC", "RTC", 1, NULL, common_rtc_module_test, QC_MOD_NOT_RECORD, QC_MOD_RECORD, 0},

    {"UART", "UART0", 0, "test_uart -u uart0 -t 2000", NULL, QC_MOD_RECORD, QC_MOD_NOT_RECORD, 0},
    {"UART", "UART2", 1, "test_uart -u uart2 -t 2000", NULL, QC_MOD_RECORD, QC_MOD_NOT_RECORD, 0},
    {"UART", "UART5", 2, "test_uart -u uart5 -t 2000", NULL, QC_MOD_RECORD, QC_MOD_NOT_RECORD, 0},
    {"UART", "UART7", 3, "test_uart -u uart7 -t 2000", NULL, QC_MOD_RECORD, QC_MOD_NOT_RECORD, 0},

    {"MAC", "MAC", 0, NULL, common_mac_test_disp_testing, QC_MOD_NOT_RECORD, QC_MOD_NOT_RECORD, 0},
    {"MAC", "MAC", 0, NULL, common_mac_wait_start_up, QC_MOD_NOT_RECORD, QC_MOD_NOT_RECORD, 0},
    {"MAC", "MAC", 0, "test_eth -d 10 -n 10", NULL, QC_MOD_RECORD, QC_MOD_NOT_RECORD, 0},

    {"USB", "USB", 0, NULL, common_usb_module_test, QC_MOD_NOT_RECORD, QC_MOD_RECORD, 0},

    {"SAVE", "SAVE Udisk", 0, NULL, common_qc_save_chip, QC_MOD_NOT_RECORD, QC_MOD_RECORD, 0},

    {NULL, NULL, 0, NULL, NULL, 0, 0, 0}
};

qc_exec_ops_order_init_t board_exec_order[] = {
    {"SID", 0},
    {"RTC", 0},
    {"GPIO", 0}, {"GPIO", 1}, {"GPIO", 2}, {"GPIO", 3}, {"GPIO", 4}, {"GPIO", 5},
    {"GPAI", 0}, {"GPAI", 1}, {"GPAI", 2}, {"GPAI", 3}, {"GPAI", 4}, {"GPAI", 5},
    {"CARD", 0},
    {"DSPK", 0},
    {"UART", 0}, {"UART", 1}, {"UART", 2}, {"UART", 3},
    {"MAC", 0},
    {"RTC", 1},
    {"USB", 0},
    {"SAVE", 0},
    {NULL, 0}
};

qc_save_init_t board_save_list[] = {
    {"/udisk/qc_save/"BOARD_NAME"/", USB_DEV, 0},
    {NULL, 0, 0}
};

qc_env_init_list_t board_list_env[] = {
    {NULL, common_rtc_module_test_init, 0},
    {NULL, NULL, 0}
};
#endif
