/*
 * Copyright (c) 2023-2024, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors:  Zequan Liang <zequan.liang@artinchip.com>
 */

#include "../qc_board.h"

#if AIC_1605
#if QC_BOARD_D12XX
char *board_script = "Board: D12XXX";
char *board_chip_intel_list[] = {
    "D121BAV",
    "D121BBV",
    "D122BAV",
    "D122BBV",
    "D122BCV1",
    "D122BCV2",
    "TR230",
    "JYX58",
    NULL
};
#elif QC_BOARD_D121B == 1
char *board_script = "Board: D121BXX";
char *board_chip_intel_list[] = {
    "D121BAV",
    "D121BBV",
    NULL
};
#elif QC_BOARD_D122B == 1
char *board_script = "Board: D122BXX";
char *board_chip_intel_list[] = {
    "D122BAV",
    "D122BBV",
    "D122BCV1",
    "D122BCV2",
    NULL
};
#elif QC_BOARD_TR23X == 1
char *board_script = "Board: TR23XX";
char *board_chip_intel_list[] = {
    "TR230",
    NULL
};
#elif QC_BOARD_JYX58 == 1
char *board_script = "Board: JYXXX";
char *board_chip_intel_list[] = {
    "JYX58",
    NULL
};
#endif

#endif /* AIC_1605 */
