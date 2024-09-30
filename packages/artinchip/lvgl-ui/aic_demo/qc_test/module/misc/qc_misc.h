/*
 * Copyright (c) 2022-2023, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors:  Zequan Liang <zequan.liang@artinchip.com>
 */

#ifndef _QC_TEST_OPS_H_
#define _QC_TEST_OPS_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "../qc_config.h"
#include "../core/qc_core.h"

qc_res_t qc_block_dev_test(const char *file, const char *content, int content_len);

qc_res_t qc_read_chip_id(char *chip_id, int len);
qc_res_t qc_read_chip_intel_model(char *chip_model, int len); /* read the actual model */
qc_res_t qc_read_chip_intel_model_file(char *chip_model, int len); /* read the from the save */
qc_res_t qc_write_chip_intel_module_file(char *chip_model);

qc_res_t qc_save_write(void *board);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _QC_BOARD_CFG_H_ */
