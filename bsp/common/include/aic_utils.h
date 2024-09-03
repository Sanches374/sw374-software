/*
 * Copyright (c) 2023-2024, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __AIC_UTILS_H_
#define __AIC_UTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

void hexdump(unsigned char *buf, unsigned long len, int groupsize);
void hexdump_msg(char *msg, unsigned char *buf, unsigned long len, int groupsize);

void show_speed(char *msg, unsigned long len, unsigned long us);
#ifdef LPKG_USING_FDTLIB
int pinmux_fdt_parse(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __AIC_UTILS_H_ */
