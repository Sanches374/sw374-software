/*
 * Copyright (c) 2022, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors: weilin.peng@artinchip.com
 */

#include <aic_core.h>
#include <aic_hal.h>
#include "board.h"

struct aic_sysclk
{
    unsigned long       freq;
    unsigned int        clk_id;
};

struct aic_sysclk aic_sysclk_config[] = {
    //{600000000, CLK_PLL_INT0},
    {1200000000, CLK_PLL_INT1},
    {491520000, CLK_PLL_FRA1},
    {840000000, CLK_PLL_FRA2},
    {240000000, CLK_AXI0},
    {240000000, CLK_AHB0},
    {100000000, CLK_APB0},
    {24000000, CLK_APB1},
    {600000000, CLK_CPU},
#ifdef AIC_USING_UART0
    {48000000, CLK_UART0},
#endif
#ifdef AIC_USING_UART1
    {48000000, CLK_UART0+1},
#endif
#ifdef AIC_USING_UART2
    {48000000, CLK_UART0+2},
#endif
#ifdef AIC_USING_UART3
    {48000000, CLK_UART0+3},
#endif
#ifdef AIC_USING_UART4
    {48000000, CLK_UART0+4},
#endif
#ifdef AIC_USING_UART5
    {48000000, CLK_UART0+5},
#endif
#ifdef AIC_USING_UART6
    {48000000, CLK_UART0+6},
#endif
#ifdef AIC_USING_UART7
    {48000000, CLK_UART0+7},
#endif
};

void aic_board_sysclk_init(void)
{
    uint32_t i = 0;

    for (i=0; i<sizeof(aic_sysclk_config)/sizeof(struct aic_sysclk); i++) {
        hal_clk_set_freq(aic_sysclk_config[i].clk_id, aic_sysclk_config[i].freq);
    }

    /* Enable sys clk */
    hal_clk_enable_deassertrst_iter(CLK_GPIO);
    hal_clk_enable_deassertrst_iter(CLK_GTC);
}

