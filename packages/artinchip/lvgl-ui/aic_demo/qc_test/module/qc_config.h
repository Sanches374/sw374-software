/*
 * Copyright (c) 2023-2024, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors:  Zequan Liang <zequan.liang@artinchip.com>
 */

#define QC_TEST_TOOL_VERSION "V2.1.3"

/* Only one item can be selected from a large category */

/* operating system */
#define QC_OS_RT_THREAD 1
#define QC_OS_LINUX     0

/* AIC_1605 */
#define QC_BOARD_D12XX 0

/* AIC_1603 */
#define QC_BOARD_D133B 1
#define QC_BOARD_D133C 0
#define QC_BOARD_D133E 0
#define QC_BOARD_D132E 0
#define QC_BOARD_G730C 0
#define QC_BOARD_G730E 0
#define QC_BOARD_G730B 0
#define QC_BOARD_M6801 0
#define QC_BOARD_M6806 0
#define QC_BOARD_DR128 0
#define QC_BOARD_JYX68 0

/* auto config */
#if QC_BOARD_D133B || QC_BOARD_D133C || QC_BOARD_D132E || QC_BOARD_G730B || \
    QC_BOARD_D133E || QC_BOARD_G730C || QC_BOARD_G730E || QC_BOARD_M6801 || \
    QC_BOARD_M6806 || QC_BOARD_DR128 || QC_BOARD_JYX68
#define AIC_1603 1
#endif

#if QC_BOARD_D121B || QC_BOARD_TR230 || QC_BOARD_D122B || QC_BOARD_D12XX
#define AIC_1605 1
#endif
