/*
 * Copyright (c) 2023-2024, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors:  ArtInChip
 */

#include "com_label_ui.h"

// #if LV_USE_FREETYPE ==  1
// #define MAX_FT_INFO   48
// #define MAX_FONT_SIZE 48
// #define MAX_FONT_TYPE 3

// static lv_style_t ft_styles[MAX_FONT_TYPE][(MAX_FONT_SIZE / 2) - 5 + 2]; /* min font size 8 */
// static lv_ft_info_t ft_info[MAX_FONT_SIZE / 2];

// static int font_size_map(int font_size)
// {
//     if (font_size % 2) return 0;
//     if (font_size <= 10) return 0;
//     if (font_size >= 12 && font_size <= MAX_FONT_SIZE) return (font_size / 2) - 5;
//     return 0;
// }
// #endif

lv_obj_t *commom_label_ui_create(lv_obj_t *parent, int font_size, int font_type)
{
    lv_obj_t *common_label = lv_label_create(parent);
    lv_obj_set_width(common_label, LV_SIZE_CONTENT);
    lv_obj_set_height(common_label, LV_SIZE_CONTENT);
    lv_obj_align(common_label, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_pos(common_label, 0, 0);
    lv_label_set_text(common_label, "ft label");
    lv_obj_set_style_text_color(common_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_opa(common_label, LV_OPA_100, 0);

// #if LV_USE_FREETYPE ==  1
//     printf("enter LV_USE_FREETYPE\n\n\n");
//     static int ft_info_num = 0;
//     lv_ft_info_t *info = &ft_info[ft_info_num];
//     info->name = "/rodata/lvgl_data/font/Lato-Regular.ttf";
//     info->weight = font_size;
//     info->style = font_type;
//     info->mem = NULL;
//     if(!lv_ft_font_init(info)) {
//         LV_LOG_ERROR("create failed.");
//     }
//     ft_info_num++;
//     if (ft_info_num >= MAX_FT_INFO) {
//         ft_info_num = MAX_FT_INFO;
//         return common_label;
//     }
//     /*Create style*/
//     int style_index = font_size_map(font_size);
//     lv_style_t *ft_style = &ft_styles[font_type][style_index];

//     lv_style_init(ft_style);
//     lv_style_set_text_font(ft_style, info->font);
//     lv_obj_add_style(common_label, ft_style, 0);
// #endif

    return common_label;
}
