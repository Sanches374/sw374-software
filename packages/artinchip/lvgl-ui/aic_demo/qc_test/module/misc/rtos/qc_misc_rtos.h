/*
 * Copyright (c) 2023-2024, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors:  Zequan Liang <zequan.liang@artinchip.com>
 */

#include "../../board/qc_board.h"
#include "../qc_misc.h"

qc_res_t qc_save_result_rtos(qc_board_config_t *board);
qc_res_t qc_read_chip_id_rtos(char *chip_id, int len);
qc_res_t qc_read_chip_intel_model_rtos(char *chip_model, int len);
qc_res_t qc_read_chip_intel_model_file_rtos(char *chip_model, int len, const char *file);
qc_res_t qc_write_chip_intel_module_file_rtos(char *chip_model, const char *file);
