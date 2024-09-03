/*
 * Copyright (C) 2024 ArtInChip Technology Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors:  Huahui Mai <huahui.mai@artinchip.com>
 *
 * base on base_demo/base_ui.c
 */

#include "usb_osd_video.h"
#include "usb_osd_ui.h"
#include "mpp_fb.h"

#ifdef AIC_MPP_PLAYER_INTERFACE
//#define VIDEO_PLAYER
#endif

//#define FILE_LIST

#ifdef VIDEO_PLAYER
LV_IMG_DECLARE(next_normal);
LV_IMG_DECLARE(next_press);
LV_IMG_DECLARE(pause_norma_1);
LV_IMG_DECLARE(pause_press_1);

LV_IMG_DECLARE(play_normal);
LV_IMG_DECLARE(play_press);
LV_IMG_DECLARE(pre_normal);
LV_IMG_DECLARE(pre_press);
#endif

#ifdef VIDEO_PLAYER
#include "aic_player.h"

#define LVGL_PLAYER_STATE_PLAY  1
#define LVGL_PLAYER_STATE_PAUSE  2
#define LVGL_PLAYER_STATE_STOP   3

struct lvgl_player_context{
    int file_cnt;
    int file_index;
    int player_state;
    struct aic_player *player;
    int sync_flag;
    struct av_media_info media_info;
    int demuxer_detected_flag;
    int player_end;
    struct mpp_size screen_size;
    struct mpp_rect disp_rect;
};

static char g_filename[][256] = {
    LVGL_FILE_LIST_PATH(cartoon.mp4),
};

static struct lvgl_player_context g_lvgl_player_ctx;
static lv_obj_t *g_btn_pre;
static lv_obj_t *g_btn_next;
static lv_obj_t *g_btn_pause_play;
static lv_obj_t *g_btn_hide_show;
static struct mpp_fb *fd_dev;

#ifdef FILE_LIST
static lv_obj_t * g_file_list;
static lv_style_t g_style_scrollbar;
static lv_style_t g_file_list_btn_style_def;
static lv_style_t g_file_list_btn_style_pre;
static lv_style_t g_file_list_btn_style_chk;
#endif

static int lvgl_stop(struct lvgl_player_context *ctx);

void video_back_event_cb(lv_event_t *e)
{
    lv_event_code_t code = (lv_event_code_t)lv_event_get_code(e);
    struct lvgl_player_context *ctx;

    ctx = &g_lvgl_player_ctx;
    if (code == LV_EVENT_CLICKED) {

        lvgl_stop(ctx);
        if (ctx->player_state != LVGL_PLAYER_STATE_STOP) {
            ctx->player_state = LVGL_PLAYER_STATE_STOP;
            lv_imgbtn_set_src(g_btn_pause_play, LV_IMGBTN_STATE_RELEASED , NULL, &play_normal, NULL);
            lv_imgbtn_set_src(g_btn_pause_play, LV_IMGBTN_STATE_PRESSED , NULL, &play_press, NULL);
        }
        //back_logo_screen();
    }
}

static s32 event_handle(void* app_data,s32 event,s32 data1,s32 data2)
{
    int ret = 0;
    struct lvgl_player_context *ctx = (struct lvgl_player_context *)app_data;

    switch (event) {
        case AIC_PLAYER_EVENT_PLAY_END:
            ctx->player_end = 1;
        case AIC_PLAYER_EVENT_PLAY_TIME:
            break;
        case AIC_PLAYER_EVENT_DEMUXER_FORMAT_DETECTED:
            if (AIC_PLAYER_PREPARE_ASYNC == ctx->sync_flag) {
                ctx->demuxer_detected_flag = 1;
            }
            break;
        case AIC_PLAYER_EVENT_DEMUXER_FORMAT_NOT_DETECTED:
            if (AIC_PLAYER_PREPARE_ASYNC == ctx->sync_flag) {
                ctx->player_end = 1;
            }
            break;
        default:
            break;
    }
    return ret;
}

static int lvgl_play(struct lvgl_player_context *ctx)
{
    int k = 0;
    int ret = 0;

    k  = ctx->file_index%ctx->file_cnt;
    aic_player_set_uri(ctx->player,g_filename[k]);
    ctx->sync_flag = AIC_PLAYER_PREPARE_SYNC;
    if (ctx->sync_flag == AIC_PLAYER_PREPARE_ASYNC) {
        ret = aic_player_prepare_async(ctx->player);
    } else {
        ret = aic_player_prepare_sync(ctx->player);
    }
    if (ret) {
        return -1;
    }
    if (ctx->sync_flag == AIC_PLAYER_PREPARE_SYNC) {
        ret = aic_player_start(ctx->player);
        if(ret != 0) {
            return -1;
        }
        aic_player_get_screen_size(ctx->player, &ctx->screen_size);
        printf("screen_width:%d,screen_height:%d\n",ctx->screen_size.width,ctx->screen_size.height);
#if 0
        ctx->disp_rect.x = 324;
        ctx->disp_rect.y = 20;
        ctx->disp_rect.width = 600;
        ctx->disp_rect.height = 450;

        ret = aic_player_set_disp_rect(ctx->player, &ctx->disp_rect);
        if(ret != 0) {
            printf("aic_player_set_disp_rect error\n");
            return -1;
        }
#endif
        ret =  aic_player_get_media_info(ctx->player,&ctx->media_info);
        if (ret != 0) {
            return -1;
        }
        ret = aic_player_play(ctx->player);
        if (ret != 0) {
            return -1;
        }
    }
    return 0;
}

static int lvgl_stop(struct lvgl_player_context *ctx)
{
    return aic_player_stop(ctx->player);
}

static int lvgl_pause(struct lvgl_player_context *ctx)
{
    return aic_player_pause(ctx->player);
}

void lv_video_play(void)
{
    struct lvgl_player_context *ctx = &g_lvgl_player_ctx;

    printf("%s, %d\n", __func__, __LINE__);

    lvgl_play(ctx);

    lv_imgbtn_set_src(g_btn_pause_play, LV_IMGBTN_STATE_RELEASED , NULL, &pause_norma_1, NULL);
    lv_imgbtn_set_src(g_btn_pause_play, LV_IMGBTN_STATE_PRESSED , NULL, &pause_press_1, NULL);
}

void lv_video_stop(void)
{
    struct lvgl_player_context *ctx = &g_lvgl_player_ctx;

    printf("%s, %d\n", __func__, __LINE__);

    lvgl_stop(ctx);
    if (ctx->player_state != LVGL_PLAYER_STATE_STOP) {
        ctx->player_state = LVGL_PLAYER_STATE_STOP;
        lv_imgbtn_set_src(g_btn_pause_play, LV_IMGBTN_STATE_RELEASED , NULL, &play_normal, NULL);
        lv_imgbtn_set_src(g_btn_pause_play, LV_IMGBTN_STATE_PRESSED , NULL, &play_press, NULL);
    }

    lv_imgbtn_set_src(g_btn_pause_play, LV_IMGBTN_STATE_RELEASED , NULL, &play_normal, NULL);
    lv_imgbtn_set_src(g_btn_pause_play, LV_IMGBTN_STATE_PRESSED , NULL, &play_press, NULL);
}

static int lvgl_play_pre(struct lvgl_player_context *ctx)
{
    ctx->file_index--;
    ctx->file_index = (ctx->file_index < 0) ? (ctx->file_cnt - 1) : ctx->file_index;
    lvgl_stop(ctx);
    if (lvgl_play(ctx) != 0) {
        return -1;
    }
    return 0;
}

static int lvgl_play_next(struct lvgl_player_context *ctx)
{
    ctx->file_index++;
    ctx->file_index = (ctx->file_index > ctx->file_cnt - 1) ? 0 : ctx->file_index;
    lvgl_stop(ctx);
    if (lvgl_play(ctx) != 0) {
        return -1;
    }
    return 0;
}

static void btn_pause_play_event_cb(lv_event_t *e)
{
    lv_event_code_t code = (lv_event_code_t)lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_target(e);
    struct lvgl_player_context * ctx = (struct lvgl_player_context *)btn->user_data;
    if (code == LV_EVENT_CLICKED) {
        if (ctx->player_state == LVGL_PLAYER_STATE_STOP) {
            if (lvgl_play(ctx) != 0) {//if play fail ,it is considered play finsh.play the next one
                ctx->player_state = LVGL_PLAYER_STATE_STOP;
                ctx->player_end = 1;
                return;
            }
            ctx->player_end = 0;
            ctx->player_state = LVGL_PLAYER_STATE_PLAY;
            lv_imgbtn_set_src(g_btn_pause_play, LV_IMGBTN_STATE_RELEASED , NULL, &pause_norma_1, NULL);
            lv_imgbtn_set_src(g_btn_pause_play, LV_IMGBTN_STATE_PRESSED , NULL, &pause_press_1, NULL);
        } else if (ctx->player_state == LVGL_PLAYER_STATE_PLAY) {
            lvgl_pause(ctx);
            ctx->player_state = LVGL_PLAYER_STATE_PAUSE;
            lv_imgbtn_set_src(g_btn_pause_play, LV_IMGBTN_STATE_RELEASED , NULL, &play_normal, NULL);
            lv_imgbtn_set_src(g_btn_pause_play, LV_IMGBTN_STATE_PRESSED , NULL, &play_press, NULL);
        } else if (ctx->player_state == LVGL_PLAYER_STATE_PAUSE) {
            lvgl_pause(ctx);
            ctx->player_state = LVGL_PLAYER_STATE_PLAY;
            lv_imgbtn_set_src(g_btn_pause_play, LV_IMGBTN_STATE_RELEASED , NULL, &pause_norma_1, NULL);
            lv_imgbtn_set_src(g_btn_pause_play, LV_IMGBTN_STATE_PRESSED , NULL, &pause_press_1, NULL);
        }
    }
}

static void btn_next_event_cb(lv_event_t *e)
{
    lv_event_code_t code = (lv_event_code_t)lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_target(e);
    struct lvgl_player_context * ctx = (struct lvgl_player_context *)btn->user_data;

    if (code == LV_EVENT_CLICKED) {
        if (lvgl_play_next(ctx) != 0) {//if play fail ,it is considered play finsh.play the next one
            ctx->player_state =LVGL_PLAYER_STATE_STOP;
            ctx->player_end = 1;
        } else {
            ctx->player_state =LVGL_PLAYER_STATE_PLAY;
            ctx->player_end = 0;
            lv_imgbtn_set_src(g_btn_pause_play, LV_IMGBTN_STATE_RELEASED , NULL, &pause_norma_1, NULL);
            lv_imgbtn_set_src(g_btn_pause_play, LV_IMGBTN_STATE_PRESSED , NULL, &pause_press_1, NULL);

        }
    }
}

static void btn_pre_event_cb(lv_event_t *e)
{
    lv_event_code_t code = (lv_event_code_t)lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_target(e);
    struct lvgl_player_context * ctx = (struct lvgl_player_context *)btn->user_data;

    if (code == LV_EVENT_CLICKED) {
        if (lvgl_play_pre(ctx) != 0) {//if play fail ,it is considered play finsh.play the next one
            ctx->player_state =LVGL_PLAYER_STATE_STOP;
            ctx->player_end = 1;
        } else {
            ctx->player_state =LVGL_PLAYER_STATE_PLAY;
            ctx->player_end = 0;
            lv_imgbtn_set_src(g_btn_pause_play, LV_IMGBTN_STATE_RELEASED , NULL, &pause_norma_1, NULL);
            lv_imgbtn_set_src(g_btn_pause_play, LV_IMGBTN_STATE_PRESSED , NULL, &pause_press_1, NULL);
        }
    }
}

static void btn_show_hide_event_cb(lv_event_t *e)
{
    static int show = 1;
    lv_event_code_t code = (lv_event_code_t)lv_event_get_code(e);

    if (code == LV_EVENT_RELEASED) {
        printf("!!!!!btn_show_hide_event_cb!!!!!!!!!!!\n");
        if (show) {
            lv_obj_add_flag(g_btn_pre, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(g_btn_next, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(g_btn_pause_play, LV_OBJ_FLAG_HIDDEN);
            #ifdef FILE_LIST
            lv_obj_add_flag(g_file_list, LV_OBJ_FLAG_HIDDEN);
            #endif

            show = 0;
        } else {
            lv_obj_clear_flag(g_btn_pre, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(g_btn_next, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(g_btn_pause_play, LV_OBJ_FLAG_HIDDEN);
            #ifdef FILE_LIST
            lv_obj_clear_flag(g_file_list, LV_OBJ_FLAG_HIDDEN);
            #endif
            show = 1;
        }
    }
}

#ifdef FILE_LIST
static void file_list_btn_click_event_cb(lv_event_t * e)
{
    lv_obj_t * btn = lv_event_get_target(e);
    uint32_t idx = lv_obj_get_child_id(btn);
    lv_event_code_t code = (lv_event_code_t)lv_event_get_code(e);
    struct lvgl_player_context * ctx = (struct lvgl_player_context *)btn->user_data;

    if (code == LV_EVENT_CLICKED) {
        printf("idx=%u\n",idx);
        ctx->file_index = idx;
        lvgl_stop(ctx);
        if (lvgl_play(ctx) != 0) {
            ctx->player_end = 1;
            ctx->player_state = LVGL_PLAYER_STATE_STOP;
        } else {
            ctx->player_end = 0;
            ctx->player_state = LVGL_PLAYER_STATE_PLAY;
        }
    }
}

static lv_obj_t * add_file_list_btn(lv_obj_t * parent, uint32_t video_id)
{
    lv_obj_t * btn = lv_obj_create(parent);
    lv_obj_t * title_label = lv_label_create(btn);
    lv_obj_set_size(btn, lv_pct(95), 60);
    btn->user_data = &g_lvgl_player_ctx;
     lv_obj_add_style(btn, &g_file_list_btn_style_pre, LV_STATE_DEFAULT);
    lv_obj_add_style(btn, &g_file_list_btn_style_def, LV_STATE_PRESSED);
    lv_obj_add_style(btn, &g_file_list_btn_style_chk, LV_STATE_CHECKED);
    lv_obj_add_event_cb(btn, file_list_btn_click_event_cb, LV_EVENT_CLICKED, NULL);
    const char * title = (video_id >= sizeof(g_filename) / sizeof(g_filename[0]))?NULL:g_filename[video_id];
    lv_label_set_text(title_label, title);
    return btn;
}
#endif

static void create_player(lv_obj_t * parent)
{
#ifdef FILE_LIST
    int i = 0;
#endif
    static lv_style_t btn_style;
    lv_style_init(&btn_style);
    lv_style_set_radius(&btn_style, 0);
    lv_style_set_border_width(&btn_style, 2);
    //lv_style_set_border_color(&btn_style, lv_palette_main(LV_PALETTE_YELLOW));
    lv_style_set_bg_opa(&btn_style, LV_OPA_0);

    lv_memset_00(&g_lvgl_player_ctx, sizeof(struct lvgl_player_context));
    g_lvgl_player_ctx.player = aic_player_create(NULL);
    if (g_lvgl_player_ctx.player == NULL) {
        printf("aic_player_create fail!!!!\n");
        return;
    }

    g_lvgl_player_ctx.file_cnt =  sizeof(g_filename) / sizeof(g_filename[0]);
    g_lvgl_player_ctx.file_index = 0;
    g_lvgl_player_ctx.player_state = LVGL_PLAYER_STATE_STOP;
    aic_player_set_event_callback(g_lvgl_player_ctx.player, &g_lvgl_player_ctx, event_handle);

    g_btn_hide_show = lv_btn_create(parent);
    g_btn_hide_show->user_data = &g_lvgl_player_ctx;
    lv_obj_set_pos(g_btn_hide_show, 800, 40);
    lv_obj_set_size(g_btn_hide_show, 180, 520);
    lv_obj_add_event_cb(g_btn_hide_show,btn_show_hide_event_cb,LV_EVENT_ALL, NULL);
    lv_obj_add_style(g_btn_hide_show,&btn_style, 0);
    lv_obj_set_style_border_opa(g_btn_hide_show,LV_OPA_TRANSP, 0);
    lv_obj_set_style_shadow_opa(g_btn_hide_show, LV_OPA_TRANSP, 0);

    g_btn_pre = lv_imgbtn_create(parent);
    lv_imgbtn_set_src(g_btn_pre, LV_IMGBTN_STATE_RELEASED , NULL, &pre_normal, NULL);
    lv_imgbtn_set_src(g_btn_pre, LV_IMGBTN_STATE_PRESSED , NULL, &pre_press, NULL);
    g_btn_pre->user_data = &g_lvgl_player_ctx;
    lv_obj_set_pos(g_btn_pre, 200, 450);
    lv_obj_set_size(g_btn_pre, 64, 64);
    lv_obj_add_event_cb(g_btn_pre,btn_pre_event_cb,LV_EVENT_ALL, NULL);
    lv_obj_add_style(g_btn_pre,&btn_style, 0);
    lv_obj_set_style_border_opa(g_btn_pre,LV_OPA_TRANSP, 0);
    lv_obj_set_style_shadow_opa(g_btn_pre, LV_OPA_TRANSP, 0);

    lv_obj_add_flag(g_btn_pre, LV_OBJ_FLAG_HIDDEN); // FIXME

    g_btn_pause_play = lv_imgbtn_create(parent);
    lv_imgbtn_set_src(g_btn_pause_play, LV_IMGBTN_STATE_RELEASED , NULL, &play_normal, NULL);
    lv_imgbtn_set_src(g_btn_pause_play, LV_IMGBTN_STATE_PRESSED , NULL, &play_press, NULL);
    g_btn_pause_play->user_data = &g_lvgl_player_ctx;
    lv_obj_set_pos(g_btn_pause_play, 450, 450);
    lv_obj_set_size(g_btn_pause_play, 64, 64);
    lv_obj_add_event_cb(g_btn_pause_play,btn_pause_play_event_cb,LV_EVENT_ALL, NULL);
    lv_obj_add_style(g_btn_pause_play,&btn_style, 0);
    lv_obj_set_style_border_opa(g_btn_pause_play,LV_OPA_TRANSP, 0);
    lv_obj_set_style_shadow_opa(g_btn_pause_play, LV_OPA_TRANSP, 0);

    lv_obj_add_flag(g_btn_pause_play, LV_OBJ_FLAG_HIDDEN); // FIXME

    g_btn_next = lv_imgbtn_create(parent);
    lv_imgbtn_set_src(g_btn_next, LV_IMGBTN_STATE_RELEASED , NULL, &next_normal, NULL);
    lv_imgbtn_set_src(g_btn_next, LV_IMGBTN_STATE_PRESSED , NULL, &next_press, NULL);
    g_btn_next->user_data = &g_lvgl_player_ctx;
    lv_obj_set_pos(g_btn_next, 700, 450);
    lv_obj_set_size(g_btn_next, 64, 64);
    lv_obj_add_event_cb(g_btn_next,btn_next_event_cb,LV_EVENT_ALL, NULL);
    lv_obj_add_style(g_btn_next,&btn_style, 0);
    lv_obj_set_style_border_opa(g_btn_next,LV_OPA_TRANSP, 0);
    lv_obj_set_style_shadow_opa(g_btn_next, LV_OPA_TRANSP, 0);

    lv_obj_add_flag(g_btn_next, LV_OBJ_FLAG_HIDDEN); // FIXME

#ifdef FILE_LIST
    lv_style_init(&g_style_scrollbar);
    lv_style_set_width(&g_style_scrollbar,  4);
    lv_style_set_bg_opa(&g_style_scrollbar, LV_OPA_COVER);
    lv_style_set_bg_color(&g_style_scrollbar, lv_color_hex3(0xeee));
    lv_style_set_radius(&g_style_scrollbar, LV_RADIUS_CIRCLE);
    lv_style_set_pad_right(&g_style_scrollbar, 4);

    g_file_list = lv_obj_create(parent);
    lv_obj_remove_style_all(g_file_list);
    lv_obj_set_size(g_file_list, 300, 450);
    lv_obj_set_y(g_file_list, 40);
    lv_obj_add_style(g_file_list, &g_style_scrollbar, LV_PART_SCROLLBAR);
    lv_obj_set_flex_flow(g_file_list, LV_FLEX_FLOW_COLUMN);

    lv_style_init(&g_file_list_btn_style_def);
    lv_style_set_bg_opa(&g_file_list_btn_style_def, LV_OPA_TRANSP);
    lv_style_set_radius(&g_file_list_btn_style_def, 0);

    lv_style_init(&g_file_list_btn_style_pre);
    lv_style_set_bg_opa(&g_file_list_btn_style_pre, LV_OPA_COVER);
    lv_style_set_bg_color(&g_file_list_btn_style_pre,  lv_color_hex(0x4c4965));
    lv_style_set_radius(&g_file_list_btn_style_pre, 0);

    lv_style_init(&g_file_list_btn_style_chk);
    lv_style_set_bg_opa(&g_file_list_btn_style_chk, LV_OPA_COVER);
    lv_style_set_bg_color(&g_file_list_btn_style_chk, lv_color_hex(0x4c4965));
    lv_style_set_radius(&g_file_list_btn_style_chk, 0);

    for (i = 0;i < sizeof(g_filename) / sizeof(g_filename[0]); i++) {
        add_file_list_btn(g_file_list,i);
    }
#endif
}

static void player_callback(lv_timer_t *tmr)
{
    struct lvgl_player_context *ctx;

    (void)tmr;

    ctx = &g_lvgl_player_ctx;
    if (ctx->player_end && ctx->player_state != LVGL_PLAYER_STATE_STOP) {
        ctx->player_end = 0;
        lvgl_stop(ctx);
        ctx->player_state = LVGL_PLAYER_STATE_STOP;
        lv_imgbtn_set_src(g_btn_pause_play, LV_IMGBTN_STATE_RELEASED , NULL, &play_normal, NULL);
        lv_imgbtn_set_src(g_btn_pause_play, LV_IMGBTN_STATE_PRESSED , NULL, &play_press, NULL);
    }

    return;
}
#endif

static void lv_bg_image_creat(lv_obj_t * parent)
{
    struct mpp_fb * fb = mpp_fb_open();
    struct aicfb_screeninfo info;
    char bg_dark[256];

    mpp_fb_ioctl(fb, AICFB_GET_SCREENINFO, &info);
    snprintf(bg_dark, 255, "L:/%dx%d_%d_%08x.fake",\
                 info.width, info.height, 0, 0x00000000);

    lv_obj_t * img_bg = lv_img_create(parent);
    lv_img_set_src(img_bg, bg_dark);
    lv_obj_set_pos(img_bg, 0, 0);
    mpp_fb_close(fb);
}

lv_obj_t * lv_video_screen_creat(void)
{
    lv_obj_t * video_src = lv_obj_create(NULL);
    lv_obj_clear_flag(video_src, LV_OBJ_FLAG_SCROLLABLE);

    lv_bg_image_creat(video_src);

    static lv_style_t style_pr;
    lv_style_init(&style_pr);
    lv_style_set_translate_x(&style_pr, 2);
    lv_style_set_translate_y(&style_pr, 2);

    // video back btn
    lv_obj_t *back_btn = lv_imgbtn_create(video_src);
    lv_imgbtn_set_src(back_btn, LV_IMGBTN_STATE_RELEASED , NULL, LVGL_PATH(Back.png), NULL);
    lv_imgbtn_set_src(back_btn, LV_IMGBTN_STATE_PRESSED , NULL, LVGL_PATH(Back.png), NULL);
    //lv_obj_add_event_cb(back_btn, video_back_event_cb, LV_EVENT_ALL, NULL);

    lv_obj_set_pos(back_btn, 10, 10);
    lv_obj_set_size(back_btn, 32, 32);
    lv_obj_add_style(back_btn, &style_pr, LV_STATE_PRESSED);
    lv_obj_add_flag(back_btn, LV_OBJ_FLAG_HIDDEN); // FIXME

#ifdef VIDEO_PLAYER
    fd_dev = mpp_fb_open();
    create_player(video_src);
    lv_timer_create(player_callback, 20, 0);
#endif

    return video_src;
}
