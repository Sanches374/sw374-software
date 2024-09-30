/*
 * Copyright (c) 2023-2024, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors:  Zequan Liang <zequan.liang@artinchip.com>
 */

#include "../qc_board.h"
#include "../common/common_test.h"
#if QC_BOARD_D133E
#if QC_OS_RT_THREAD
static qc_res_t d133e_gpai_0_cmp(int msh_result);
static qc_res_t d133e_gpai_1_cmp(int msh_result);
static qc_res_t d133e_gpai_2_cmp(int msh_result);
static qc_res_t d133e_gpai_3_cmp(int msh_result);
static qc_res_t d133e_gpai_4_cmp(int msh_result);
static qc_res_t d133e_gpai_5_cmp(int msh_result);
static qc_res_t d133e_gpai_6_cmp(int msh_result);
static qc_res_t d133e_gpai_7_cmp(int msh_result);

qc_res_t d133e_gpai_0_test(void *param)
{
    int msh_result;

    msh_result = qc_cmd_exec_ex("test_gpai -n 1 -c 0 -t 2.5", NULL, 0, 0, NULL, NULL);
    return d133e_gpai_0_cmp(msh_result);
}

qc_res_t d133e_gpai_1_test(void *param)
{
    int msh_result;

    msh_result = qc_cmd_exec_ex("test_gpai -n 1 -c 1 -t 2.5", NULL, 0, 0, NULL, NULL);
    return d133e_gpai_1_cmp(msh_result);
}

qc_res_t d133e_gpai_2_test(void *param)
{
    int msh_result;

    msh_result = qc_cmd_exec_ex("test_gpai -n 1 -c 2 -t 2.5", NULL, 0, 0, NULL, NULL);
    return d133e_gpai_2_cmp(msh_result);
}

qc_res_t d133e_gpai_3_test(void *param)
{
    int msh_result;

    msh_result = qc_cmd_exec_ex("test_gpai -n 1 -c 3 -t 2.5", NULL, 0, 0, NULL, NULL);
    return d133e_gpai_3_cmp(msh_result);
}

qc_res_t d133e_gpai_4_test(void *param)
{
    int msh_result;

    msh_result = qc_cmd_exec_ex("test_gpai -n 1 -c 4 -t 2.5", NULL, 0, 0, NULL, NULL);
    return d133e_gpai_4_cmp(msh_result);
}
qc_res_t d133e_gpai_5_test(void *param)
{
    int msh_result;

    msh_result = qc_cmd_exec_ex("test_gpai -n 1 -c 5 -t 2.5", NULL, 0, 0, NULL, NULL);
    return d133e_gpai_5_cmp(msh_result);
}

qc_res_t d133e_gpai_6_test(void *param)
{
    int msh_result;

    msh_result = qc_cmd_exec_ex("test_gpai -n 1 -c 6 -t 2.5", NULL, 0, 0, NULL, NULL);
    return d133e_gpai_6_cmp(msh_result);
}

qc_res_t d133e_gpai_7_test(void *param)
{
    int msh_result;

    msh_result = qc_cmd_exec_ex("test_gpai -n 1 -c 7 -t 2.5", NULL, 0, 0, NULL, NULL);
    return d133e_gpai_7_cmp(msh_result);
}

static qc_res_t d133e_gpai_0_cmp(int msh_result)
{
    float input_mv = (float)msh_result;
    return gpai_calculate_range_error_voltage(input_mv, 2.1429, 0.065);
}

static qc_res_t d133e_gpai_1_cmp(int msh_result)
{
    float input_mv = (float)msh_result;
    return gpai_calculate_range_error_voltage(input_mv, 1.7857, 0.065);
}

static qc_res_t d133e_gpai_2_cmp(int msh_result)
{
    float input_mv = (float)msh_result;
    return gpai_calculate_range_error_voltage(input_mv, 1.4268, 0.065);
}

static qc_res_t d133e_gpai_3_cmp(int msh_result)
{
    float input_mv = (float)msh_result;
    return gpai_calculate_percentage_error_voltage(input_mv, 1.0714, 0.015);
}

static qc_res_t d133e_gpai_4_cmp(int msh_result)
{
    float input_mv = (float)msh_result;
    return gpai_calculate_percentage_error_voltage(input_mv, 0.7143, 0.015);;
}

static qc_res_t d133e_gpai_5_cmp(int msh_result)
{
    float input_mv = (float)msh_result;
    return gpai_calculate_range_error_voltage(input_mv, 0.3571, 0.065);
}

static qc_res_t d133e_gpai_6_cmp(int msh_result)
{
    float input_mv = (float)msh_result;
    return gpai_calculate_range_error_voltage(input_mv, 1.6667, 0.065);
}

static qc_res_t d133e_gpai_7_cmp(int msh_result)
{
    float input_mv = (float)msh_result;
    return gpai_calculate_range_error_voltage(input_mv, 0.8333, 0.065);
}
#endif

#if QC_OS_RT_THREAD == 0 && QC_OS_LINUX == 0

qc_res_t d133e_gpai_0_test(void *param)
{
    return QC_RES_OK;
}

qc_res_t d133e_gpai_1_test(void *param)
{
    return QC_RES_OK;
}

qc_res_t d133e_gpai_2_test(void *param)
{
    return QC_RES_OK;
}

qc_res_t d133e_gpai_3_test(void *param)
{
    return QC_RES_OK;
}

qc_res_t d133e_gpai_4_test(void *param)
{
    return QC_RES_OK;
}
qc_res_t d133e_gpai_5_test(void *param)
{
    return QC_RES_OK;
}

qc_res_t d133e_gpai_6_test(void *param)
{
    return QC_RES_OK;
}

qc_res_t d133e_gpai_7_test(void *param)
{
    return QC_RES_OK;
}
#endif
#endif
