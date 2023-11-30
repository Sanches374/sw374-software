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

static struct aic_dma_dev aich_dma __ALIGNED(CACHE_LINE_SIZE) = {
    .base = DMA_BASE,
    .burst_length = BIT(1) | BIT(4) | BIT(8) | BIT(16),
    .addr_widths = AIC_DMA_BUS_WIDTH,
};

struct aic_dma_dev *get_aic_dma_dev(void)
{
    return &aich_dma;
}

static void hal_dma_reg_dump(struct aic_dma_chan *chan)
{
    int i;
    struct aic_dma_dev *aich_dma;

    aich_dma = get_aic_dma_dev();

    printf("\nCommon register: \n");
    for (i = 0; i < AIC_DMA_CH_NUM / DMA_IRQ_CHAN_NR; i++) {
    printf( "    IRQ_EN(%d) 0x%x, \tIRQ_STA(%d) 0x%x\n",
            i, readl(aich_dma->base + DMA_IRQ_EN_REG(i)),
            i, readl(aich_dma->base + DMA_IRQ_STA_REG(i)));
    }

    printf("Ch%d register: ID:%x\n"
            "    Enable 0x%x, \tPause 0x%x, Task 0x%x\n"
            "    Config1 0x%x, \tConfig2 0x%x, \tSrc 0x%x, \tDST 0x%x\n"
            "    Byte Counter 0x%d\n",
            chan->ch_nr,
            readl(chan->base + DMA_LINK_ID_REG),
            readl(chan->base + DMA_CH_EN_REG),
            readl(chan->base + DMA_CH_PAUSE_REG),
            readl(chan->base + DMA_CH_TASK_REG),
            readl(chan->base + DMA_TASK_CFG1_REG),
            readl(chan->base + DMA_TASK_CFG2_REG),
            readl(chan->base + DMA_SRC_ADDR_REG),
            readl(chan->base + DMA_DST_ADDR_REG),
            readl(chan->base + DMA_CH_TASK_BCNT_REG));
}

static void hal_dma_task_dump(struct aic_dma_chan *chan)
{
    struct aic_dma_task *task;

    printf("\nDMA Ch%d: desc = 0x%lx\n", chan->ch_nr, (unsigned long)chan->desc);

    for (task = chan->desc; task != NULL; task = task->v_next)
    {
        printf(" task_id (0x%x):\n"
                "\tcfg1 - 0x%x,\tblock_len - 0x%x,\tsrc - 0x%x,\tdst - 0x%x,\tlen - 0x%x\n"
                "\tcfg2 - 0x%x,\tdata_src - 0x%x,\tdata_dst - 0x%x\n"
                "\tp_next - 0x%x, mode - 0x%x, v_next - 0x%lx\n",
                task->link_id,
                task->cfg1, task->block_len, task->src, task->dst, task->len,
                task->cfg2, task->data_src, task->data_dst,
                task->p_next, task->mode,(unsigned long)task->v_next);
    }
}

int hal_dma_chan_dump(int ch_nr)
{
    struct aic_dma_chan *chan;
    struct aic_dma_dev *aich_dma;

    aich_dma = get_aic_dma_dev();

    CHECK_PARAM(ch_nr < AIC_DMA_CH_NUM, -EINVAL);

    chan = &aich_dma->dma_chan[ch_nr];

    hal_dma_task_dump(chan);
    hal_dma_reg_dump(chan);

    return 0;
}

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
    case DMA_SLAVE_BUSWIDTH_16_BYTES:
        return 4;
    default:
        /* For 1 byte width or fallback */
        return 0;
    }
}

int aic_set_burst(struct dma_slave_config *sconfig,
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
            src_addr_width = DMA_SLAVE_BUSWIDTH_16_BYTES;
        src_maxburst = src_maxburst ? src_maxburst : 16;
        break;
    case DMA_DEV_TO_MEM:
        if (dst_addr_width == DMA_SLAVE_BUSWIDTH_UNDEFINED)
            dst_addr_width = DMA_SLAVE_BUSWIDTH_16_BYTES;
        dst_maxburst = dst_maxburst ? dst_maxburst : 16;
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

struct aic_dma_task *aic_dma_task_alloc(void)
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

void *aic_dma_task_add(struct aic_dma_task *prev,
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

void aic_dma_free_desc(struct aic_dma_chan *chan)
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

enum dma_status hal_dma_chan_tx_status(struct aic_dma_chan *chan,
                                       u32 *left_size)
{
    CHECK_PARAM(chan != NULL && left_size != NULL, -EINVAL);

    if (!(readl(aich_dma.base + DMA_CH_STA_REG) & BIT(chan->ch_nr)))
        return DMA_COMPLETE;

    *left_size = readl(chan->base + DMA_CH_LEFT_REG);
        return DMA_IN_PROGRESS;

    return DMA_COMPLETE;
}

void hal_dma_irq_enable(struct aic_dma_chan *chan)
{
    u32 irq_reg, irq_offset;
    u32 val;
    struct aic_dma_dev *aich_dma;
    aich_dma = get_aic_dma_dev();

    irq_reg = chan->ch_nr / DMA_IRQ_CHAN_NR;
    irq_offset = chan->ch_nr % DMA_IRQ_CHAN_NR;

    val = chan->irq_type << DMA_IRQ_SHIFT(irq_offset);
    writel(val, aich_dma->base + DMA_IRQ_EN_REG(irq_reg));

}

int hal_dma_irq_disable(struct aic_dma_chan *chan)
{
    u32 irq_reg, irq_offset;
    u32 val;
    struct aic_dma_dev *aich_dma;
    aich_dma = get_aic_dma_dev();

    irq_reg = chan->ch_nr / DMA_IRQ_CHAN_NR;
    irq_offset = chan->ch_nr % DMA_IRQ_CHAN_NR;

    val = chan->irq_type << DMA_IRQ_SHIFT(irq_offset);
    writel(val, aich_dma->base + DMA_IRQ_DIS_REG(irq_reg));
    return 0;
}

int hal_dma_chan_stop(struct aic_dma_chan *chan)
{
    CHECK_PARAM(chan != NULL, -EINVAL);
    /* disable irq */
    hal_dma_irq_disable(chan);
    /* pause */
    hal_dma_chan_pause(chan);
    /* stop */
    writel(0x00, chan->base + DMA_CH_EN_REG);
    /* resume */
    hal_dma_chan_resume(chan);

    chan->cyclic = false;
    chan->memset = false;

    /* free task list */
    aic_dma_free_desc(chan);
    return 0;
}

/* dma chan ctl2 */
int hal_dma_chan_resume(struct aic_dma_chan *chan)
{
    CHECK_PARAM(chan != NULL, -EINVAL);
    /* resume */
    writel(DMA_CH_RESUME, chan->base + DMA_CH_PAUSE_REG);

    return 0;
}

int hal_dma_chan_pause(struct aic_dma_chan *chan)
{
    u32 val;
    CHECK_PARAM(chan != NULL, -EINVAL);

    /* pause */
    val = readl(chan->base + DMA_CH_PAUSE_REG);
    val |= DMA_CH_TASK_PAUSE;
    writel(val, chan->base + DMA_CH_PAUSE_REG);

    return 0;
}

int hal_dma_chan_link_pause(struct aic_dma_chan *chan)
{
    u32 val;
    CHECK_PARAM(chan != NULL, -EINVAL);

    /* pause */
    val = readl(chan->base + DMA_CH_PAUSE_REG);
    val |= DMA_CH_LINK_PAUSE;
    writel(val, chan->base + DMA_CH_PAUSE_REG);

    return 0;
}

int hal_dma_chan_abandon(struct aic_dma_chan *chan)
{
    writel(DMA_CH_ABANDON, chan->base + DMA_CH_PAUSE_REG);
    return 0;
}

int hal_dma_chan_terminate_all(struct aic_dma_chan *chan)
{
    CHECK_PARAM(chan != NULL, -EINVAL);

    hal_dma_chan_stop(chan);

    return 0;
}

int hal_dma_chan_wb_enable(struct aic_dma_chan *chan,
                        u32 src_addr, u32 dst_addr)
{
    /* enable chan write back function*/
    writel(BIT(4), chan->base + DMA_CH_CTL3_REG);
    writel(BIT(31),chan->base + DMA_TASK_LEN_REG);

    /* set write back addr */
    writel(src_addr, chan->base + DMA_SRC_WB_ADDR_SET_REG);
    writel(dst_addr, chan->base + DMA_DST_WB_ADDR_SET_REG);

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

    for (i = 0; i < AIC_DMA_CH_NUM; i++) {
        aich_dma.dma_chan[i].ch_nr = i;
        aich_dma.dma_chan[i].base = aich_dma.base + 0x100
                                    + i * DMA_CHAN_OFFSET;
    }

    aich_dma.freetask = NULL;
    for (i = 0; i < TASK_MAX_NUM; i++)
        aic_dma_task_free(&aich_dma.task[i]);


    for (i = 0; i < AIC_DMA_CH_NUM / DMA_IRQ_CHAN_NR; i++) {
        writel(0x0, aich_dma.base + DMA_IRQ_EN_REG(i));
    }

    return 0;
}

irqreturn_t hal_dma_irq(int irq, void *arg)
{
    int i, j;
    u32 status;
    struct aic_dma_chan *chan;
    struct aic_dma_dev *aich_dma;
    dma_async_callback cb = NULL;
    void *cb_data = NULL;

    aich_dma = get_aic_dma_dev();

    for (i = 0; i < AIC_DMA_CH_NUM / DMA_IRQ_CHAN_NR; i++) {

        /* get dma irq pending */
        status = readl(aich_dma->base + DMA_IRQ_STA_REG(i));
        if (!status) {
            /* none irq trigger */
            continue;
        }

        pr_debug("IRQ status:0x%x->0x%x\n",
                aich_dma->base + DMA_IRQ_STA_REG(i), status);

        /* clear irq pending */
        writel(status, aich_dma->base + DMA_IRQ_STA_REG(i));

        /* process irq for every dma channel */
        for (j = 0; (j < DMA_IRQ_CHAN_NR) && status; j++, status >>= DMA_IRQ_CH_WIDTH) {
            chan = &aich_dma->dma_chan[j + i * DMA_IRQ_CHAN_NR];

            if ((!chan->used) || !(status & chan->irq_type))
                continue;

            cb = chan->callback;
            cb_data = chan->callback_param;
            if (cb)
            {
                cb(cb_data);
            }
        }


    }
    return IRQ_HANDLED;
}

int hal_dma_chan_prep_memset(struct aic_dma_chan *chan,
                             u32 p_dest, u32 value, u32 len)
{
    struct aic_dma_task *task;
    CHECK_PARAM(chan != NULL && len != 0, -EINVAL);

    CHECK_PARAM((p_dest % AIC_DMA_ALIGN_SIZE) == 0, -EINVAL);

    task = aic_dma_task_alloc();
    CHECK_PARAM(task != NULL, -ENOMEM);

    task->link_id = DMA_LINK_ID_DEF;
    task->block_len = DMA_FIFO_SIZE / 2;
    task->src = p_dest;
    task->dst = p_dest;
    task->len = len;
    task->cfg1 =
        (DMA_ID_DRAM << SRC_PORT_BITSHIFT) | (DMA_ID_DRAM << DST_PORT_BITSHIFT) |
        (TYPE_MEMORYSET << SRC_TYPE_BITSHIFT) | (TYPE_MEMORYSET << DST_TYPE_BITSHIFT) |
        (3 << SRC_BURST_BITSHIFT) | (3 << DST_BURST_BITSHIFT)|
        (4 << SRC_WIDTH_BITSHIFT) | (4 << DST_WIDTH_BITSHIFT);
    task->cfg2 = 0;

    task->mode = DMA_S_WAIT_D_WAIT;

    aicos_dcache_clean_invalid_range((void *)(unsigned long)task->dst, task->len);

    aic_dma_task_add(NULL, task, chan);

    writel(value, chan->base + DMA_CH_MEM_SET_REG);
    chan->memset = true;

#ifdef AIC_DMA_DRV_DEBUG
    hal_dma_task_dump(chan);
#endif
    return 0;
}


int hal_dma_chan_prep_memcpy(struct aic_dma_chan *chan,
                             u32 p_dest, u32 p_src, u32 len)
{
    struct aic_dma_task *task;

    CHECK_PARAM(chan != NULL && len != 0, -EINVAL);

    CHECK_PARAM((p_dest%AIC_DMA_ALIGN_SIZE) == 0 && (p_src%AIC_DMA_ALIGN_SIZE) == 0, -EINVAL);

    task = aic_dma_task_alloc();
    CHECK_PARAM(task != NULL, -ENOMEM);

    task->link_id = DMA_LINK_ID_DEF;
    task->block_len = DMA_FIFO_SIZE / 2;
    task->src = p_src;
    task->dst = p_dest;
    task->len = len;
	task->cfg1 =
        (DMA_ID_DRAM << SRC_PORT_BITSHIFT)  | (DMA_ID_DRAM << DST_PORT_BITSHIFT)  |
		(TYPE_MEMORY << SRC_TYPE_BITSHIFT)  | (TYPE_MEMORY << DST_TYPE_BITSHIFT)  |
		(3 << SRC_BURST_BITSHIFT) | (3 << DST_BURST_BITSHIFT)|
		(4 << SRC_WIDTH_BITSHIFT) | (4 << DST_WIDTH_BITSHIFT);

    task->cfg2 = 0;

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
    struct dma_slave_config *sconfig = &chan->cfg;
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
    task->link_id = DMA_LINK_ID_DEF;
    task->len = len;
    task->src = p_src;
    task->dst = p_dest;
    task->cfg1 = task_cfg;
    if (dir == DMA_MEM_TO_DEV) {
        task->block_len = sconfig->dst_addr_width * sconfig->dst_maxburst;
        task->cfg1 |= (DMA_ID_DRAM << SRC_PORT_BITSHIFT) |
                    ((chan->cfg.slave_id & DMA_DRQ_PORT_MASK) << DST_PORT_BITSHIFT) |
                    (TYPE_MEMORY << SRC_TYPE_BITSHIFT);
        if (sconfig->dst_maxburst != 1)
            task->cfg1 |= (TYPE_BURST << DST_TYPE_BITSHIFT);
        else
            task->cfg1 |= (TYPE_IO_SINGLE << DST_TYPE_BITSHIFT);
        task->mode = DMA_S_WAIT_D_HANDSHAKE;
        aicos_dcache_clean_range((void *)(unsigned long)task->src, task->len);
    } else {
        task->block_len = sconfig->src_addr_width * sconfig->src_maxburst;
        task->cfg1 |= (DMA_ID_DRAM << DST_PORT_BITSHIFT) |
                    ((chan->cfg.slave_id & DMA_DRQ_PORT_MASK) << SRC_PORT_BITSHIFT) |
                    (TYPE_MEMORY << DST_TYPE_BITSHIFT);
        if (sconfig->src_maxburst != 1)
            task->cfg1 |= (TYPE_BURST << SRC_TYPE_BITSHIFT);
        else
            task->cfg1 |= (TYPE_IO_SINGLE << SRC_TYPE_BITSHIFT);
        task->mode = DMA_S_HANDSHAKE_D_WAIT;
        aicos_dcache_clean_invalid_range((void *)(unsigned long)task->dst, task->len);
    }
    task->cfg2 = 0;

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
    struct dma_slave_config *sconfig = &chan->cfg;
    u32 task_cfg;
    u32 periods;
    u32 i;
    int ret;

    CHECK_PARAM(chan != NULL && buf_len != 0 && period_len != 0, -EINVAL);

    CHECK_PARAM((p_buf_addr % AIC_DMA_ALIGN_SIZE) == 0
                && (buf_len % AIC_DMA_ALIGN_SIZE) == 0, -EINVAL);

    ret = aic_set_burst(&chan->cfg, dir, &task_cfg);
    if (ret) {
        hal_log_err("Invalid DMA configuration\n");
        return -EINVAL;
    }

    periods = buf_len / period_len;

    for (i = 0; i < periods; i++) {
        task = aic_dma_task_alloc();
        if (task == NULL) {
            aic_dma_free_desc(chan);
            return -ENOMEM;
        }

        task->link_id = DMA_LINK_ID_DEF;
        task->block_len = 0;
        task->len = period_len;
        task->cfg1 = task_cfg;
        if (dir == DMA_MEM_TO_DEV) {
            task->src = p_buf_addr + period_len * i;
            task->dst = chan->cfg.dst_addr;
            task->block_len = sconfig->dst_addr_width * sconfig->dst_maxburst;
            task->cfg1 |= (DMA_ID_DRAM << SRC_PORT_BITSHIFT) |
                    ((chan->cfg.slave_id & DMA_DRQ_PORT_MASK) << DST_PORT_BITSHIFT) |
                    (TYPE_MEMORY << SRC_TYPE_BITSHIFT);
            if (sconfig->dst_maxburst != 1)
                task->cfg1 |= (TYPE_BURST << DST_TYPE_BITSHIFT);
            else
                task->cfg1 |= (TYPE_IO_SINGLE << DST_TYPE_BITSHIFT);
            task->mode = DMA_S_WAIT_D_HANDSHAKE;
            aicos_dcache_clean_range((void *)(unsigned long)task->src, task->len);
        } else {
            task->src = chan->cfg.src_addr;
            task->dst = p_buf_addr + period_len * i;
            task->block_len = sconfig->dst_addr_width * sconfig->dst_maxburst;
            task->cfg1 |= (DMA_ID_DRAM << DST_PORT_BITSHIFT) |
                    ((chan->cfg.slave_id & DMA_DRQ_PORT_MASK) << SRC_PORT_BITSHIFT)|
                    (TYPE_MEMORY << DST_TYPE_BITSHIFT);
            if (sconfig->src_maxburst != 1)
                task->cfg1 |= (TYPE_BURST << SRC_TYPE_BITSHIFT);
            else
                task->cfg1 |= (TYPE_IO_SINGLE << SRC_TYPE_BITSHIFT);
            task->mode = DMA_S_HANDSHAKE_D_WAIT;
            aicos_dcache_clean_invalid_range((void *)(unsigned long)task->dst, task->len);
        }
        task->cfg2 = 0;

        prev = aic_dma_task_add(prev, task, chan);
    }

    prev->p_next = __pa((unsigned long)chan->desc);

    chan->cyclic = true;

#ifdef AIC_DMA_DRV_DEBUG
    hal_dma_task_dump(chan);
#endif
    return 0;
}

int hal_dma_chan_start(struct aic_dma_chan *chan)
{
    struct aic_dma_task *task;

    CHECK_PARAM(chan != NULL && chan->desc != NULL, -EINVAL);

    for (task = chan->desc; task != NULL; task = task->v_next)
        aicos_dcache_clean_range((void *)(unsigned long)task, sizeof(*task));

    chan->irq_type = chan->cyclic ? DMA_IRQ_ONE_TASK : DMA_IRQ_LINK_TASK;
    chan->irq_type |= DMA_IRQ_ID_ERR | DMA_IRQ_ADDR_ERR | DMA_IRQ_RD_AHB_ERR |
                        DMA_IRQ_WT_AHB_ERR | DMA_IRQ_WT_AXI_ERR;
    /* enable chan irq */
    hal_dma_irq_enable(chan);
    /* add dma desc */
    writel((u32)(unsigned long)(chan->desc), chan->base + DMA_CH_TASK_ADD1_REG);
    /* resume */
    hal_dma_chan_resume(chan);
    /* enable dma chan*/
    writel(0x1, chan->base + DMA_CH_CTL1_REG);

    #ifdef AIC_DMA_DRV_DEBUG
    hal_dma_reg_dump(chan);
    #endif

    return 0;
}

void hal_dma_linkid_set(u32 id)
{
    struct aic_dma_dev *aich_dma;
    aich_dma = get_aic_dma_dev();
    writel(id, aich_dma->base + DMA_LINK_ID_REG);
}

void hal_dma_linkid_rst(void)
{
    struct aic_dma_dev *aich_dma;
    aich_dma = get_aic_dma_dev();
    writel(DMA_LINK_ID_DEF, aich_dma->base + DMA_LINK_ID_REG);
}

