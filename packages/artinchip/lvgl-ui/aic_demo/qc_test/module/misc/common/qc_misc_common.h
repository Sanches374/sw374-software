/*
 * Copyright (c) 2023-2024, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors:  Zequan Liang <zequan.liang@artinchip.com>
 */

#include "../qc_misc.h"
#include "../../board/qc_board.h"

qc_res_t qc_block_dev_test_common(const char *file, const char *content, int content_len);
qc_res_t qc_save_result_common(qc_board_config_t *board, const char *path);
qc_res_t qc_read_chip_intel_model_file_common(char *chip_model, int len, const char *file);
qc_res_t qc_write_chip_intel_module_file_common(char *chip_model, const char *file);
