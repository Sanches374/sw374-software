/*
 * Copyright (c) 2022-2023, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>

#include "aic_core.h"
#include "aic_dma_id.h"

#include "hal_dma_reg.h"
#include "hal_dma.h"

struct aic_dma_dev {
    struct aic_dma_task task[TASK_MAX_NUM];
    s32 inited;
    unsigned long base;
    u32 burst_length; /* burst length capacity */
    u32 addr_widths; /* address width support capacity */
    struct aic_dma_chan dma_chan[AIC_DMA_CH_NUM];
    struct aic_dma_task *freetask;
};

static struct aic_dma_dev aich_dma __ALIGNED(CACHE_LINE_SIZE) = {
    .base = DMA_BASE,
    .burst_length = BIT(1) | BIT(4) | BIT(8) | BIT(16),
    .addr_widths = AIC_DMA_BUS_WIDTH,
};

static inline s8 convert_burst(u32 maxburst)
{
    switch (maxburst) {
    case 1:
        return 0;
    case 4:
        return 1;
    case 8:
        return 2;
    case 16:
        return 3;
    default:
        return -EINVAL;
    }
}

static inline s8 convert_buswidth(enum dma_slave_buswidth addr_width)
{
    switch (addr_width) {
    case DMA_SLAVE_BUSWIDTH_2_BYTES:
        return 1;
    case DMA_SLAVE_BUSWIDTH_4_BYTES:
        return 2;
    case DMA_SLAVE_BUSWIDTH_8_BYTES:
        return 3;
    default:
        /* For 1 byte width or fallback */
        return 0;
    }
}

static int aic_set_burst(struct dma_slave_config *sconfig,
                             enum dma_transfer_direction direction,
                             u32 *p_cfg)
{
    enum dma_slave_buswidth src_addr_width, dst_addr_width;
    u32 src_maxburst, dst_maxburst;
    s8 src_width, dst_width, src_burst, dst_burst;

    src_addr_width = sconfig->src_addr_width;
    dst_addr_width = sconfig->dst_addr_width;
    src_maxburst = sconfig->src_maxburst;
    dst_maxburst = sconfig->dst_maxburst;

    switch (direction) {
    case DMA_MEM_TO_DEV:
        if (src_addr_width == DMA_SLAVE_BUSWIDTH_UNDEFINED)
            src_addr_width = DMA_SLAVE_BUSWIDTH_4_BYTES;
        src_maxburst = src_maxburst ? src_maxburst : 8;
        break;
    case DMA_DEV_TO_MEM:
        if (dst_addr_width == DMA_SLAVE_BUSWIDTH_UNDEFINED)
            dst_addr_width = DMA_SLAVE_BUSWIDTH_4_BYTES;
        dst_maxburst = dst_maxburst ? dst_maxburst : 8;
        break;
    default:
        return -EINVAL;
    }

    if (!(BIT(src_addr_width) & aich_dma.addr_widths))
        return -EINVAL;
    if (!(BIT(dst_addr_width) & aich_dma.addr_widths))
        return -EINVAL;
    if (!(BIT(src_maxburst) & aich_dma.burst_length))
        return -EINVAL;
    if (!(BIT(dst_maxburst) & aich_dma.burst_length))
        return -EINVAL;

    src_width = convert_buswidth(src_addr_width);
    dst_width = convert_buswidth(dst_addr_width);
    dst_burst = convert_burst(dst_maxburst);
    src_burst = convert_burst(src_maxburst);

    *p_cfg = (src_width << SRC_WIDTH_BITSHIFT) |
         (dst_width << DST_WIDTH_BITSHIFT) |
         (src_burst << SRC_BURST_BITSHIFT) |
         (dst_burst << DST_BURST_BITSHIFT);

    return 0;
}

static void hal_dma_reg_dump(struct aic_dma_chan *chan)
{
    printf("Common register: \n"
            "    IRQ_EN 0x%x, \tIRQ_STA 0x%x, \tCH_STA 0x%x, GATE 0x%x\n",
            readl(aich_dma.base + DMA_IRQ_EN_REG),
            readl(aich_dma.base + DMA_IRQ_STA_REG),
            readl(aich_dma.base + DMA_CH_STA_REG),
            readl(aich_dma.base + DMA_GATE_REG));

    printf("Ch%d register: \n"
            "    Enable 0x%x, \tMode 0x%x, \tPause 0x%x\n"
            "    Task 0x%x, \tConfig 0x%x, \tSrc 0x%x, \tSink 0x%x\n"
            "    Left 0x%x, \tPackage_cnt %d\n",
            chan->ch_nr,
            readl(chan->base + DMA_CH_EN_REG),
            readl(chan->base + DMA_CH_MODE_REG),
            readl(chan->base + DMA_CH_PAUSE_REG),
            readl(chan->base + DMA_CH_TASK_REG),
            readl(chan->base + DMA_CH_CFG_REG),
            readl(chan->base + DMA_CH_SRC_REG),
            readl(chan->base + DMA_CH_SINK_REG),
            readl(chan->base + DMA_CH_LEFT_REG),
            readl(chan->base + DMA_CH_PKG_NUM_REG));
}

static void hal_dma_task_dump(struct aic_dma_chan *chan)
{
    struct aic_dma_task *task;

    printf("DMA Ch%d: desc = 0x%lx\n", chan->ch_nr, (unsigned long)chan->desc);

    for (task = chan->desc; task != NULL; task = task->v_next)
    {
        printf(" task (0x%lx):\n"
                "\tcfg - 0x%x, src - 0x%x, dst - 0x%x, len - 0x%x\n"
                "\tdelay - 0x%x, p_next - 0x%x, mode - 0x%x, v_next - 0x%lx\n",
                (unsigned long)task,
                task->cfg, task->src, task->dst, task->len,
                task->delay, task->p_next, task->mode,
                (unsigned long)task->v_next);
    }
}

static struct aic_dma_task *aic_dma_task_alloc(void)
{
    struct aic_dma_task *task;

    /* Remove the QH structure from the freelist */

    task = aich_dma.freetask;
    if (task) {
        aich_dma.freetask = task->v_next;
        memset(task, 0, sizeof(struct aic_dma_task));
    }

    return task;
}

static void aic_dma_task_free(struct aic_dma_task *task)
{
    CHECK_PARAM_RET(task != NULL);

    task->v_next = aich_dma.freetask;
    aich_dma.freetask = task;
}

static void *aic_dma_task_add(struct aic_dma_task *prev,
                                 struct aic_dma_task *next,
                                 struct aic_dma_chan *chan)
{
    CHECK_PARAM((chan != NULL || prev != NULL) && next != NULL, NULL);

    if (!prev)
    {
        chan->desc = next;
    }
    else
    {
        prev->p_next = __pa((unsigned long)next);
        prev->v_next = next;
    }

    next->p_next = DMA_LINK_END_FLAG;
    next->v_next = NULL;

    return next;
}

static void aic_dma_free_desc(struct aic_dma_chan *chan)
{
    struct aic_dma_task *task;
    struct aic_dma_task *next;

    CHECK_PARAM_RET(chan != NULL);

    task = chan->desc;
    chan->desc = NULL;

    while (task)
    {
        next = task->v_next;
        aic_dma_task_free(task);
        task = next;
    }

    chan->callback = NULL;
    chan->callback_param = NULL;
}

irqreturn_t hal_dma_irq(int irq, void *arg)
{
    int i;
    u32 status;
    struct aic_dma_chan *chan;
    dma_async_callback cb = NULL;
    void *cb_data = NULL;

    /* get dma irq pending */
    status = readl(aich_dma.base + DMA_IRQ_STA_REG);
    if (!status) {
        /* none irq trigger */
        return IRQ_NONE;
    }
    pr_debug("IRQ status: 0x%x\n", status);

    /* clear irq pending */
    writel(status, aich_dma.base + DMA_IRQ_STA_REG);

    /* process irq for every dma channel */
    for (i = 0; (i < AIC_DMA_CH_NUM) && status; i++, status >>= DMA_IRQ_CH_WIDTH) {
        chan = &aich_dma.dma_chan[i];

        if ((!chan->used) || !(status & chan->irq_type))
            continue;

        cb = chan->callback;
        cb_data = chan->callback_param;
        if (cb)
        {
            cb(cb_data);
        }
    }

    return IRQ_HANDLED;
}

int hal_dma_chan_dump(int ch_nr)
{
    struct aic_dma_chan *chan;

    CHECK_PARAM(ch_nr < AIC_DMA_CH_NUM, -EINVAL);

    chan = &aich_dma.dma_chan[ch_nr];

    hal_dma_task_dump(chan);
    hal_dma_reg_dump(chan);

    return 0;
}

#ifndef AIC_DMA_DRV_V10

int hal_dma_chan_prep_memset(struct aic_dma_chan *chan,
                             u32 p_dest, u32 value, u32 len)
{
    struct aic_dma_task *task;

    CHECK_PARAM(chan != NULL && len != 0, -EINVAL);

    CHECK_PARAM((p_dest%AIC_DMA_ALIGN_SIZE) == 0, -EINVAL);

    task = aic_dma_task_alloc();
    CHECK_PARAM(task != NULL, -ENOMEM);

    task->src = p_dest;
    task->dst = p_dest;
    task->len = len;
    task->cfg = (ADDR_LINEAR_MODE << DST_ADDR_BITSHIFT) |
             (ADDR_LINEAR_MODE << SRC_ADDR_BITSHIFT) |
             (3 << DST_BURST_BITSHIFT) | (3 << SRC_BURST_BITSHIFT) |
             (2 << DST_WIDTH_BITSHIFT) | (2 << SRC_WIDTH_BITSHIFT) |
             (DMA_ID_DRAM << DST_PORT_BITSHIFT) |
             (DMA_ID_DRAM << SRC_PORT_BITSHIFT);
    task->delay = DELAY_DEF_VAL;
    task->mode = DMA_S_WAIT_D_WAIT;

    aicos_dcache_clean_invalid_range((void *)(unsigned long)task->dst, task->len);

    aic_dma_task_add(NULL, task, chan);

    writel(value, chan->base + DMA_CH_MEMSET_VAL_REG);
    chan->memset = true;

#ifdef AIC_DMA_DRV_DEBUG
    hal_dma_task_dump(chan);
#endif
    return 0;
}

#endif

int hal_dma_chan_prep_memcpy(struct aic_dma_chan *chan,
                             u32 p_dest, u32 p_src, u32 len)
{
    struct aic_dma_task *task;

    CHECK_PARAM(chan != NULL && len != 0, -EINVAL);

    CHECK_PARAM((p_dest%AIC_DMA_ALIGN_SIZE) == 0 && (p_src%AIC_DMA_ALIGN_SIZE) == 0, -EINVAL);

    task = aic_dma_task_alloc();
    CHECK_PARAM(task != NULL, -ENOMEM);

    task->src = p_src;
    task->dst = p_dest;
    task->len = len;
    task->cfg = (ADDR_LINEAR_MODE << DST_ADDR_BITSHIFT) |
             (ADDR_LINEAR_MODE << SRC_ADDR_BITSHIFT) |
             (3 << DST_BURST_BITSHIFT) | (3 << SRC_BURST_BITSHIFT) |
             (2 << DST_WIDTH_BITSHIFT) | (2 << SRC_WIDTH_BITSHIFT) |
             (DMA_ID_DRAM << DST_PORT_BITSHIFT) |
             (DMA_ID_DRAM << SRC_PORT_BITSHIFT);
    task->delay = DELAY_DEF_VAL;
    task->mode = DMA_S_WAIT_D_WAIT;

    aicos_dcache_clean_range((void *)(unsigned long)task->src, task->len);
    aicos_dcache_clean_invalid_range((void *)(unsigned long)task->dst, task->len);

    aic_dma_task_add(NULL, task, chan);

#ifdef AIC_DMA_DRV_DEBUG
    hal_dma_task_dump(chan);
#endif
    return 0;
}

int hal_dma_chan_prep_device(struct aic_dma_chan *chan,
                             u32 p_dest, u32 p_src, u32 len,
                             enum dma_transfer_direction dir)
{
    struct aic_dma_task *task;
    u32 task_cfg;
    int ret;

    CHECK_PARAM(chan != NULL && len != 0, -EINVAL);

    CHECK_PARAM((p_dest%AIC_DMA_ALIGN_SIZE) == 0 && (p_src%AIC_DMA_ALIGN_SIZE) == 0, -EINVAL);

    ret = aic_set_burst(&chan->cfg, dir, &task_cfg);
    if (ret) {
        hal_log_err("Invalid DMA configuration\n");
        return -EINVAL;
    }

    task = aic_dma_task_alloc();
    CHECK_PARAM(task != NULL, -ENOMEM);

    task->delay = DELAY_DEF_VAL;
    task->len = len;
    task->src = p_src;
    task->dst = p_dest;
    task->cfg = task_cfg;
    if (dir == DMA_MEM_TO_DEV) {
        task->cfg |= (DMA_ID_DRAM << SRC_PORT_BITSHIFT) |
                  ((chan->cfg.slave_id & DMA_DRQ_PORT_MASK) << DST_PORT_BITSHIFT) |
                  (ADDR_LINEAR_MODE << SRC_ADDR_BITSHIFT) |
                  (ADDR_FIXED_MODE << DST_ADDR_BITSHIFT);
        task->mode = DMA_S_WAIT_D_HANDSHAKE;
        aicos_dcache_clean_range((void *)(unsigned long)task->src, task->len);
    } else {
        task->cfg |= (DMA_ID_DRAM << DST_PORT_BITSHIFT) |
                  ((chan->cfg.slave_id & DMA_DRQ_PORT_MASK) << SRC_PORT_BITSHIFT) |
                  (ADDR_LINEAR_MODE << DST_ADDR_BITSHIFT) |
                  (ADDR_FIXED_MODE << SRC_ADDR_BITSHIFT);
        task->mode = DMA_S_HANDSHAKE_D_WAIT;
        aicos_dcache_clean_invalid_range((void *)(unsigned long)task->dst, task->len);
    }

    aic_dma_task_add(NULL, task, chan);

#ifdef AIC_DMA_DRV_DEBUG
    hal_dma_task_dump(chan);
#endif
    return 0;
}

int hal_dma_chan_prep_cyclic(struct aic_dma_chan *chan,
                             u32 p_buf_addr, u32 buf_len, u32 period_len,
                             enum dma_transfer_direction dir)
{
    struct aic_dma_task *task = NULL;
    struct aic_dma_task *prev = NULL;
    u32 task_cfg;
    u32 periods;
    u32 i;
    int ret;

    CHECK_PARAM(chan != NULL && buf_len != 0 && period_len != 0, -EINVAL);

    CHECK_PARAM((p_buf_addr%AIC_DMA_ALIGN_SIZE) == 0 && (buf_len%AIC_DMA_ALIGN_SIZE) == 0, -EINVAL);

    ret = aic_set_burst(&chan->cfg, dir, &task_cfg);
    if (ret) {
        hal_log_err("Invalid DMA configuration\n");
        return -EINVAL;
    }

    periods = buf_len / period_len;

    for (i = 0; i < periods; i++) {
        task = aic_dma_task_alloc();
        if (task == NULL){
            aic_dma_free_desc(chan);
            return -ENOMEM;
        }

        task->len = period_len;
        if (dir == DMA_MEM_TO_DEV) {
            task->src = p_buf_addr + period_len * i;
            task->dst = chan->cfg.dst_addr;
            task->cfg = task_cfg;
            task->cfg |= (DMA_ID_DRAM << SRC_PORT_BITSHIFT) |
                      ((chan->cfg.slave_id & DMA_DRQ_PORT_MASK) << DST_PORT_BITSHIFT) |
                      (ADDR_LINEAR_MODE << SRC_ADDR_BITSHIFT) |
                      (ADDR_FIXED_MODE << DST_ADDR_BITSHIFT);
            task->delay = DELAY_DEF_VAL;
            task->mode = DMA_S_WAIT_D_HANDSHAKE;
            aicos_dcache_clean_range((void *)(unsigned long)task->src, task->len);
        } else {
            task->src = chan->cfg.src_addr;
            task->dst = p_buf_addr + period_len * i;
            task->cfg = task_cfg;
            task->cfg |= (DMA_ID_DRAM << DST_PORT_BITSHIFT) |
                      ((chan->cfg.slave_id & DMA_DRQ_PORT_MASK) << SRC_PORT_BITSHIFT) |
                      (ADDR_LINEAR_MODE << DST_ADDR_BITSHIFT) |
                      (ADDR_FIXED_MODE << SRC_ADDR_BITSHIFT);
            task->delay = DELAY_DEF_VAL;
            task->mode = DMA_S_HANDSHAKE_D_WAIT;
            aicos_dcache_clean_invalid_range((void *)(unsigned long)task->dst, task->len);
        }

        prev = aic_dma_task_add(prev, task, chan);
    }

    prev->p_next = __pa((unsigned long)chan->desc);

    chan->cyclic = true;

#ifdef AIC_DMA_DRV_DEBUG
    hal_dma_task_dump(chan);
#endif
    return 0;
}

enum dma_status hal_dma_chan_tx_status(struct aic_dma_chan *chan,
                                       u32 *left_size)
{
    CHECK_PARAM(chan != NULL && left_size != NULL, -EINVAL);

    if (!(readl(aich_dma.base + DMA_CH_STA_REG) & BIT(chan->ch_nr)))
        return DMA_COMPLETE;

    *left_size = readl(chan->base + DMA_CH_LEFT_REG);
        return DMA_IN_PROGRESS;
}

int hal_dma_chan_start(struct aic_dma_chan *chan)
{
    u32 value;
    struct aic_dma_task *task;

    CHECK_PARAM(chan != NULL && chan->desc != NULL, -EINVAL);

    for (task = chan->desc; task != NULL; task = task->v_next)
        aicos_dcache_clean_range((void *)(unsigned long)task, sizeof(*task));

    chan->irq_type = chan->cyclic ? DMA_IRQ_ONE_TASK : DMA_IRQ_ALL_TASK;

    value = readl(aich_dma.base + DMA_IRQ_EN_REG);
    value &= ~(DMA_IRQ_MASK(chan->ch_nr));
    value |= chan->irq_type << DMA_IRQ_SHIFT(chan->ch_nr);
    writel(value, aich_dma.base + DMA_IRQ_EN_REG);

    writel(chan->desc->mode, chan->base + DMA_CH_MODE_REG);
    writel((u32)(unsigned long)(chan->desc), chan->base + DMA_CH_TASK_REG);
    if (chan->memset)
        writel(DMA_CH_MEMSET, chan->base + DMA_CH_PAUSE_REG);
    else
        writel(0x00, chan->base + DMA_CH_PAUSE_REG);
    writel(0x01, chan->base + DMA_CH_EN_REG);

#ifdef AIC_DMA_DRV_DEBUG
    hal_dma_reg_dump(chan);
#endif

    return 0;
}

int hal_dma_chan_stop(struct aic_dma_chan *chan)
{
    u32 value;

    CHECK_PARAM(chan != NULL, -EINVAL);

    /* disable irq */
    value = readl(aich_dma.base + DMA_IRQ_EN_REG);
    value &= ~(DMA_IRQ_MASK(chan->ch_nr));
    writel(value, aich_dma.base + DMA_IRQ_EN_REG);

    /* pause */
    writel(0x01, chan->base + DMA_CH_PAUSE_REG);
    /* stop */
    writel(0x00, chan->base + DMA_CH_EN_REG);
    /* resume */
    writel(0x00, chan->base + DMA_CH_PAUSE_REG);

    chan->cyclic = false;
    chan->memset = false;

    /* free task list */
    aic_dma_free_desc(chan);
    return 0;
}

int hal_dma_chan_pause(struct aic_dma_chan *chan)
{
    CHECK_PARAM(chan != NULL, -EINVAL);

    /* pause */
    writel(0x01, chan->base + DMA_CH_PAUSE_REG);

    return 0;
}

int hal_dma_chan_resume(struct aic_dma_chan *chan)
{
    CHECK_PARAM(chan != NULL, -EINVAL);

    /* resume */
    writel(0x00, chan->base + DMA_CH_PAUSE_REG);

    return 0;
}

int hal_dma_chan_terminate_all(struct aic_dma_chan *chan)
{
    CHECK_PARAM(chan != NULL, -EINVAL);

    hal_dma_chan_stop(chan);

    return 0;
}

int hal_dma_chan_register_cb(struct aic_dma_chan *chan,
                                     dma_async_callback callback,
                                     void *callback_param)
{
    CHECK_PARAM(chan != NULL && callback != NULL && callback_param != NULL, -EINVAL);

    chan->callback = callback;
    chan->callback_param = callback_param;

    return 0;
}

int hal_dma_chan_config(struct aic_dma_chan *chan,
                               struct dma_slave_config *config)
{

    CHECK_PARAM(chan != NULL && config != NULL, -EINVAL);

    memcpy(&chan->cfg, config, sizeof(*config));

    return 0;
}

int hal_release_dma_chan(struct aic_dma_chan *chan)
{
    CHECK_PARAM(chan != NULL && chan->used != 0, -EINVAL);

    /* free task list */
    aic_dma_free_desc(chan);

    chan->used = 0;

    return 0;
}

struct aic_dma_chan *hal_request_dma_chan(void)
{
    int i = 0;
    struct aic_dma_chan *chan;

    for (i = 0; i < AIC_DMA_CH_NUM; i++)
    {
        chan = &aich_dma.dma_chan[i];
        if (chan->used == 0)
        {
            chan->used = 1;
            chan->cyclic = false;
            chan->memset = false;
            chan->irq_type = 0;
            chan->callback = NULL;
            chan->callback_param = NULL;
            chan->desc = NULL;
            return chan;
        }
    }

    return NULL;
}

int hal_dma_init(void)
{
    int i;

    aich_dma.base = DMA_BASE;

    for (i=0; i<AIC_DMA_CH_NUM; i++){
        aich_dma.dma_chan[i].ch_nr = i;
        aich_dma.dma_chan[i].base = aich_dma.base + 0x100 + i * 0x40;
    }

    aich_dma.freetask = NULL;
    for (i=0; i<TASK_MAX_NUM; i++){
        aic_dma_task_free(&aich_dma.task[i]);
    }

    writel(0x0, aich_dma.base + DMA_IRQ_EN_REG);
    return 0;
}
