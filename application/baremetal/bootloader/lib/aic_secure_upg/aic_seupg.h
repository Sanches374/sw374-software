/*
 * Copyright (c) 2023-2024, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __PTUPG_H__
#define __PTUPG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <aic_core.h>
int mbox_cmd(u32 *data, u32 len);

#ifdef __cplusplus
}
#endif
#endif /* __PTUPG_H__ */
