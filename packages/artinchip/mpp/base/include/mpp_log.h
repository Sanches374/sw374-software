/*
 * Copyright (c) 2022-2023, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 *  author: <qi.xu@artinchip.com>
 *  Desc: log module
 */

#ifndef MPP_LOG_H
#define MPP_LOG_H

#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

enum log_level {
	MPP_LOG_ERROR = 3,
	MPP_LOG_WARNING,
	MPP_LOG_INFO,
	MPP_LOG_DEBUG,
	MPP_LOG_VERBOSE
};

#define MPP_LOG_LEVEL  MPP_LOG_ERROR

/*avoid  redefine warning */
#ifdef LOG_TAG
#undef LOG_TAG
#define LOG_TAG "aic_mpp"
#else
#define LOG_TAG "aic_mpp"
#endif

#define TAG_ERROR	"error  "
#define TAG_WARNING	"warning"
#define TAG_INFO	"info   "
#define TAG_DEBUG	"debug  "
#define TAG_VERBOSE	"verbose"

#define mpp_log(level,tag,fmt,arg...)														\
	do{																						\
		if(level <= MPP_LOG_LEVEL)															\
			printf("%s: %s <%s:%d>: "fmt"\n", tag, LOG_TAG, __FUNCTION__, __LINE__, ##arg);	\
	}while(0)


#define loge(fmt, arg...) mpp_log(MPP_LOG_ERROR, TAG_ERROR, "\033[40;31m"fmt"\033[0m", ##arg)
#define logw(fmt, arg...) mpp_log(MPP_LOG_WARNING, TAG_WARNING, "\033[40;33m"fmt"\033[0m", ##arg)
#define logi(fmt, arg...) mpp_log(MPP_LOG_INFO, TAG_INFO, "\033[40;32m"fmt"\033[0m", ##arg)
#if (MPP_LOG_LEVEL < MPP_LOG_DEBUG)
#define logd(fmt, arg...)
#define logv(fmt, arg...)
#else
#define logd(fmt, arg...) mpp_log(MPP_LOG_DEBUG, TAG_DEBUG, fmt, ##arg)
#define logv(fmt, arg...) mpp_log(MPP_LOG_VERBOSE, TAG_VERBOSE, fmt, ##arg)
#endif

#define time_start(tag) unsigned int time_##tag##_start = aic_get_time_us()
#define time_end(tag) unsigned int time_##tag##_end = aic_get_time_us();\
			fprintf(stderr, #tag " time: %u us\n",\
			time_##tag##_end - time_##tag##_start)

#endif














