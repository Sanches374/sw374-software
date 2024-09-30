/*
 * Copyright (c) 2024, ArtInChip Technology CO.,LTD. All Rights Reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Author: Chen JunLong <junlong.chen@artinchip.com>
 */

#ifndef __PLATFORM_H
#define __PLATFORM_H

#define BASE_DRAM		(0x00000000)
#define DRAM_SIZE		(0x80000000UL)
#define BASE_SP_SRAM		(0x80040000)
#define BASE_BROM		(0x90000000)
#define BASE_CS_SRAM		(0x90040000)
#define CS_SRAM_SIZE		(0x00008000UL)
#define SP_SRAM_SIZE		(0x00020000UL)
#define END_CS_SRAM		(BASE_CS_SRAM + CS_SRAM_SIZE)
#define END_SP_SRAM		(BASE_SP_SRAM + SP_SRAM_SIZE)

#define BASE_SP_USBD		(0x81200000)
#define BASE_SP_PRCM		(0x88000000)
#define BASE_SP_GPIO		(0x88700000)
#define BASE_SP_UART0		(0x88710000)
#define BASE_SP_UART1		(0x88711000)
#define BASE_SP_WDOG		(0x89000000)
#define BASE_SP_WRI		(0x8900F000)
#define BASE_SP_RTC		(0x89030000)
#define BASE_SP_GTC		(0x89050000)

#define BASE_DMA0		(0x91000000)
#define BASE_DMA1		(0x91010000)
#define BASE_DMA2		(0x91020000)
#define BASE_DCE		(0x910FF000)
#define BASE_USBH0		(0x91210000)
#define BASE_USBH1		(0x91220000)
#define BASE_SPI0		(0x913F0000)
#define BASE_SPI1		(0x91400000)
#define BASE_SPI2		(0x91410000)
#define BASE_SPI3		(0x91420000)
#define BASE_SPI4		(0x91430000)
#define BASE_SDMC0		(0x91440000)
#define BASE_SDMC1		(0x91450000)
#define BASE_AHBCFG		(0x914FE000)
#define BASE_CS_MBOX_CS2SP	(0x91FF0000)
#define BASE_CS_MBOX_CS2SC	(0x91FF1000)
#define BASE_CS_MBOX_CS2SE	(0x91FF2000)
#define BASE_CS_MBOX_SP2CS	(0x91FF3000)
#define BASE_CS_MBOX_SC2CS	(0x91FF4000)
#define BASE_CS_MBOX_SP2SC	(0x91FF5000)
#define BASE_CS_MBOX_SC2SP	(0x91FF6000)
#define BASE_SYSCFG		(0x98000000)

#define BASE_CMU		(0x98020000)
#define BASE_AXICFG		(0x984FE000)
#define BASE_UART0		(0x98710000)
#define BASE_UART1		(0x98711000)
#define BASE_UART2		(0x98712000)
#define BASE_UART3		(0x98713000)
#define BASE_UART4		(0x98714000)
#define BASE_UART5		(0x98715000)
#define BASE_WDOG		(0x99000000)
#define BASE_SID		(0x99010000)

#define BASE_SE_SRAM		(0xF0040000)
#define SE_SRAM_SIZE		(0x00018000UL)
#define END_SE_SRAM		(BASE_SE_SRAM + SE_SRAM_SIZE)

#define IS_SESS_SRAM(x)		(((unsigned long)(x) >= BASE_SE_SRAM) && ((unsigned long)(x) < END_SE_SRAM))
#define IS_CSYS_SRAM(x)		(((unsigned long)(x) >= BASE_CS_SRAM) && ((unsigned long)(x) < END_CS_SRAM))
#define IS_SPSS_SRAM(x)		(((unsigned long)(x) >= BASE_SP_SRAM) && ((unsigned long)(x) < END_SP_SRAM))
#endif
