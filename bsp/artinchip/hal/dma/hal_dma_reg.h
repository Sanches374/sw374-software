/*
 * Copyright (c) 2022, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ARTINCHIP_HAL_DMA_REG_H_
#define _ARTINCHIP_HAL_DMA_REG_H_

#include "aic_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TASK_MAX_NUM            24

#define DELAY_DEF_VAL           0x40

/*
 * define dma register list
 */
#define DMA_IRQ_EN_REG          (0x0000)
#define DMA_IRQ_STA_REG         (0x0010)
#define DMA_GATE_REG            (0x0028)
#define DMA_CH_STA_REG          (0x0030)

#define DMA_CH_EN_REG           (0x0000)
#define DMA_CH_PAUSE_REG        (0x0004)
#define DMA_CH_TASK_REG         (0x0008)
#define DMA_CH_CFG_REG          (0x000C)
#define DMA_CH_SRC_REG          (0x0010)
#define DMA_CH_SINK_REG         (0x0014)
#define DMA_CH_LEFT_REG         (0x0018)
#define DMA_CH_MODE_REG         (0x0028)
#define DMA_CH_PKG_NUM_REG      (0x0030)
#define DMA_CH_MEMSET_VAL_REG   (0x0034)

/*
 * define macro for access register for specific channel
 */
#define DMA_IRQ_HALF_TASK   BIT(0)
#define DMA_IRQ_ONE_TASK    BIT(1)
#define DMA_IRQ_ALL_TASK    BIT(2)
#define DMA_IRQ_CH_WIDTH    (4)
#define DMA_IRQ_SHIFT(ch)   (DMA_IRQ_CH_WIDTH * (ch))
#define DMA_IRQ_MASK(ch)    (GENMASK(2, 0) << DMA_IRQ_SHIFT(ch))
#define AIC_DMA_BUS_WIDTH                                                      \
    (BIT(DMA_SLAVE_BUSWIDTH_1_BYTE) | BIT(DMA_SLAVE_BUSWIDTH_2_BYTES) |    \
     BIT(DMA_SLAVE_BUSWIDTH_4_BYTES) | BIT(DMA_SLAVE_BUSWIDTH_8_BYTES))

/*
 * define bit index in channel configuration register
 */
#define DST_WIDTH_BITSHIFT  25
#define DST_ADDR_BITSHIFT   24
#define DST_BURST_BITSHIFT  22
#define DST_PORT_BITSHIFT   16
#define SRC_WIDTH_BITSHIFT  9
#define SRC_ADDR_BITSHIFT   8
#define SRC_BURST_BITSHIFT  6
#define SRC_PORT_BITSHIFT   0

#define ADDR_LINEAR_MODE    0
#define ADDR_FIXED_MODE     1

#define DMA_WAIT_MODE       0
#define DMA_HANDSHAKE_MODE  1
#define DMA_DST_MODE_SHIFT  3
#define DMA_SRC_MODE_SHIFT  2
#define DMA_S_WAIT_D_HANDSHAKE  (DMA_HANDSHAKE_MODE << DMA_DST_MODE_SHIFT)
#define DMA_S_HANDSHAKE_D_WAIT  (DMA_HANDSHAKE_MODE << DMA_SRC_MODE_SHIFT)
#define DMA_S_WAIT_D_WAIT   (DMA_WAIT_MODE)

#define DMA_DRQ_PORT_MASK   0x3F

#define DMA_LINK_END_FLAG   0xfffff800

/*
 * define bit index in channel pause register
 */
#define DMA_CH_PAUSE        0x1
#define DMA_CH_MEMSET       0x10
#define DMA_CH_BYTEMODE     0x20

#ifdef __cplusplus
}
#endif

#endif /*_ARTINCHIP_HAL_DMA_REG_H_  */
