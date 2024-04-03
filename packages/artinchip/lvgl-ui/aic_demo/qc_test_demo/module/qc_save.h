/*
 * Copyright (c) 2023-2024, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors:  ArtInChip
 */
#ifndef _QC_SAVE_H_
#define _QC_SAVE_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "qc_ops.h"

/*
 * file.csv name:chipid.csv
 * file.csv context:
 * chip_id,status,success_rate,success_num,failure_num,module_num
 * XXXXXX,XXXXX,XXXXX,XXXXX,XXXXX,XXXXX,XXXXX
 *
 * module_name,status
 * XXXXXX,XXXXX
 * XXXXXX,XXXXX
 * XXXXXX,XXXXX
 * XXXXXX,XXXXX
 *
*/

int qc_save_write(struct qc_manager *mgr, char *path);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _QC_SAVE_H_ */
