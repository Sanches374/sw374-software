/*
 * Copyright (c) 2024, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#include <rtconfig.h>
#ifdef RT_USING_FINSH
#include <rthw.h>
#include <rtthread.h>
#include <string.h>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <aic_core.h>

#include "mpp_fb.h"

#define MIPI_DSI_DISABLE_COMMAND 0
#define MIPI_DSI_UPDATE_COMMAND  1
#define MIPI_DSI_SEND_COMMAND    2

enum AIC_COM_TYPE {
    AIC_DE_COM   = 0x00,  /* display engine component */
    AIC_RGB_COM  = 0x01,  /* rgb component */
    AIC_LVDS_COM = 0x02,  /* lvds component */
    AIC_MIPI_COM = 0x03,  /* mipi dsi component */
    AIC_DBI_COM  = 0x04,  /* mipi dbi component */
};

struct panel_rgb {
    unsigned int mode;
    unsigned int format;
    unsigned int clock_phase;
    unsigned int data_order;
    unsigned int data_mirror;
};

struct panel_lvds {
    unsigned int mode;
    unsigned int link_mode;
    unsigned int link_swap;
    unsigned int pols;
    unsigned int line;
};

struct dsi_command
{
    unsigned char buf[1024];
    size_t len;
    unsigned int command_on;
};

struct panel_dsi {
    unsigned int mode;
    unsigned int format;
    unsigned int lane_num;
    unsigned int vc_num;
    unsigned int dc_inv;
    unsigned int ln_polrs;
    unsigned int ln_assign;
    struct dsi_command command;
};

static inline int char2int(char ch)
{
    return ch - '0';
}

static long long int str2int(char *_str)
{
    if (_str == NULL) {
        pr_err("The string is empty!\n");
        return -1;
    }

    if (strncmp(_str, "0x", 2))
        return atoi(_str);
    else
        return strtoll(_str, NULL, 16);
}

static void usage(char *app)
{
    printf("Usage: %s [Options], built on %s %s\n", app, __DATE__, __TIME__);
    printf("\t-u, --usage \n");
    printf("\t-t, --timing: config display timing \n");
    printf("\t-c, --connector: config display interface \n");
    printf("\t-g, --get: get aipq tools config \n");
    printf("\t-p, --pin: set or clr gpio \n");
    printf("\t-i, --input: input dsi command file \n");
    printf("\n");
    printf("Example: %s \n", app);
}

static int get_file_size(const char *path)
{
    struct stat st;

    stat(path, &st);

    return st.st_size;
}

static void aicpq_print_config(struct aicfb_pq_config *config, int connector_type)
{
        printf("display timing:\n\tpixelclock: %d\n"
                "\thactive: %d\n"
                "\thfront_porch: %d\n"
                "\thback_porch: %d\n"
                "\thsync_len: %d\n"
                "\tvactive: %d\n"
                "\tvfront_porch: %d\n"
                "\tvback_porch: %d\n"
                "\tvsync_len: %d\n",
                config->timing->pixelclock,
                config->timing->hactive,
                config->timing->hfront_porch,
                config->timing->hback_porch,
                config->timing->hsync_len,
                config->timing->vactive,
                config->timing->vfront_porch,
                config->timing->vback_porch,
                config->timing->vsync_len);

        switch (connector_type)
        {
            case AIC_RGB_COM:
            {
                struct panel_rgb *rgb = config->data;

                printf("RGB info:\n"
                        "\tmode:%d\n"
                        "\tformat:%d\n"
                        "\tclock_phase:%d\n"
                        "\tdata_order:%08x\n"
                        "\tdata_mirror:%d\n",
                        rgb->mode,
                        rgb->format,
                        rgb->clock_phase,
                        rgb->data_order,
                        rgb->data_mirror);
                break;
            }
            case AIC_LVDS_COM:
            {
                struct panel_lvds *lvds = config->data;

                printf("lvds info:\n"
                        "\tmode:%d\n"
                        "\tlink_mode:%d\n"
                        "\tlink_swap:%d\n"
                        "\tpols:%x\n"
                        "\tline:%x\n",
                        lvds->mode,
                        lvds->link_mode,
                        lvds->link_swap,
                        lvds->pols,
                        lvds->line);
                break;
            }
            case AIC_MIPI_COM:
            {
                struct panel_dsi *dsi = config->data;

                printf("dsi info:\n"
                        "\tmode:%d\n"
                        "\tformat:%d\n"
                        "\tlane_num:%d\n"
                        "\tvc_num:%d\n"
                        "\tdc_inv:%d\n"
                        "\tln_polrs:%d\n"
                        "\tln_assign:%04x\n",
                        dsi->mode,
                        dsi->format,
                        dsi->lane_num,
                        dsi->vc_num,
                        dsi->dc_inv,
                        dsi->ln_polrs,
                        dsi->ln_assign);
                break;
            }
            default:
                break;
        }
}

#define AIPQ_TIMING_SRTING_LEN  29

static void aipq_parse_timing(struct display_timing *timing)
{
    char str_3[4] = {0};
    char str_4[5] = {0};
    int len = 0;

    len = strlen(optarg);
    if (len != AIPQ_TIMING_SRTING_LEN) {
        pr_err("Invaild timing table, table len %d\n", len);
        return;
    }

    strncpy(str_3, optarg + 0, 3);
    timing->pixelclock = str2int(str_3) * 1000 * 1000;

    strncpy(str_4, optarg + 3, 4);
    timing->hactive = str2int(str_4);

    strncpy(str_3, optarg + 7, 3);
    timing->hfront_porch = str2int(str_3);

    strncpy(str_3, optarg + 10, 3);
    timing->hback_porch = str2int(str_3);

    strncpy(str_3, optarg + 13, 3);
    timing->hsync_len = str2int(str_3);

    strncpy(str_4, optarg + 16, 4);
    timing->vactive = str2int(str_4);

    strncpy(str_3, optarg + 20, 3);
    timing->vfront_porch = str2int(str_3);

    strncpy(str_3, optarg + 23, 3);
    timing->vback_porch = str2int(str_3);

    strncpy(str_3, optarg + 26, 3);
    timing->vsync_len = str2int(str_3);
}

static int aipq_config_test(int argc, char **argv)
{
    struct mpp_fb *fb = NULL;
    struct aicfb_pq_config config = { 0 };
    struct display_timing timing = { 0 };
    struct panel_rgb rgb = { 0 };
    struct panel_lvds lvds = { 0 };
    struct panel_dsi dsi = { 0 };
    int connector_type = 1;
    int c, ret = 0;

    const char sopts[] = "g:p:i:t:c:u";
    const struct option lopts[] = {
        {"timing",          required_argument, NULL, 't'},
        {"connector",       required_argument, NULL, 'c'},
        {"get",             required_argument, NULL, 'g'},
        {"pin",             required_argument, NULL, 'p'},
        {"input",           required_argument, NULL, 'i'},
        {"usage",                 no_argument, NULL, 'u'},
        {0, 0, 0, 0}
    };

    fb = mpp_fb_open();
    if(!fb) {
        pr_err("mpp fb open failed\n");
        return -1;
    }

    optind = 0;
    while ((c = getopt_long(argc, argv, sopts, lopts, NULL)) != -1) {
        switch (c) {
        case 'u':
            usage(argv[0]);
            return 0;
        case 'p':
        {
            u32 base = str2int(argv[2]);
            u32 id = str2int(argv[3]);
            u32 set = str2int(argv[4]);
            u32 mask, value;
            void *addr;

            mask = (0x1 << id);

            addr = (void*)(uintptr_t)base;
            value = readl(addr);

            if (set)
                value = value | mask;
            else
                value = value & ~(mask);

            writel(value, addr);

            break;
        }
        case 'i':
        {
            int fd = -1, ret = -1;
            int fsize = 0;

            if (strlen(optarg) < 3) {
                dsi.command.command_on = MIPI_DSI_DISABLE_COMMAND;
                config.data  = &dsi;
                mpp_fb_ioctl(fb, AICFB_PQ_SET_CONFIG, &config);
                break;
            }

            fd = open(optarg, O_RDONLY);
            if (fd < 0) {
                printf("Failed to open %s.\n", optarg);
                return -1;
            }
            fsize = get_file_size(optarg);
            dsi.command.len = fsize;

            ret = read(fd, (void *)dsi.command.buf, fsize);
            if (ret != fsize) {
                printf("read(%d) return %d.\n", fsize, ret);
                return -1;
            }

            dsi.command.command_on = MIPI_DSI_UPDATE_COMMAND;
            config.data  = &dsi;
            mpp_fb_ioctl(fb, AICFB_PQ_SET_CONFIG, &config);
            break;
        }
        case 't':
        {
            aipq_parse_timing(&timing);

            config.timing = &timing;
            continue;
        }
        case 'c':
        {
            char con_info[18] = { 0 };

            memcpy(con_info, optarg, strlen(optarg));
            connector_type = con_info[0] - '0';

            switch (connector_type)
            {
                case AIC_RGB_COM:
                {
                    unsigned int data_order_id = 0;
                    unsigned int data_order_val[] = {
                        0x02100210, 0x02010201, 0x00120012,
                        0x00210021, 0x01200120, 0x01020102
                    };

                    rgb.mode        = char2int(con_info[1]);
                    rgb.format      = char2int(con_info[2]);
                    rgb.clock_phase = char2int(con_info[3]);
                    data_order_id   = char2int(con_info[4]);
                    rgb.data_mirror = char2int(con_info[5]);
                    rgb.data_order  = data_order_val[data_order_id];
                    config.data     = &rgb;
                    break;
                }
                case AIC_LVDS_COM:
                {
                    char polr[3] = {0};
                    lvds.mode      = char2int(con_info[1]);
                    lvds.link_mode = char2int(con_info[2]);
                    lvds.link_swap = char2int(con_info[3]);

                    memcpy(polr, con_info + 4, 2);

                    lvds.pols      = strtoll(polr, NULL, 16);
                    lvds.line      = strtoll(con_info + 6, NULL, 16);
                    config.data    = &lvds;
                    break;
                }
                case AIC_MIPI_COM:
                {
                    char ln_polrs[3] = {0};

                    dsi.mode     = char2int(con_info[1]);
                    dsi.format   = char2int(con_info[2]);
                    dsi.lane_num = char2int(con_info[3]);
                    dsi.vc_num   = char2int(con_info[4]);
                    dsi.dc_inv   = char2int(con_info[5]);

                    memcpy(ln_polrs, con_info + 6, 2);

                    dsi.ln_polrs           = strtoll(ln_polrs, NULL, 16);
                    dsi.ln_assign          = strtoll(con_info + 8, NULL, 16);
                    dsi.command.command_on = MIPI_DSI_SEND_COMMAND;
                    config.data            = &dsi;
                    break;
                }
                default:
                    break;
            }

            mpp_fb_ioctl(fb, AICFB_PQ_SET_CONFIG, &config);
            break;
        }
        case 'g':
        {

            connector_type = str2int(optarg);

            config.timing = &timing;
            config.connector_type = connector_type;

            switch (connector_type)
            {
                case AIC_RGB_COM:
                    config.data = &rgb;
                    break;
                case AIC_LVDS_COM:
                    config.data = &lvds;
                    break;
                case AIC_MIPI_COM:
                    config.data = &dsi;
                    break;
                default:
                    break;
            }

            mpp_fb_ioctl(fb, AICFB_PQ_GET_CONFIG, &config);
            aicpq_print_config(&config, connector_type);
            break;
        }
        default:
            pr_err("Invalid parameter: %#x\n", ret);
            usage(argv[0]);
            return 0;
        }
    }
    return 0;
}

MSH_CMD_EXPORT_ALIAS(aipq_config_test, aipq_cfg, set aipq config);
#endif /* RT_USING_FINSH */

