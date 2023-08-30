/*
 * Copyright (c) 2023, Artinchip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _DISP_CONF_H_
#define _DISP_CONF_H_

/**
 * LVDS options
 */

/* lvds sync mode enable */
#define AIC_LVDS_SYNC_MODE_EN   1

/* lvds swap enable, swap lvds link0 and link1 */
#define AIC_LVDS_SWAP_EN    0

/* lvds data channel swap */
#define AIC_LVDS_SWAP       0x0

/* lvds channel polarities */
#define AIC_LVDS_POL        0x0

/* lvds channel phy */
#define AIC_LVDS_PHY        0xFA

/**
 * MIPI-DSI options
 */

/* data line assignments */
#define LANE_ASSIGNMENTS 0x0123;

/* data line polarities */
#define LANE_POLARITIES  0b1111;

/* data clk inverse */
#define CLK_INVERSE      1

#define VIRTUAL_CHANNEL  0

/**
 * FB ROTATION options
 */

/* drawing buf for GUI, range [1, 2] */
#define AIC_FB_DRAW_BUF_NUM 2

#endif /* _DISP_CONF_H_ */
