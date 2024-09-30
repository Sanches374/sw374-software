/*
 * Copyright (c) 2022-2023, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors:  Zequan Liang <zequan.liang@artinchip.com>
 */

#ifndef _QC_CHIP_BURN_OPS_H_
#define _QC_CHIP_BURN_OPS_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "qc_misc_rtos.h"

#define BITS_PER_LONG       32
#define BIT(s) (1U << (s))
#define GENMASK(h, l)       (((~(0U)) - ((1U) << (l)) + 1) & \
                            (~(0U) >> (BITS_PER_LONG - 1 - (h))))

#ifdef AIC_1603
#define CHIID_MAIN_BASE 0x19010210
#define CHIP_LOCK_BASE  0x19010220

#define CHIPID_MAIN_NVM1 (CHIID_MAIN_BASE + 0X0)
#define CHIPID_MAIN_NVM2 (CHIID_MAIN_BASE + 0X4)
#define CHIPID_MAIN_NVM3 (CHIID_MAIN_BASE + 0X8)
#define CHIPID_MAIN_NVM4 (CHIID_MAIN_BASE + 0XC)

#define CHIP_LOCK_NVM1   (CHIP_LOCK_BASE + 0X0)
#define CHIP_LOCK_NVM2   (CHIP_LOCK_BASE + 0X4)

#define PACKAGE_ID_MASK    GENMASK(15, 8)
#define MARK_ID_MASK       GENMASK(7, 0)
#define PSRAM_ID_MASK      GENMASK(23, 20)
#define FLASH_IOMAP_MASK   (GENMASK(30, 24) & (~BIT(27))) /* bit 27 is reserved */
#define PWMCS_DIS_MASK     BIT(17)
#define SPI_ENC_DIS_MASK   BIT(16)
#define CE_DIS_MASK        BIT(15)
#define CAN1_DIS_MASK      BIT(14)
#define CAN0_DIS_MASK      BIT(13)
#define EMAC_DIS_MASK      BIT(12)
#define DVP_DIS_MASK       BIT(11)
#define MIPI_DIS_MASK      BIT(10)
#define LVDS_DIS_MASK      BIT(9)
#define DE_DIS_MASK        BIT(8)
#define SRAM_DIS_MASK      GENMASK(6, 0)
#endif

#ifdef AIC_1605
#define CHIID_MAIN_BASE 0x19010210

#define CHIPID_MAIN_NVM1 (CHIID_MAIN_BASE + 0X0)
#define CHIPID_MAIN_NVM2 (CHIID_MAIN_BASE + 0X4)
#define CHIPID_MAIN_NVM3 (CHIID_MAIN_BASE + 0X8)
#define CHIPID_MAIN_NVM4 (CHIID_MAIN_BASE + 0XC)

#define PACKAGE_ID_MASK    GENMASK(11, 8)
#define MARK_ID_MASK       GENMASK(7, 0)
#define PSRAM_ID_MASK      GENMASK(7, 4)
#define FLASH_IOMAP_MASK   (GENMASK(14, 8) & (~BIT(11)))
#define MDI_DIS            BIT(2)
#define CAN1_DIS_MASK      BIT(1)
#define CAN0_DIS_MASK      BIT(0)
#endif

typedef struct _aic_1603_chip_burn_rule {
    char intel_model[64];
    unsigned char mark_id;
    unsigned char package_id;
    unsigned char psram_id;
    unsigned char flash_iomap;
    unsigned char pwmcs_dis;
    unsigned char spi_enc_dis;
    unsigned char ce_dis;
    unsigned char can1_dis;
    unsigned char can0_dis;
    unsigned char emac_dis;
    unsigned char dvp_dis;
    unsigned char mipi_dis;
    unsigned char lvds_dis;
    unsigned char de_dis;
    unsigned char sram_dis;
} aic_1603_burn_rule_t;

typedef struct _aic_1605_chip_burn_rule {
    char intel_model[64];
    unsigned char mark_id;
    unsigned char package_id;
    unsigned char psram_id;
    unsigned char flash_iomap;
    unsigned char mdi_dis;
    unsigned char can1_dis;
    unsigned char can0_dis;
} aic_1605_burn_rule_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _QC_BOARD_CFG_H_ */
