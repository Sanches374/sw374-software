/*
 * Copyright (c) 2023-2024, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors:  Zequan Liang <zequan.liang@artinchip.com>
 */

#include "qc_misc.h"
#include "./common/qc_misc_common.h"
#include "../board/qc_board.h"
#if QC_OS_RT_THREAD
#include "./rtos/qc_misc_rtos.h"
#endif
#if QC_OS_LINUX
#include "./rtos/qc_misc_linux.h"
#endif

qc_res_t qc_save_write(void *board)
{
#if QC_OS_RT_THREAD
    return qc_save_result_rtos(board);
#endif
#if QC_OS_LINUX
    return qc_save_result_linux(board);
#endif

    return qc_save_result_common(board, "result");
}

qc_res_t qc_read_chip_id(char *chip_id, int len)
{
#if QC_OS_RT_THREAD
    return qc_read_chip_id_rtos(chip_id, len);
#endif
#if QC_OS_LINUX
    return qc_read_chip_id_linux(chip_id, len);
#endif
    return QC_RES_INV;
}

qc_res_t qc_read_chip_intel_model(char *chip_model, int len)
{
#if QC_OS_RT_THREAD
    return qc_read_chip_intel_model_rtos(chip_model, len);
#endif
#if QC_OS_LINUX
    return qc_read_chip_intel_model_linux(chip_model, len);
#endif
    return QC_RES_INV;
}

qc_res_t qc_read_chip_intel_model_file(char *chip_model, int len)
{
#if QC_OS_RT_THREAD
    return qc_read_chip_intel_model_file_common(chip_model, len, "/data/chip_model.txt");
#endif
#if QC_OS_LINUX
    return qc_read_chip_intel_model_file_linux(chip_model, len, "tmp/chip_model.txt");
#endif
#if QC_OS_RT_THREAD == 0 && QC_OS_LINUX == 0
    return qc_read_chip_intel_model_file_common(chip_model, len, "chip_model.txt");
#endif
}

qc_res_t qc_write_chip_intel_module_file(char *chip_model)
{
#if QC_OS_RT_THREAD
    return qc_write_chip_intel_module_file_common(chip_model, "/data/chip_model.txt");
#endif
#if QC_OS_LINUX
    return qc_write_chip_intel_module_file_linux(chip_model, "data/chip_model.txt");
#endif
#if QC_OS_RT_THREAD == 0 && QC_OS_LINUX == 0
    return qc_write_chip_intel_module_file_common(chip_model, "chip_model.txt");
#endif
}

qc_res_t qc_block_dev_test(const char *file, const char *content, int content_len)
{
    return qc_block_dev_test_common(file, content, content_len);
}
