/*
 * Copyright (c) 2023-2024, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors:  ArtInChip
 */

#include "showcase_demo.h"
#include "sys_icon_ui.h"

#define LANGUAGE_ENGLISH 0
#define LANGUAGE_CHINESE 1

char cur_screen[64];
extern lv_obj_t *navigator_ui_init(void);
extern void coffee_ui_del(void);

typedef struct _showcase_private_theme {
    lv_color_t color_emphasize;
    uint32_t dark_theme;
    uint32_t language;
} showcase_private_theme;

layer_sys_ui sys_ui;

static void showcase_demo_theme_init(void)
{
    static showcase_private_theme theme_user_data;

    lv_disp_t *disp = lv_disp_get_default();
    theme_user_data.color_emphasize = lv_color_hex(0xF0F0F0);
    theme_user_data.dark_theme = 1;
    theme_user_data.language = LANGUAGE_ENGLISH;

    lv_theme_t *theme = lv_theme_default_init(disp, lv_color_hex(0x1A1D26), lv_color_hex(0x272729), true, LV_FONT_DEFAULT);
    theme->font_small = LV_FONT_DEFAULT;
    theme->font_normal = LV_FONT_DEFAULT;
    theme->font_large = LV_FONT_DEFAULT;
    theme->user_data = &theme_user_data;
    lv_disp_set_theme(disp, theme);
}

void back_home_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);

    if (code == LV_EVENT_SHORT_CLICKED) {
        if (strncmp(cur_screen, "navigator", strlen("navigator")) == 0) {
            return;
        }

        layer_sys_ui_invisible(0);

        if (strncmp(cur_screen, "coffee", strlen("coffee")) == 0)
            coffee_ui_del();

        lv_disp_t *disp = lv_disp_get_default();
        lv_theme_default_init(disp, lv_color_hex(0x1A1D26), lv_color_hex(0x272729), true, LV_FONT_DEFAULT);
        lv_theme_t *theme = lv_theme_default_get();
        lv_disp_set_theme(disp, theme);

        lv_obj_t *obj_navigator = navigator_ui_init();
        lv_theme_apply(obj_navigator);
        lv_scr_load_anim(obj_navigator, LV_SCR_LOAD_ANIM_NONE, 0, 5, true);

        strncpy(cur_screen, "navigator", sizeof(cur_screen));
    }

    /* LV_OBJ_FLAG_USER_4 flag is used to record whether there is movement and the click event will not be triggered if there is movement */
    if (LV_EVENT_PRESSED == code)
        lv_obj_clear_flag(obj, LV_OBJ_FLAG_USER_4);

    if (code == LV_EVENT_PRESSING) {
        if (lv_obj_has_flag(obj, LV_OBJ_FLAG_USER_4) == false)  lv_obj_set_style_opa(obj, LV_OPA_100, 0);
        lv_indev_t *indev = lv_indev_get_act();
        if (indev == NULL) return;

        lv_indev_type_t indev_type = lv_indev_get_type(indev);
        if (indev_type != LV_INDEV_TYPE_POINTER) return;

        lv_point_t vect;
        lv_indev_get_vect(indev, &vect);
        lv_coord_t x = lv_obj_get_x(obj) + vect.x;
        lv_coord_t y = lv_obj_get_y(obj) + vect.y;

        /* after moving it will not trigger click */
        lv_obj_add_flag(obj, LV_OBJ_FLAG_USER_4);

        lv_obj_set_pos(obj, x, y);
    }
}

void showcase_demo_init(void)
{
    showcase_demo_theme_init();

    sys_ui.home_btn = home_icon_ui_create(lv_layer_sys());
    layer_sys_ui_invisible(0);

    lv_obj_t *obj_navigator = navigator_ui_init();

    lv_theme_apply(obj_navigator);

    lv_scr_load(obj_navigator);

    lv_obj_add_event_cb(sys_ui.home_btn, back_home_event_cb, LV_EVENT_ALL, NULL);

    strncpy(cur_screen, "navigator", sizeof(cur_screen));
}

lv_color_t lv_theme_get_color_emphasize(lv_obj_t * obj)
{
    lv_theme_t * th = lv_theme_get_from_obj(obj);
    showcase_private_theme *theme_user_data;
    if (th->user_data != NULL) {
        theme_user_data = (showcase_private_theme *)th->user_data;
        return theme_user_data->color_emphasize;
    }
    return lv_palette_main(LV_PALETTE_BLUE_GREY);
}

void layer_sys_ui_visible(int flag)
{
    lv_obj_clear_flag(sys_ui.home_btn, LV_OBJ_FLAG_HIDDEN);
}

void layer_sys_ui_invisible(int flag)
{
    lv_obj_add_flag(sys_ui.home_btn, LV_OBJ_FLAG_HIDDEN);
}
