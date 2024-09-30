/*
 * Copyright (c) 2022-2023, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors:  Zequan Liang <zequan.liang@artinchip.com>
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>

#include "qc_chip_info_rtos.h"
#if QC_OS_RT_THREAD

#define DEBUG_CHIP_INFO 0

typedef unsigned long u32;

static void bin_to_hex_str(unsigned char byte, char hex_str[2]);
static u32 get_efuse_sid_offset(u32 addr);
static u32 efuse_read_sid(u32 addr);

#ifdef AIC_1603
static void get_aic_1603_burn_info(aic_1603_burn_rule_t *rule);
static qc_res_t compare_aic_1603_burn_info(aic_1603_burn_rule_t *rule, int *rule_table_index);

static aic_1603_burn_rule_t aic_1603_rule_table[] = {
    {"D133BAS",    0x01, 0xB, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x78},
    {"D133BBS",    0x02, 0xB, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x78},
    {"D133CBS",    0x03, 0xC, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
    {"D133CCS1",   0x06, 0xC, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
    {"D133CCS2",   0x06, 0xC, 0x2, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
    {"D133EBS",    0x04, 0xE, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
    {"D133ECS1",   0x07, 0xE, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
    {"D133ECS2",   0x07, 0xE, 0x2, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
    {"D132ENS",    0x05, 0xE, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
    {"G730CES",    0x21, 0xC, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x1, 0x0, 0x0},
    {"G730EES",    0x22, 0xE, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x1, 0x0, 0x0},
    {"G730BDU",    0x23, 0xB, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x1, 0x0, 0x0},
    {"M6801SPCS",  0x31, 0x1, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x1, 0x1, 0x1, 0x78},
    {"M6806SPES",  0x32, 0x3, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x1, 0x1, 0x1, 0x0},
    {"DR128",      0XA1, 0xB, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
    {"JYX68",      0XA2, 0xB, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}
};
#endif

#ifdef AIC_1605
static void get_aic_1605_burn_info(aic_1605_burn_rule_t *rule);
static qc_res_t compare_aic_1605_burn_info(aic_1605_burn_rule_t *rule, int *rule_table_index);

static aic_1605_burn_rule_t aic_1605_rule_table[] = {
    {"D121BAV",  0x03, 0xB, 0x0, 0x00, 0x1, 0x1, 0x1},
    {"D121BBV",  0x04, 0xB, 0x0, 0x00, 0x1, 0x1, 0x1},
    {"D122BAV",  0x01, 0xB, 0x0, 0x00, 0x1, 0x0, 0x0},
    {"D122BBV",  0x02, 0xB, 0x0, 0x00, 0x1, 0x0, 0x0},
    {"D122BCV1", 0x05, 0xB, 0x1, 0x00, 0x1, 0x0, 0x0},
    {"D122BCV2", 0x05, 0xB, 0x2, 0x00, 0x1, 0x0, 0x0},
    {"TR230",    0xA1, 0xB, 0x0, 0x00, 0x0, 0x0, 0x0},
    {"JYX58",    0xB1, 0xB, 0x0, 0x00, 0x1, 0x0, 0x0}
};

#endif

qc_res_t qc_read_chip_intel_model_rtos(char *chip_model, int len)
{
#ifdef AIC_1603
    int rule_table_index = -1;
    aic_1603_burn_rule_t burn_rule = {0};

    get_aic_1603_burn_info(&burn_rule);
    if (compare_aic_1603_burn_info(&burn_rule, &rule_table_index) == QC_RES_INV) {
        strncpy(chip_model, "Unkown chip model", len);
        return QC_RES_INV;
    }
    strncpy(chip_model, aic_1603_rule_table[rule_table_index].intel_model, len);
    return QC_RES_OK;
#endif

#ifdef AIC_1605
    int rule_table_index = -1;
    aic_1605_burn_rule_t burn_rule = {0};

    get_aic_1605_burn_info(&burn_rule);
    if (compare_aic_1605_burn_info(&burn_rule, &rule_table_index) == QC_RES_INV) {
        strncpy(chip_model, "Unkown chip model", len);
        return QC_RES_INV;
    }

    strncpy(chip_model, aic_1605_rule_table[rule_table_index].intel_model, len);
    return QC_RES_OK;
#endif

    return QC_RES_INV;
}

qc_res_t qc_read_chip_intel_model_file_rtos(char *chip_model, int len, const char *file)
{
    return QC_RES_INV;
}

qc_res_t qc_write_chip_intel_module_file_rtos(char *chip_model, const char *file)
{
    return QC_RES_INV;
}

qc_res_t qc_read_chip_id_rtos(char *chip_id, int len)
{
    static unsigned char chip_id_context[128 / 8] = {0};
    char cmd[128] = {0};
    int i = 0, pos = 0, chip_id_len = 128 / 8;

    if (len < chip_id_len) {
        return QC_RES_INV;
    }

    snprintf(cmd, sizeof(cmd), "efuse read %p 10 0x20", (void *)chip_id_context);
    qc_cmd_exec(cmd, NULL, 0, 0);

    /* the chip_id buf context is
     * 44-80-ce-b8-52-2f-c0-0c-06-0c-11-18-09-00-30-04
    */
    for (i = 0; i < chip_id_len; i++) {
        bin_to_hex_str(chip_id_context[i], (chip_id + pos));
        pos = ((i + 1) * 2) + i;
        chip_id[pos] = '-';
        pos++;
    }
    chip_id[pos - 1] = '\0';

    printf("chip_id = %s\n", chip_id);
    return QC_RES_OK;
}

static void bin_to_hex_str(unsigned char byte, char hex_str[2])
{
    static const char *hex_digits = "0123456789abcdef";

    hex_str[0] = hex_digits[(byte >> 4) & 0xF];
    hex_str[1] = hex_digits[byte & 0xF];
}

static u32 get_efuse_sid_offset(u32 addr)
{
    u32 offset = 0;

#ifdef AIC_1603
    switch (addr) {
    case 0x19010210: /* CHIPID_MAIN_NVM1 or CHIID_MAIN_BASE */
        offset = 0x10;
        break;
    case 0x19010214: /* CHIPID_MAIN_NVM2 */
        offset = 0x14;
        break;
    case 0x19010218: /* CHIPID_MAIN_NVM3 */
        offset = 0x18;
        break;
    case 0x1901021C: /* CHIPID_MAIN_NVM4 */
        offset = 0x1C;
        break;
    case 0x19010220: /* CHIP_LOCK_NVM1 or CHIP_LOCK_BASE */
        offset = 0x20;
        break;
    case 0x19010224: /* CHIP_LOCK_NVM2 */
        offset = 0x24;
        break;
    default:
        break;
    }
#endif

#ifdef AIC_1605
    switch (addr) {
    case 0x19010210: /* CHIPID_MAIN_NVM1 or CHIID_MAIN_BASE */
        offset = 0x10;
        break;
    case 0x19010214: /* CHIPID_MAIN_NVM2 */
        offset = 0x14;
        break;
    case 0x19010218: /* CHIPID_MAIN_NVM3 */
        offset = 0x18;
        break;
    case 0x1901021C: /* CHIPID_MAIN_NVM4 */
        offset = 0x1C;
        break;
    default:
        break;
    }
#endif

    return offset;
}

static u32 efuse_read_sid(u32 addr)
{
    static unsigned char efuse_context[32 / 8] = {0};
    char cmd[128] = {0};

    u32 efuse_offset = get_efuse_sid_offset(addr);
    if (efuse_offset == 0)
        return 0;

    snprintf(cmd, sizeof(cmd), "efuse read %p %lx 0x4", (void *)efuse_context, efuse_offset);
    qc_cmd_exec(cmd, NULL, 0, 0);
    u32 result = efuse_context[3] << 24 |
                 efuse_context[2] << 16 |
                 efuse_context[1] << 8 |
                 efuse_context[0];
#if DEBUG_CHIP_INFO
    printf("cmd = %s, result = 0x%x 0x%x 0x%x 0x%x\n", cmd, efuse_context[3], efuse_context[2], efuse_context[1], efuse_context[0]);
#endif
    return result;
}

#ifdef AIC_1603
static void get_aic_1603_burn_info(aic_1603_burn_rule_t *rule)
{
    u32 chipid_main_nvm3 = 0;
    u32 chip_lock_nvm1 = 0;
    u32 chip_lock_nvm2 = 0;

    chipid_main_nvm3 = efuse_read_sid(CHIPID_MAIN_NVM3);
    chip_lock_nvm1 = efuse_read_sid(CHIP_LOCK_NVM1);
    chip_lock_nvm2 = efuse_read_sid(CHIP_LOCK_NVM2);

    rule->mark_id = (((chipid_main_nvm3 & MARK_ID_MASK) >> 0) & 0xff);
    rule->package_id = (((chipid_main_nvm3 & PACKAGE_ID_MASK) >> 8) & 0xff);
    rule->psram_id = (((chip_lock_nvm2 & PSRAM_ID_MASK) >> 20) & 0xff);
    rule->flash_iomap = (((chip_lock_nvm2 & FLASH_IOMAP_MASK) >> 24) & 0xff);
    rule->pwmcs_dis = (((chip_lock_nvm1 & PWMCS_DIS_MASK) >> 17) & 0xff);
    rule->spi_enc_dis = (((chip_lock_nvm1 & SPI_ENC_DIS_MASK) >> 16) & 0xff);
    rule->ce_dis = (((chip_lock_nvm1 & CE_DIS_MASK) >> 15) & 0xff);
    rule->can1_dis = (((chip_lock_nvm1 & CAN1_DIS_MASK) >> 14) & 0xff);
    rule->can0_dis = (((chip_lock_nvm1 & CAN0_DIS_MASK) >> 13) & 0xff);
    rule->emac_dis = (((chip_lock_nvm1 & EMAC_DIS_MASK) >> 12) & 0xff);
    rule->dvp_dis = (((chip_lock_nvm1 & DVP_DIS_MASK) >> 11) & 0xff);
    rule->mipi_dis = (((chip_lock_nvm1 & MIPI_DIS_MASK) >> 10) & 0xff);
    rule->de_dis = (((chip_lock_nvm1 & DE_DIS_MASK) >> 8) & 0xff);
    rule->sram_dis = (((chip_lock_nvm1 & SRAM_DIS_MASK) >> 0) & 0xff);

#if DEBUG_CHIP_INFO
    printf("mark_id = 0x%x\n", rule->mark_id);
    printf("package_id = 0x%x\n", rule->package_id);
    printf("psram_id = 0x%x\n", rule->psram_id);
    printf("flash_iomap = 0x%x\n", rule->flash_iomap);
    printf("pwmcs_dis = 0x%x\n", rule->pwmcs_dis);
    printf("ce_dis = 0x%x\n", rule->ce_dis);
    printf("can1_dis = 0x%x\n", rule->can1_dis);
    printf("can0_dis = 0x%x\n", rule->can0_dis);
    printf("emac_dis = 0x%x\n", rule->emac_dis);
    printf("dvp_dis = 0x%x\n", rule->dvp_dis);
    printf("mipi_dis = 0x%x\n", rule->mipi_dis);
    printf("de_dis = 0x%x\n", rule->de_dis);
    printf("sram_dis = 0x%x\n", rule->sram_dis);
#endif
}

static int compare_aic_1603_burn_info(aic_1603_burn_rule_t *rule, int *rule_table_index)
{
    int rule_table_len = sizeof(aic_1603_rule_table) / sizeof(aic_1603_rule_table[0]);
    int i = -1;

    for (i = 0; i < rule_table_len; i++) {
        if (rule->mark_id != aic_1603_rule_table[i].mark_id ||
            rule->package_id != aic_1603_rule_table[i].package_id ||
            rule->psram_id != aic_1603_rule_table[i].psram_id ||
            rule->flash_iomap != aic_1603_rule_table[i].flash_iomap ||
            rule->pwmcs_dis != aic_1603_rule_table[i].pwmcs_dis ||
            rule->spi_enc_dis != aic_1603_rule_table[i].spi_enc_dis ||
            rule->ce_dis != aic_1603_rule_table[i].ce_dis ||
            rule->can1_dis != aic_1603_rule_table[i].can1_dis ||
            rule->can0_dis != aic_1603_rule_table[i].can0_dis ||
            rule->emac_dis != aic_1603_rule_table[i].emac_dis ||
            rule->dvp_dis != aic_1603_rule_table[i].dvp_dis ||
            rule->mipi_dis != aic_1603_rule_table[i].mipi_dis ||
            rule->lvds_dis != aic_1603_rule_table[i].lvds_dis ||
            rule->de_dis != aic_1603_rule_table[i].de_dis ||
            rule->sram_dis != aic_1603_rule_table[i].sram_dis) {
            continue;
        }
        break;
    }

    if (i == rule_table_len)
        return QC_RES_INV;

    *rule_table_index = i;
    return QC_RES_OK;
}

#endif

#ifdef AIC_1605
static void get_aic_1605_burn_info(aic_1605_burn_rule_t *rule)
{
    u32 chipid_main_nvm3 = 0;
    u32 chipid_main_nvm4 = 0;

    chipid_main_nvm3 = efuse_read_sid(CHIPID_MAIN_NVM3);
    chipid_main_nvm4 = efuse_read_sid(CHIPID_MAIN_NVM4);

    rule->mark_id = (((chipid_main_nvm3 & MARK_ID_MASK) >> 0) & 0xff);
    rule->package_id = (((chipid_main_nvm3 & PACKAGE_ID_MASK) >> 8) & 0xff);
    rule->psram_id = (((chipid_main_nvm4 & PSRAM_ID_MASK) >> 4) & 0xff);
    rule->flash_iomap = (((chipid_main_nvm4 & FLASH_IOMAP_MASK) >> 8) & 0xff);
    rule->mdi_dis = (((chipid_main_nvm4 & MDI_DIS) >> 2) & 0xff);
    rule->can1_dis = (((chipid_main_nvm4 & CAN1_DIS_MASK) >> 1) & 0xff);
    rule->can0_dis = (((chipid_main_nvm4 & CAN0_DIS_MASK) >> 0) & 0xff);

#if DEBUG_CHIP_INFO
    printf("mark_id = 0x%x\n", rule->mark_id);
    printf("package_id = 0x%x\n", rule->package_id);
    printf("psram_id = 0x%x\n", rule->psram_id);
    printf("flash_iomap = 0x%x\n", rule->flash_iomap);
    printf("mdi_dis = 0x%x\n", rule->mdi_dis);
    printf("can1_dis = 0x%x\n", rule->can1_dis);
    printf("can0_dis = 0x%x\n", rule->can0_dis);
#endif
}

static int compare_aic_1605_burn_info(aic_1605_burn_rule_t *rule, int *rule_table_index)
{
    int rule_table_len = sizeof(aic_1605_rule_table) / sizeof(aic_1605_rule_table[0]);
    int i = -1;

    for (i = 0; i < rule_table_len; i++) {
        if (rule->mark_id != aic_1605_rule_table[i].mark_id ||
            rule->package_id != aic_1605_rule_table[i].package_id ||
            rule->psram_id != aic_1605_rule_table[i].psram_id ||
            rule->flash_iomap != aic_1605_rule_table[i].flash_iomap ||
            rule->mdi_dis != aic_1605_rule_table[i].mdi_dis ||
            rule->can1_dis != aic_1605_rule_table[i].can1_dis ||
            rule->can0_dis != aic_1605_rule_table[i].can0_dis) {
            continue;
        }
        break;
    }

    if (i == rule_table_len)
        return QC_RES_INV;

    *rule_table_index = i;
    return QC_RES_OK;
}
#endif
#endif
