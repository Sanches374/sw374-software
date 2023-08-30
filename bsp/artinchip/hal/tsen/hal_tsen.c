/*
 * Copyright (c) 2022-2023, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors: matteo <duanmt@artinchip.com>
 */

#include "aic_core.h"
#include "hal_tsen.h"
#include "aic_hal_clk.h"

#define AIC_TSEN_TIMEOUT    3000

/* Register definition of Thermal Sensor Controller */
#define TSEN_MCR        0x000
#define TSEN_INTR       0x004
#define TSENn_CFG(n)    (0x100 + (((n) & 0x3) << 5))
#define TSENn_ITV(n)    (0x100 + (((n) & 0x3) << 5) + 0x4)
#define TSENn_FIL(n)    (0x100 + (((n) & 0x3) << 5) + 0x8)
#define TSENn_DATA(n)   (0x100 + (((n) & 0x3) << 5) + 0xC)
#define TSENn_INT(n)    (0x100 + (((n) & 0x3) << 5) + 0x10)
#define TSENn_HTAV(n)   (0x100 + (((n) & 0x3) << 5) + 0x14)
#define TSENn_LTAV(n)   (0x100 + (((n) & 0x3) << 5) + 0x18)
#define TSENn_OTPV(n)   (0x100 + (((n) & 0x3) << 5) + 0x1C)
#define TSEN_VERSION    0xFFC

#define TSEN_MCR_CTLX_EN(n)     (BIT(20) << (n))
#define TSEN_MCR_TSEN_EN(n)     (BIT(16) << n)

#define TSEN_MCR_ACQ_SHIFT      8
#define TSEN_MCR_ACQ_MASK       GENMASK(15, 8)
#define TSEN_MCR_MAX_SHIFT      4
#define TSEN_MCR_MAX_MASK       GENMASK(5, 4)
#define TSEN_MCR_EN             BIT(0)

#define TSEN_INTR_CH_INT_FLAG(n)    (BIT(16) << (n))
#define TSEN_INTR_CH_INT_EN(n)      (BIT(0) << (n))

#define TSENn_CFG_DIFF_MODE_SELECT  BIT(31)
#define TSENn_CFG_INVERTED_SELECT   BIT(27)
#define TSENn_CFG_HIGH_ADC_PRIORITY BIT(4)
#define TSENn_CFG_PERIOD_SAMPLE_EN  BIT(1)
#define TSENn_CFG_SINGLE_SAMPLE_EN  BIT(0)

#define TSENn_ITV_SHIFT         16

#define TSENn_FIL_2_POINTS      1
#define TSENn_FIL_4_POINTS      2
#define TSENn_FIL_8_POINTS      3

#define TSENn_INT_OTP_FLAG          BIT(28)
#define TSENn_INT_LTA_RM_FLAG       BIT(27)
#define TSENn_INT_LTA_VALID_FLAG    BIT(26)
#define TSENn_INT_HTA_RM_FLAG       BIT(25)
#define TSENn_INT_HTA_VALID_FLAG    BIT(24)
#define TSENn_INT_DAT_OVW_FLAG      BIT(17)
#define TSENn_INT_DAT_RDY_FLAG      BIT(16)
#define TSENn_INT_OTP_RESET         (0xA << 12)
#define TSENn_INT_OTP_IE            (0x5 << 12)
#define TSENn_INT_LTA_RM_IE         BIT(11)
#define TSENn_INT_LTA_VALID_IE      BIT(10)
#define TSENn_INT_HTA_RM_IE         BIT(9)
#define TSENn_INT_HTA_VALID_IE      BIT(8)
#define TSENn_INT_DAT_OVW_IE        BIT(1)
#define TSENn_INT_DATA_RDY_IE       BIT(0)

#define TSENn_HLTA_EN               BIT(31)
#define TSENn_HLTA_RM_THD_SHIFT     16
#define TSENn_HLTA_RM_THD_MASK      GENMASK(27, 16)
#define TSENn_HLTA_THD_MASK         GENMASK(11, 0)
#define TSENn_HTA_RM_THD(t)         ((t) - 3)
#define TSENn_LTA_RM_THD(t)         ((t) + 3)

#define TSENn_OTPV_EN               BIT(31)
#define TSENn_OTPV_VAL_MASK         GENMASK(11, 0)

#define THERMAL_CORE_TEMP_AMPN_SCALE   10 // in RT-Thread sensor framework
#define TSEN_CALIB_ACCURACY_SCALE      1000 // = 10000 / 10

static inline void tsen_writel(u32 val, int reg)
{
    writel(val, TSEN_BASE + reg);
}

static inline u32 tsen_readl(int reg)
{
    return readl(TSEN_BASE + reg);
}

/* Temperature = ADC data * slope + offset.
 * 1. Temperature accuracy adopts 1000.
 * 2. Slope and Offset accuracy adopts 10000.
 */
s32 hal_tsen_data2temp(struct aic_tsen_ch *chan)
{
    int temp = 0;

    if (chan->latest_data == 4095 || chan->latest_data == 0)
        return 0;

    if (!chan->slope)
        return chan->latest_data;

    temp = chan->slope * chan->latest_data;
    temp += chan->offset;

    if ((temp % TSEN_CALIB_ACCURACY_SCALE) < (TSEN_CALIB_ACCURACY_SCALE / 2))
        temp = temp / TSEN_CALIB_ACCURACY_SCALE;
    else
        temp = temp / TSEN_CALIB_ACCURACY_SCALE + 1;

    hal_log_debug("%s temperature: %d -> %d.%d\n", chan->name,
                  chan->latest_data,
                  temp / THERMAL_CORE_TEMP_AMPN_SCALE,
                  temp % THERMAL_CORE_TEMP_AMPN_SCALE);
    return temp;
}

/* ADC data = (Temperature - offset) / slope */
u16 hal_tsen_temp2data(struct aic_tsen_ch *chan, s32 temp)
{
    int data = temp * TSEN_CALIB_ACCURACY_SCALE - chan->offset;

    if (!chan->slope)
        return (u16)temp;

    data /= chan->slope;
    hal_log_debug("%s data: %d -> %d\n", chan->name, temp, data);
    return (u16)data;
}

static u32 tsen_sec2itv(u32 pclk, u32 sec)
{
    u32 tmp = pclk >> 16;

    tmp *= sec;
    return tmp;
}

void hal_tsen_enable(int enable)
{
    int mcr;

    mcr = tsen_readl(TSEN_MCR);
    if (enable)
        mcr |= TSEN_MCR_EN;
    else
        mcr &= ~TSEN_MCR_EN;

    tsen_writel(mcr, TSEN_MCR);
}

void hal_tsen_ch_enable(u32 ch, int enable)
{
    int mcr;

    mcr = tsen_readl(TSEN_MCR);
    if (enable) {
        mcr |= TSEN_MCR_TSEN_EN(ch);
        mcr |= TSEN_MCR_CTLX_EN(ch);
    } else {
        mcr &= ~TSEN_MCR_TSEN_EN(ch);
        mcr &= ~TSEN_MCR_CTLX_EN(ch);
    }

    tsen_writel(mcr, TSEN_MCR);
}

static void tsen_int_enable(u32 ch, u32 enable, u32 detail)
{
    u32 val = 0;

    val = tsen_readl(TSEN_INTR);
    if (enable) {
        val |= TSEN_INTR_CH_INT_EN(ch);
        tsen_writel(detail, TSENn_INT(ch));
    } else {
        val &= ~TSEN_INTR_CH_INT_EN(ch);
        tsen_writel(0, TSENn_INT(ch));
    }
    tsen_writel(val, TSEN_INTR);
}

static void tsen_single_mode(u32 ch)
{
    tsen_writel(TSENn_FIL_8_POINTS, TSENn_FIL(ch));
    tsen_writel(TSENn_CFG_SINGLE_SAMPLE_EN | tsen_readl(TSENn_CFG(ch)),
           TSENn_CFG(ch));

    tsen_int_enable(ch, 1, TSENn_INT_DATA_RDY_IE);
}

/* Only in period mode, HTA, LTA and OTP are available */
static void tsen_period_mode(struct aic_tsen_ch *chan, u32 pclk)
{
    u32 val, detail = TSENn_INT_DATA_RDY_IE;

    if (chan->hta_enable) {
        detail |= TSENn_INT_HTA_RM_IE | TSENn_INT_HTA_VALID_IE;
        val = TSENn_HLTA_EN
            | ((chan->hta_rm_thd << TSENn_HLTA_RM_THD_SHIFT)
            & TSENn_HLTA_RM_THD_MASK)
            | (chan->hta_thd & TSENn_HLTA_THD_MASK);
        tsen_writel(val, TSENn_HTAV(chan->id));
    }

    if (chan->lta_enable) {
        detail |= TSENn_INT_LTA_RM_IE | TSENn_INT_LTA_VALID_IE;
        val = TSENn_HLTA_EN
            | ((chan->lta_rm_thd << TSENn_HLTA_RM_THD_SHIFT)
            & TSENn_HLTA_RM_THD_MASK)
            | (chan->lta_thd & TSENn_HLTA_THD_MASK);
        tsen_writel(val, TSENn_LTAV(chan->id));
    }

    if (chan->otp_enable) {
        detail |= TSENn_INT_OTP_RESET;
        val = TSENn_OTPV_EN | (chan->otp_thd & TSENn_OTPV_VAL_MASK);
        tsen_writel(val, TSENn_OTPV(chan->id));
    }
    tsen_int_enable(chan->id, 1, detail);

    tsen_writel(TSENn_FIL_8_POINTS, TSENn_FIL(chan->id));

    val = tsen_sec2itv(pclk, chan->smp_period);
    tsen_writel(val << TSENn_ITV_SHIFT | 0xFFFF, TSENn_ITV(chan->id));

    tsen_writel(tsen_readl(TSENn_CFG(chan->id)) | TSENn_CFG_PERIOD_SAMPLE_EN,
           TSENn_CFG(chan->id));

    hal_tsen_ch_enable(chan->id, 1);
}

static void tsen_diff_mode(u32 ch, u32 diff, u32 inverted)
{
    u32 val = tsen_readl(TSENn_CFG(ch));

    if (diff)
        val |= TSENn_CFG_DIFF_MODE_SELECT;
    else
        val &= ~TSENn_CFG_DIFF_MODE_SELECT;

    if (inverted)
        val |= TSENn_CFG_INVERTED_SELECT;
    else
        val &= ~TSENn_CFG_INVERTED_SELECT;

    tsen_writel(val, TSENn_CFG(ch));
}

int hal_tsen_ch_init(struct aic_tsen_ch *chan, u32 pclk)
{
    if (chan->mode == AIC_TSEN_MODE_PERIOD)
        tsen_period_mode(chan, pclk);

    tsen_diff_mode(chan->id, chan->diff_mode, chan->inverted);

    /* For single mode, should init the channel in .get_temp() */
    return 0;
}

int hal_tsen_get_temp(struct aic_tsen_ch *chan, s32 *val)
{
    int ret = 0;

    if (!chan->available) {
        hal_log_err("%s is unavailable!\n", chan->name);
        return -ENODATA;
    }

#ifndef CONFIG_ARTINCHIP_ADCIM_DM
    if (chan->mode == AIC_TSEN_MODE_PERIOD)
        return hal_tsen_data2temp(chan);
#endif

    tsen_single_mode(chan->id);
    hal_tsen_ch_enable(chan->id, 1);

    ret = aicos_sem_take(chan->complete, AIC_TSEN_TIMEOUT);
    if (ret < 0) {
        hal_log_err("%s read timeout!\n", chan->name);
        hal_tsen_ch_enable(chan->id, 0);
        return -ETIMEDOUT;
    }
    hal_tsen_ch_enable(chan->id, 0);

    if (val)
        *val = hal_tsen_data2temp(chan);

    return 0;
}

void hal_tsen_status_show(struct aic_tsen_ch *chan)
{
    int mcr = tsen_readl(TSEN_MCR);
    int version = tsen_readl(TSEN_VERSION);

    printf("In Thermal Sensor V%d.%02d:\n"
               "Ch Name          Mode Diff Inv Enable Value  LTA  HTA  OTP\n"
               "%d  %-13s %4s %4d %3d %6d %5d %4d %4d %4d\n",
               version >> 8, version & 0xff,
               chan->id, chan->name, chan->mode ? "P" : "S",
               chan->diff_mode, chan->inverted,
               mcr & TSEN_MCR_TSEN_EN(chan->id) ? 1 : 0,
               chan->latest_data, chan->lta_thd, chan->hta_thd, chan->otp_thd);
}

// TODO: irq_handle() should get 'struct aic_tsen_ch *' from 'void *arg'
extern struct aic_tsen_ch aic_tsen_chs[AIC_TSEN_CH_NUM];

irqreturn_t hal_tsen_irq_handle(int irq, void *arg)
{
    int i, status, detail = 0;
    struct aic_tsen_ch *chan = NULL;

    aicos_irq_disable(TSEN_IRQn);
    status = tsen_readl(TSEN_INTR);
    hal_log_debug("Module IRQ status: %#x\n", status);
    for (i = 0; i < AIC_TSEN_CH_NUM; i++) {
        if (!(status & TSEN_INTR_CH_INT_FLAG(i)))
            continue;

        chan = &aic_tsen_chs[i];
        detail = tsen_readl(TSENn_INT(i));
        tsen_writel(detail, TSENn_INT(i));
        hal_log_debug("ch%d IRQ status: %#x\n", i, detail);
        if (detail & TSENn_INT_DAT_RDY_FLAG) {
            chan->latest_data = tsen_readl(TSENn_DATA(i));
            hal_log_debug("ch%d data %d\n", i, chan->latest_data);

            if (chan->mode == AIC_TSEN_MODE_SINGLE)
                aicos_sem_give(chan->complete);
        }

        if (detail & TSENn_INT_LTA_VALID_FLAG)
            hal_log_warn("LTA: ch%d %d(%d)!\n", i,
                 hal_tsen_data2temp(chan), chan->latest_data);
        if (detail & TSENn_INT_LTA_RM_FLAG)
            hal_log_warn("LTA removed: ch%d %d(%d)\n", i,
                 hal_tsen_data2temp(chan), chan->latest_data);
        if (detail & TSENn_INT_HTA_VALID_FLAG)
            hal_log_warn("HTA: ch%d %d(%d)!\n", i,
                 hal_tsen_data2temp(chan), chan->latest_data);
        if (detail & TSENn_INT_HTA_RM_FLAG)
            hal_log_warn("HTA removed: ch%d %d(%d)\n", i,
                 hal_tsen_data2temp(chan), chan->latest_data);
        if (detail & TSENn_INT_OTP_FLAG)
            hal_log_warn("OTP: ch%d %d(%d)!\n", i,
                 hal_tsen_data2temp(chan), chan->latest_data);
        if (detail & TSENn_INT_DAT_OVW_FLAG)
            hal_log_warn("Data Over Write: ch%d %d(%d)!\n", i,
                 hal_tsen_data2temp(chan), chan->latest_data);
    }

    aicos_irq_enable(TSEN_IRQn);
    hal_log_debug("IRQ status %#x, detail %#x\n", status, detail);

    return IRQ_HANDLED;
}

s32 hal_tsen_clk_init(void)
{
    s32 ret = 0;

    ret = hal_clk_enable(CLK_TSEN);
    if (ret < 0) {
        pr_err("TSensor clk enable failed!\n");
        return -1;
    }

    ret = hal_clk_enable_deassertrst(CLK_TSEN);
    if (ret < 0) {
        pr_err("TSensor reset deassert failed!\n");
        return -1;
    }

    return ret;
}

void hal_tsen_pclk_get(struct aic_tsen_ch *chan)
{
    chan->pclk_rate = hal_clk_get_freq(hal_clk_get_parent(CLK_TSEN));
}
