/*
 * Copyright (c) 2024, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors: Jiji.CHen <jiji.chen@artinchip.com>
 */

#include <string.h>
#include <finsh.h>
#include <aic_core.h>

#define DATA_LEN_64K    64*1024
#define DATA_LEN_16K    16*1024
#define USAGE \
    "fs help : Get this information.\n" \
    "fs read <filename> : Test filesystem write speed.\n" \
    "fs write <filename> : Test filesystem write speed.\n" \
    "example:\n" \
    "fs read data/testfile.txt\n" \
    "fs read rodata/testfile.txt\n" \
    "fs write rodata/testfile.txt\n" \
    "fs write\n"

static void fs_usage(void)
{
    printf("%s", USAGE);
}

static void show_speed(char *msg, u32 len, u32 us)
{
    u32 tmp, speed;

    /* Split to serval step to avoid overflow */
    tmp = 1000 * len;
    tmp = tmp / us;
    tmp = 1000 * tmp;
    speed = tmp / 1024;

    printf("%s: %d byte, %d us -> %d KB/s\n", msg, len, us, speed);
}

static void test_fs_read(int argc, char **argv)
{
    uint32_t start_us;
    char *data;
    FILE *fp;
    char file_path[128] = { 0 };

    if (argc > 1)
        rt_sprintf(file_path, "%s", argv[1]);
    else
        rt_sprintf(file_path, "%s", "data/testfile.txt");
    data = aicos_malloc_align(0, DATA_LEN_64K, CACHE_LINE_SIZE);
    if (data == NULL) {
        printf("Low memory");
        return;
    }

    fp = fopen(file_path, "rb");
    if (fp == NULL) {
        printf("Open file %s failed!\n", file_path);
        aicos_free_align(0, data);
        return;
    }

    start_us = aic_get_time_us();
    if (DATA_LEN_64K != fread(data, sizeof(char), DATA_LEN_64K, fp)) {
        printf("Fs read data failed!\n");
        goto read_err;
    }
    show_speed("Filesystem read speed", DATA_LEN_64K, aic_get_time_us() - start_us);

read_err:
    fclose(fp);
    aicos_free_align(0, data);
    return;
}

static void test_fs_write(int argc, char **argv)
{
    uint32_t start_us;
    char *data;
    FILE *fp;
    char file_path[128] = { 0 };

    if (argc > 1)
        rt_sprintf(file_path, "%s", argv[1]);
    else
        rt_sprintf(file_path, "%s", "data/testfile.txt");

    data = aicos_malloc_align(0, DATA_LEN_64K, CACHE_LINE_SIZE);
    if (data == NULL) {
        printf("Low memory");
        return;
    }

    fp = fopen(file_path, "w");
    if (fp == NULL) {
        printf("Read file %s failed!\n", file_path);
        aicos_free_align(0, data);
        return;
    }

    start_us = aic_get_time_us();
    if (DATA_LEN_64K != fwrite(data, sizeof(char), DATA_LEN_64K, fp)) {
        printf("Fs write data failed!");
        goto write_err;
    }
    show_speed("Filesystem write speed", DATA_LEN_64K, aic_get_time_us() - start_us);

write_err:
    fclose(fp);
    aicos_free_align(0, data);
    return;
}

static void cmd_test_fs(int argc, char **argv)
{
    if (argc < 2)
        goto help;

    if (!rt_strcmp(argv[1], "help")) {
        goto help;
    } else if (!rt_strcmp(argv[1], "read")) {
        test_fs_read(argc - 1, &argv[1]);
        return;
    } else if (!rt_strcmp(argv[1], "write")) {
        test_fs_write(argc - 1, &argv[1]);
        return;
    }
help:
    fs_usage();
}

MSH_CMD_EXPORT_ALIAS(cmd_test_fs, fs, Test fs);
