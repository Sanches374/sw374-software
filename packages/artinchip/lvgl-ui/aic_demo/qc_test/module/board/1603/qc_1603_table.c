/*
 * Copyright (c) 2023-2024, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors:  Zequan Liang <zequan.liang@artinchip.com>
 */

#include "../qc_board.h"

#if AIC_1603
#if QC_BOARD_D133B
char *board_script = "Board: D133BXX";
char *board_chip_intel_list[] = {
    "D133BAS",
    "D133BBS",
    NULL
};
#elif QC_BOARD_D133C == 1
char *board_script = "Board: D133CXX";
char *board_chip_intel_list[] = {
    "D133CBS",
    "D133CCS1",
    "D133CCS2",
    NULL
};
#elif QC_BOARD_D133E == 1
char *board_script = "Board: D133EXX";
char *board_chip_intel_list[] = {
    "D133EBS",
    "D133ECS1",
    "D133ECS2",
    NULL
};
#elif QC_BOARD_D132E == 1
char *board_script = "Board: D132EXX";
char *board_chip_intel_list[] = {
    "D132ENS",
    NULL
};
#elif QC_BOARD_G730C == 1
char *board_script = "Board: G730CXX";
char *board_chip_intel_list[] = {
    "G730CES",
    NULL
};
#elif QC_BOARD_G730E == 1
char *board_script = "Board: G730CXX";
char *board_chip_intel_list[] = {
    "G730EES",
    NULL
};
#elif QC_BOARD_G730B == 1
char *board_script = "Board: G730BXX";
char *board_chip_intel_list[] = {
    "G730BDU",
    NULL
};
#elif QC_BOARD_M6801 == 1
char *board_script = "Board: M6801XX";
char *board_chip_intel_list[] = {
    "M6801SPCS",
    NULL
};
#elif QC_BOARD_M6806 == 1
char *board_script = "Board: M6806XX";
char *board_chip_intel_list[] = {
    "M6806SPES",
    NULL
};
#elif QC_BOARD_DR128 == 1
char *board_script = "Board: DR128XX";
char *board_chip_intel_list[] = {
    "DR128",
    NULL
};
#elif QC_BOARD_JYX68 == 1
char *board_script = "Board: JYX68XX";
char *board_chip_intel_list[] = {
    "JYX68",
    NULL
};
#endif

#endif /* AIC_1603 */
