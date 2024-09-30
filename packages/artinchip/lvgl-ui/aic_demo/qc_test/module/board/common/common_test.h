/*
 * Copyright (c) 2023-2024, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors:  Zequan Liang <zequan.liang@artinchip.com>
 */

#include "../qc_board.h"

qc_res_t gpai_calculate_range_error_voltage(float input_voltage, float reference_voltage, float voltage);
qc_res_t gpai_calculate_percentage_error_voltage(float input_voltage, float reference_voltage, float percentage);

qc_res_t common_sdcard_module_test(void *param);
qc_res_t common_usb_module_test(void *param);
qc_res_t common_rtc_module_test_init(void *param);
qc_res_t common_rtc_module_test_start(void *param);
qc_res_t common_rtc_module_test(void *param);
qc_res_t common_mac_test_disp_testing(void *param);
qc_res_t common_mac_wait_start_up(void *param);
qc_res_t common_uart_test(void *param);
qc_res_t common_qc_save_chip(void *param);
qc_res_t common_compare_chip_intel_module(void *param);
