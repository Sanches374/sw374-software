/*******************************************************************************
 * Size: 6 px
 * Bpp: 4
 * Opts: --bpp 4 --size 6 --no-compress --font Formula1-Regular_web_0.ttf --symbols !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~  --format lvgl -o F1_Display_Regular_6px_4bpp.c
 ******************************************************************************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#ifndef F1_DISPLAY_REGULAR_6PX_4BPP
#define F1_DISPLAY_REGULAR_6PX_4BPP 1
#endif

#if F1_DISPLAY_REGULAR_6PX_4BPP

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */

    /* U+0021 "!" */
    0x92, 0x82, 0x81, 0x40, 0x72,

    /* U+0022 "\"" */
    0x98, 0x37, 0x51,

    /* U+0023 "#" */
    0x1, 0x89, 0x3, 0xbb, 0xd6, 0x8, 0x29, 0x6,
    0xdb, 0xb2, 0x9, 0x81, 0x0,

    /* U+0024 "$" */
    0x0, 0x20, 0x6, 0xbd, 0x90, 0xa4, 0x80, 0x1,
    0x9e, 0x50, 0x1, 0x8b, 0x6, 0xad, 0xa0, 0x1,
    0x80, 0x0,

    /* U+0025 "%" */
    0x8a, 0x45, 0x5a, 0x18, 0xa0, 0x5e, 0xaf, 0x70,
    0xa6, 0x4a, 0x28, 0x3a, 0x90,

    /* U+0026 "&" */
    0x6, 0xbb, 0x20, 0x8, 0x40, 0x0, 0x4a, 0xd2,
    0x90, 0xa0, 0x1f, 0x30, 0x7b, 0xb8, 0x90,

    /* U+0027 "'" */
    0x91, 0x70,

    /* U+0028 "(" */
    0x49, 0x73, 0x73, 0x73, 0x73, 0x49,

    /* U+0029 ")" */
    0x67, 0xa, 0xa, 0xa, 0xa, 0x67,

    /* U+002A "*" */
    0x0, 0x2, 0x84, 0x5e, 0x71, 0x22,

    /* U+002B "+" */
    0x9, 0x7, 0xda, 0x8, 0x0,

    /* U+002C "," */
    0x72, 0x90,

    /* U+002D "-" */
    0x77,

    /* U+002E "." */
    0x72,

    /* U+002F "/" */
    0xa, 0x19, 0x45, 0x82, 0xa0, 0x50,

    /* U+0030 "0" */
    0x2a, 0xaa, 0x9, 0x20, 0x55, 0xa1, 0x4, 0x79,
    0x20, 0x55, 0x2a, 0xaa, 0x0,

    /* U+0031 "1" */
    0x6c, 0x30, 0x6, 0x50, 0x6, 0x50, 0x6, 0x50,
    0x7c, 0xc5,

    /* U+0032 "2" */
    0x6a, 0xab, 0x0, 0x1, 0xc0, 0x6, 0xb2, 0x6,
    0x70, 0x0, 0xaa, 0xaa, 0x0,

    /* U+0033 "3" */
    0x8a, 0xdb, 0x4, 0xa0, 0x1c, 0xa7, 0x0, 0xb,
    0x7a, 0xa9,

    /* U+0034 "4" */
    0x0, 0xaa, 0x0, 0x93, 0xb0, 0x75, 0xb, 0x8,
    0xaa, 0xe6, 0x0, 0xb, 0x0,

    /* U+0035 "5" */
    0x9b, 0xaa, 0x92, 0x0, 0x7a, 0xa8, 0x0, 0xb,
    0x5a, 0xa9,

    /* U+0036 "6" */
    0x2a, 0xaa, 0x9, 0x10, 0x0, 0xaa, 0xaa, 0x9,
    0x0, 0x73, 0x3a, 0xab, 0x0,

    /* U+0037 "7" */
    0x8a, 0xab, 0x0, 0x29, 0x0, 0xa1, 0x3, 0x80,
    0xb, 0x0,

    /* U+0038 "8" */
    0x4b, 0xab, 0x8, 0x30, 0xa0, 0x3c, 0xaa, 0xa,
    0x0, 0x92, 0x5b, 0xab, 0x0,

    /* U+0039 "9" */
    0x7a, 0xa8, 0xa, 0x0, 0x91, 0x6a, 0xad, 0x10,
    0x0, 0xa0, 0x6a, 0xa7, 0x0,

    /* U+003A ":" */
    0x72, 0x0, 0x0, 0x72,

    /* U+003B ";" */
    0x72, 0x0, 0x0, 0x72, 0x91,

    /* U+003C "<" */
    0x0, 0x0, 0x82, 0x77, 0x0, 0x92, 0x0, 0x0,

    /* U+003D "=" */
    0x7a, 0x77, 0xa7,

    /* U+003E ">" */
    0x0, 0x5, 0x50, 0xa, 0x35, 0x60, 0x0, 0x0,

    /* U+003F "?" */
    0x7a, 0xb2, 0x0, 0x46, 0xa, 0xa1, 0x0, 0x0,
    0x9, 0x0,

    /* U+0040 "@" */
    0x5a, 0xaa, 0x20, 0x0, 0xa, 0x2a, 0xa5, 0xa8,
    0x23, 0x7a, 0x4a, 0xac, 0xc0,

    /* U+0041 "A" */
    0x1, 0xc3, 0x0, 0x8, 0x3a, 0x0, 0xb, 0xa,
    0x10, 0x5d, 0xbc, 0x70, 0xb1, 0x0, 0xb0,

    /* U+0042 "B" */
    0x7b, 0xab, 0x17, 0x30, 0x83, 0x7b, 0xac, 0x7,
    0x30, 0x64, 0x7b, 0xab, 0x20,

    /* U+0043 "C" */
    0x2a, 0xaa, 0x29, 0x20, 0x0, 0xa1, 0x0, 0x9,
    0x20, 0x0, 0x2b, 0xaa, 0x30,

    /* U+0044 "D" */
    0x7b, 0xaa, 0x27, 0x30, 0x29, 0x73, 0x1, 0xa7,
    0x30, 0x29, 0x7b, 0xaa, 0x20,

    /* U+0045 "E" */
    0x7b, 0xaa, 0x17, 0x30, 0x0, 0x7b, 0xaa, 0x17,
    0x30, 0x0, 0x7b, 0xaa, 0x10,

    /* U+0046 "F" */
    0x7b, 0xaa, 0x17, 0x30, 0x0, 0x7b, 0xaa, 0x17,
    0x30, 0x0, 0x73, 0x0, 0x0,

    /* U+0047 "G" */
    0x2a, 0xaa, 0x49, 0x20, 0x0, 0xa1, 0x4a, 0x89,
    0x20, 0xa, 0x2a, 0xab, 0x50,

    /* U+0048 "H" */
    0x73, 0x0, 0xb7, 0x30, 0xb, 0x7c, 0xbb, 0xb7,
    0x30, 0xb, 0x73, 0x0, 0xb0,

    /* U+0049 "I" */
    0x64, 0x64, 0x64, 0x64, 0x64,

    /* U+004A "J" */
    0x0, 0x73, 0x0, 0x73, 0x0, 0x73, 0x0, 0x83,
    0x7b, 0xa0,

    /* U+004B "K" */
    0x73, 0xa, 0x37, 0x37, 0x50, 0x7d, 0xc0, 0x7,
    0x37, 0x70, 0x73, 0xa, 0x40,

    /* U+004C "L" */
    0x73, 0x0, 0x73, 0x0, 0x73, 0x0, 0x73, 0x0,
    0x7b, 0xa9,

    /* U+004D "M" */
    0xc, 0x50, 0x89, 0x2, 0x8a, 0xa, 0xa0, 0x55,
    0xa1, 0x98, 0x27, 0x38, 0x65, 0x65, 0xa0, 0x3c,
    0x13, 0x70,

    /* U+004E "N" */
    0x4d, 0x20, 0x64, 0x75, 0xa0, 0x64, 0x73, 0x74,
    0x64, 0x73, 0xb, 0x64, 0x73, 0x4, 0xd2,

    /* U+004F "O" */
    0x2a, 0xaa, 0x49, 0x20, 0xb, 0xa1, 0x0, 0xb9,
    0x20, 0xb, 0x2a, 0xaa, 0x40,

    /* U+0050 "P" */
    0x7b, 0xab, 0x27, 0x30, 0x46, 0x7b, 0xaa, 0x17,
    0x30, 0x0, 0x73, 0x0, 0x0,

    /* U+0051 "Q" */
    0x2a, 0xaa, 0x49, 0x20, 0xb, 0xa1, 0x0, 0xb9,
    0x20, 0xb, 0x2a, 0xab, 0x40, 0x3, 0x90,

    /* U+0052 "R" */
    0x7b, 0xab, 0x37, 0x30, 0x37, 0x76, 0xca, 0x27,
    0x3a, 0x30, 0x73, 0xb, 0x10,

    /* U+0053 "S" */
    0x6a, 0xa9, 0xa, 0x30, 0x0, 0x19, 0xb5, 0x0,
    0x0, 0xb0, 0x6a, 0xaa, 0x0,

    /* U+0054 "T" */
    0x9b, 0xda, 0x30, 0x19, 0x0, 0x1, 0x90, 0x0,
    0x19, 0x0, 0x1, 0x90, 0x0,

    /* U+0055 "U" */
    0x73, 0x0, 0xb7, 0x30, 0xb, 0x73, 0x0, 0xb6,
    0x40, 0xa, 0x2b, 0xab, 0x50,

    /* U+0056 "V" */
    0xa2, 0x3, 0x85, 0x70, 0x92, 0xb, 0xb, 0x0,
    0xa5, 0x70, 0x3, 0xd1, 0x0,

    /* U+0057 "W" */
    0xa1, 0xd, 0x40, 0xa1, 0x65, 0x38, 0x90, 0xb0,
    0x29, 0x64, 0xb2, 0x80, 0xb, 0xa0, 0xa7, 0x40,
    0x8, 0xa0, 0x5c, 0x0,

    /* U+0058 "X" */
    0x93, 0x6, 0x60, 0xb1, 0xb0, 0x7, 0xe4, 0x0,
    0xb1, 0xb0, 0x84, 0x6, 0x70,

    /* U+0059 "Y" */
    0xa2, 0x7, 0x51, 0xa1, 0xa0, 0x7, 0xc2, 0x0,
    0xb, 0x0, 0x0, 0xb0, 0x0,

    /* U+005A "Z" */
    0x6a, 0xab, 0x0, 0x4a, 0x6, 0x90, 0x67, 0x0,
    0x9a, 0xa9,

    /* U+005B "[" */
    0x79, 0x73, 0x73, 0x73, 0x73, 0x79,

    /* U+005C "\\" */
    0xa0, 0x90, 0x64, 0x27, 0xa, 0x5,

    /* U+005D "]" */
    0x6a, 0xa, 0xa, 0xa, 0xa, 0x6a,

    /* U+005E "^" */
    0x13, 0x8, 0x90,

    /* U+005F "_" */
    0xaa, 0x30,

    /* U+0060 "`" */
    0x0, 0x63, 0x1,

    /* U+0061 "a" */
    0x6a, 0xc4, 0x6, 0x99, 0x66, 0xa, 0x7a, 0xaa,

    /* U+0062 "b" */
    0x82, 0x0, 0x8a, 0xa7, 0x82, 0xa, 0x82, 0xa,
    0x8a, 0xa8,

    /* U+0063 "c" */
    0x3b, 0xa5, 0x91, 0x0, 0x91, 0x0, 0x3b, 0xa5,

    /* U+0064 "d" */
    0x0, 0xa, 0x3b, 0xac, 0x91, 0xa, 0x91, 0xa,
    0x3b, 0xac,

    /* U+0065 "e" */
    0x3a, 0x98, 0x9a, 0x9a, 0x91, 0x0, 0x3b, 0x97,

    /* U+0066 "f" */
    0x2a, 0x6b, 0xb6, 0x55, 0x5, 0x50, 0x55, 0x0,

    /* U+0067 "g" */
    0x4b, 0xab, 0xa0, 0xa, 0x91, 0xa, 0x2a, 0xab,
    0x4a, 0x96,

    /* U+0068 "h" */
    0x82, 0x0, 0x8b, 0xa7, 0x82, 0xa, 0x82, 0xa,
    0x82, 0xa,

    /* U+0069 "i" */
    0x42, 0x73, 0x73, 0x73, 0x73,

    /* U+006A "j" */
    0x4, 0x20, 0x64, 0x6, 0x40, 0x64, 0x6, 0x41,
    0xb2,

    /* U+006B "k" */
    0x82, 0x0, 0x8, 0x21, 0xa0, 0x82, 0xa1, 0x8,
    0xb9, 0x0, 0x82, 0x57, 0x0,

    /* U+006C "l" */
    0x73, 0x73, 0x73, 0x73, 0x73,

    /* U+006D "m" */
    0x8b, 0xbd, 0xa9, 0x82, 0x28, 0xa, 0x82, 0x28,
    0xa, 0x82, 0x28, 0xa,

    /* U+006E "n" */
    0x8b, 0xa7, 0x82, 0xa, 0x82, 0xa, 0x82, 0xa,

    /* U+006F "o" */
    0x3b, 0x98, 0x9, 0x10, 0x90, 0x91, 0xa, 0x3,
    0xb9, 0x80,

    /* U+0070 "p" */
    0x7b, 0xa8, 0x7, 0x30, 0xa0, 0x73, 0xa, 0x7,
    0xba, 0x80, 0x73, 0x0, 0x0,

    /* U+0071 "q" */
    0x3b, 0xac, 0x91, 0xa, 0x91, 0xa, 0x3b, 0xac,
    0x0, 0xa,

    /* U+0072 "r" */
    0x4b, 0x78, 0x20, 0x82, 0x8, 0x20,

    /* U+0073 "s" */
    0x7a, 0xa4, 0x97, 0x30, 0x3, 0x96, 0x6a, 0xa4,

    /* U+0074 "t" */
    0x22, 0xb, 0xb6, 0x55, 0x5, 0x50, 0x2b, 0x60,

    /* U+0075 "u" */
    0x82, 0xa, 0x82, 0xa, 0x82, 0xa, 0x3b, 0xac,

    /* U+0076 "v" */
    0xa1, 0xa, 0x4, 0x71, 0x90, 0xa, 0x73, 0x0,
    0x6a, 0x0,

    /* U+0077 "w" */
    0xb0, 0x88, 0xb, 0x73, 0xa9, 0x18, 0x37, 0x87,
    0x74, 0xc, 0x33, 0xd0,

    /* U+0078 "x" */
    0x93, 0x1b, 0x1, 0xa8, 0x20, 0xa, 0xc0, 0x6,
    0x64, 0x80,

    /* U+0079 "y" */
    0xb0, 0xb, 0x56, 0x46, 0xb, 0x91, 0x6, 0xa0,
    0x6a, 0x30,

    /* U+007A "z" */
    0x6a, 0xb5, 0x2, 0xb1, 0x4a, 0x0, 0x9a, 0xa5,

    /* U+007B "{" */
    0x1b, 0x3, 0x60, 0xa5, 0x3, 0x50, 0x36, 0x1,
    0xb0,

    /* U+007C "|" */
    0x64, 0x64, 0x64, 0x64, 0x64, 0x42,

    /* U+007D "}" */
    0x94, 0x36, 0x1c, 0x36, 0x36, 0xa3,

    /* U+007E "~" */
    0x46, 0x66, 0x45
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 25, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 0, .adv_w = 26, .box_w = 2, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 5, .adv_w = 43, .box_w = 3, .box_h = 2, .ofs_x = 0, .ofs_y = 3},
    {.bitmap_index = 8, .adv_w = 80, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 21, .adv_w = 69, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 39, .adv_w = 82, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 52, .adv_w = 82, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 67, .adv_w = 25, .box_w = 2, .box_h = 2, .ofs_x = 0, .ofs_y = 3},
    {.bitmap_index = 69, .adv_w = 35, .box_w = 2, .box_h = 6, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 75, .adv_w = 35, .box_w = 2, .box_h = 6, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 81, .adv_w = 50, .box_w = 3, .box_h = 4, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 87, .adv_w = 52, .box_w = 3, .box_h = 3, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 92, .adv_w = 26, .box_w = 2, .box_h = 2, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 94, .adv_w = 32, .box_w = 2, .box_h = 1, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 95, .adv_w = 25, .box_w = 2, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 96, .adv_w = 32, .box_w = 2, .box_h = 6, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 102, .adv_w = 77, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 115, .adv_w = 60, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 125, .adv_w = 70, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 138, .adv_w = 68, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 148, .adv_w = 77, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 161, .adv_w = 69, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 171, .adv_w = 71, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 184, .adv_w = 66, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 194, .adv_w = 72, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 207, .adv_w = 71, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 220, .adv_w = 25, .box_w = 2, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 224, .adv_w = 27, .box_w = 2, .box_h = 5, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 229, .adv_w = 44, .box_w = 3, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 237, .adv_w = 49, .box_w = 3, .box_h = 2, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 240, .adv_w = 44, .box_w = 3, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 248, .adv_w = 60, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 258, .adv_w = 86, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 271, .adv_w = 83, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 286, .adv_w = 75, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 299, .adv_w = 73, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 312, .adv_w = 78, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 325, .adv_w = 73, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 338, .adv_w = 71, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 351, .adv_w = 80, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 364, .adv_w = 85, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 377, .adv_w = 30, .box_w = 2, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 382, .adv_w = 60, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 392, .adv_w = 75, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 405, .adv_w = 64, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 415, .adv_w = 110, .box_w = 7, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 433, .adv_w = 93, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 448, .adv_w = 84, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 461, .adv_w = 75, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 474, .adv_w = 82, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 489, .adv_w = 77, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 502, .adv_w = 69, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 515, .adv_w = 72, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 528, .adv_w = 84, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 541, .adv_w = 78, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 554, .adv_w = 118, .box_w = 8, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 574, .adv_w = 78, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 587, .adv_w = 75, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 600, .adv_w = 68, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 610, .adv_w = 35, .box_w = 2, .box_h = 6, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 616, .adv_w = 32, .box_w = 2, .box_h = 6, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 622, .adv_w = 35, .box_w = 2, .box_h = 6, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 628, .adv_w = 68, .box_w = 3, .box_h = 2, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 631, .adv_w = 37, .box_w = 3, .box_h = 1, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 633, .adv_w = 68, .box_w = 2, .box_h = 3, .ofs_x = 1, .ofs_y = 4},
    {.bitmap_index = 636, .adv_w = 66, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 644, .adv_w = 70, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 654, .adv_w = 62, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 662, .adv_w = 70, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 672, .adv_w = 68, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 680, .adv_w = 45, .box_w = 3, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 688, .adv_w = 68, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 698, .adv_w = 69, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 708, .adv_w = 28, .box_w = 2, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 713, .adv_w = 28, .box_w = 3, .box_h = 6, .ofs_x = -1, .ofs_y = -1},
    {.bitmap_index = 722, .adv_w = 67, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 735, .adv_w = 28, .box_w = 2, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 740, .adv_w = 103, .box_w = 6, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 752, .adv_w = 69, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 760, .adv_w = 72, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 770, .adv_w = 71, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 783, .adv_w = 70, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 793, .adv_w = 46, .box_w = 3, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 799, .adv_w = 60, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 807, .adv_w = 47, .box_w = 3, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 815, .adv_w = 69, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 823, .adv_w = 69, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 833, .adv_w = 97, .box_w = 6, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 845, .adv_w = 66, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 855, .adv_w = 65, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 865, .adv_w = 62, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 873, .adv_w = 35, .box_w = 3, .box_h = 6, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 882, .adv_w = 30, .box_w = 2, .box_h = 6, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 888, .adv_w = 35, .box_w = 2, .box_h = 6, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 894, .adv_w = 83, .box_w = 3, .box_h = 2, .ofs_x = 1, .ofs_y = 2}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/



/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 32, .range_length = 95, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    }
};

/*-----------------
 *    KERNING
 *----------------*/


/*Map glyph_ids to kern left classes*/
static const uint8_t kern_left_class_mapping[] =
{
    0, 0, 0, 1, 0, 0, 0, 0,
    1, 2, 0, 1, 3, 4, 3, 4,
    5, 6, 7, 8, 9, 10, 11, 12,
    13, 9, 6, 14, 14, 0, 3, 15,
    0, 16, 17, 9, 18, 6, 19, 20,
    21, 0, 0, 22, 23, 24, 25, 26,
    6, 27, 6, 28, 29, 30, 31, 32,
    33, 34, 35, 36, 2, 37, 0, 0,
    4, 0, 38, 39, 40, 0, 0, 41,
    0, 38, 0, 0, 42, 0, 38, 38,
    39, 39, 0, 43, 44, 45, 0, 46,
    47, 48, 49, 50, 2, 0, 0, 0
};

/*Map glyph_ids to kern right classes*/
static const uint8_t kern_right_class_mapping[] =
{
    0, 0, 0, 1, 0, 0, 0, 2,
    1, 0, 3, 1, 4, 5, 4, 5,
    6, 7, 8, 9, 10, 11, 12, 7,
    13, 14, 15, 16, 16, 17, 4, 0,
    0, 0, 18, 0, 7, 0, 0, 0,
    7, 0, 0, 19, 0, 0, 20, 21,
    7, 0, 7, 0, 22, 23, 24, 25,
    26, 27, 28, 29, 0, 30, 3, 0,
    5, 0, 31, 0, 32, 32, 32, 33,
    34, 0, 0, 0, 0, 0, 35, 35,
    32, 35, 32, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 0, 0, 3, 0
};

/*Kern values between classes*/
static const int8_t kern_class_values[] =
{
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -7, 0, 0, 0, 0, 0,
    0, -9, -14, -3, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -3, 0,
    0, -2, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -2, 0, 0, 0, 0,
    0, 0, -2, -2, -2, 0, 0, -2,
    0, 0, 0, 0, 0, 0, 0, -6,
    -2, -2, 0, 0, -6, 0, 0, 0,
    0, -2, -2, 0, 0, 0, -6, 0,
    -4, -3, -2, -8, -2, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -4,
    -2, -3, -2, -2, 0, 0, 0, 0,
    0, 0, -3, 0, 0, 0, -6, -2,
    -8, -3, -2, 0, 0, 0, 0, 0,
    0, 0, -9, 0, -12, -9, 0, -14,
    0, 0, 0, -2, 0, 0, 0, 0,
    0, 0, 0, -10, -4, 0, -3, 0,
    0, 0, 0, -4, 0, 0, -2, 0,
    0, 0, -2, 0, 0, -2, 0, 0,
    0, -4, -4, -3, -2, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -1,
    0, -1, 0, -2, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -1, 0,
    -3, -2, 0, 0, -4, -1, 0, 0,
    -6, 0, 0, 0, 0, -2, -4, 0,
    0, 0, -2, 0, -2, -2, -2, -4,
    -1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -1, 0, -1, 0, -1,
    -10, 0, 0, -3, 0, 0, -3, 0,
    0, 0, -5, 0, -7, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -9, 0,
    -9, -7, 0, -10, 0, -7, 0, 0,
    0, 0, 0, 0, 0, 0, -1, 0,
    -5, 0, 0, 0, 0, 0, 0, -2,
    -1, 0, -3, 0, 0, 0, -5, 0,
    -2, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -2, 0, -3, -2, 0, 0,
    0, 0, 0, 0, -4, 0, 0, -2,
    0, -1, 0, 0, 0, 0, -2, 0,
    -4, -4, -2, -4, 0, -1, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -2, -1, 0, -4, 0, 0, 0,
    -2, 0, 0, 0, 0, -2, 0, 0,
    -7, 0, -2, -2, 0, 0, -2, 0,
    0, 0, -4, 0, -4, -4, 0, -4,
    0, -2, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -2, 0, 0, 0,
    0, 0, 0, 0, -2, -2, 0, 0,
    0, 0, 0, 0, -3, 0, 0, -2,
    0, 0, -2, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -6, -16, -5, -2, 0,
    -2, 0, -9, 0, 0, -2, 0, 0,
    0, -12, -10, -5, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -4,
    0, -4, 0, -3, -2, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -2, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -2,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -2, 0, 0, 0, 0, -7, 0,
    0, -5, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -3, 0, -3, -2, -2, -2,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -9, 0, 0, -2, 0, 0, -2, 0,
    0, 0, -3, 0, -6, -1, 0, 0,
    -2, 0, 0, 0, 0, 0, -11, -3,
    -11, -10, 0, -13, 0, -6, 0, -1,
    -1, -1, 0, 0, 0, -2, -1, -7,
    -5, 0, -2, 0, 0, 0, 0, -3,
    0, 0, -3, 0, 0, 0, -5, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -1, 0, -2, 0, 0,
    0, 0, 0, -4, -4, 0, -2, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -3,
    -2, -2, -1, 0, 0, -2, 0, 0,
    -12, -3, -2, 0, 0, 0, -2, 0,
    -4, 0, 0, 0, 0, -9, -24, 0,
    -4, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -2, -1, 0, -1, 0, -1,
    -1, 0, 0, 0, 0, -2, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -1, -1, 0, 0, 0, -2, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -2,
    0, -1, -2, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -3, 0, -1,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -4, 0,
    0, 0, -6, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -3,
    -2, -3, 0, 0, 0, 0, -4, -4,
    -4, 0, -3, -1, -18, 0, 0, -4,
    0, 0, -3, 0, 0, 0, -4, 0,
    -10, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -15, -3, -13, -12, 0, -17,
    0, -3, 0, -2, -2, -2, 0, 0,
    0, -3, -2, -12, -9, 0, -5, 0,
    -3, 0, 0, 0, 0, 0, -1, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -6, 0,
    -6, -6, 0, -8, 0, -2, 0, -1,
    0, 0, 0, 0, 0, 0, -1, 0,
    -2, 0, 0, 0, 0, 0, 0, 0,
    0, -2, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -1, -2, 0,
    0, 0, -2, 0, -3, -2, 0, -3,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -1, 0, 0,
    0, 0, 0, -4, -12, -7, 0, 0,
    0, 0, -4, 0, 0, -2, 0, 0,
    0, -10, -24, -5, 0, -1, 0, 0,
    -2, -2, -4, -3, -4, 0, 0, -3,
    0, -2, 0, -2, -1, 0, 0, 0,
    0, -2, 0, 0, 0, 0, 0, -2,
    0, 0, -1, 0, 0, 0, -3, 0,
    0, 0, 0, 0, 0, 0, -4, 0,
    0, 0, 0, 0, -2, -3, 0, -3,
    0, 0, 0, -1, 0, -2, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -1, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -1, -1, 0, 0, 0, -4, 0,
    0, 0, 0, 0, -1, 0, 0, 0,
    0, 0, 0, 0, -2, 0, 0, -3,
    -2, -1, -2, 0, 0, 0, 0, -7,
    -9, -6, -2, 0, 0, 0, -12, 0,
    0, -2, 0, 0, -7, -11, -17, -6,
    -2, -1, 0, 0, 0, 0, 0, 0,
    0, 0, -3, -8, 0, -7, -7, -8,
    -6, 0, -7, -5, -1, -2, -2, -6,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -3, -3, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -4,
    -12, -5, -2, 0, 0, 0, -5, 0,
    0, -3, 0, 0, 0, -11, -15, -6,
    -3, -1, 0, 0, 0, 0, 0, 0,
    -2, 0, -3, -6, -1, -6, -5, -5,
    -5, 0, -2, 0, 0, -3, 0, -4,
    0, 0, 0, -3, -9, -5, -2, 0,
    0, -2, -7, 0, 0, -2, 0, 0,
    -5, -10, -12, -6, -2, -2, 0, 0,
    0, 0, 0, 0, -1, 0, -3, -5,
    -1, -5, -3, -5, -4, 0, -2, 0,
    0, -2, 0, -3, 0, 0, 0, -2,
    0, 0, -2, 0, 0, 0, -2, 0,
    0, -2, 0, 0, 0, 0, 0, 0,
    -1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -3, 0, -4, 0, -1,
    0, 0, 0, -4, -2, 0, -1, 0,
    0, 0, 0, -8, -14, -6, -4, 0,
    0, 0, -10, 0, 0, -3, 0, -2,
    0, -12, -17, -8, -3, -2, 0, 0,
    0, 0, 0, 0, 0, 0, -3, -9,
    -2, -8, -5, -9, -7, 0, -4, -3,
    -2, -3, -2, -5, 0, 0, 0, -2,
    0, 0, -1, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -1, 0, 0, -1, 0, 0, -1,
    0, 0, 0, -2, 0, -2, 0, 0,
    -2, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -2, 0,
    0, -2, -2, 0, -5, -1, 0, 0,
    0, 0, 0, 0, 0, 0, -2, 0,
    -7, -5, 0, -6, 0, 0, 0, 0,
    0, -1, 0, 0, 0, 0, 0, -5,
    -2, 0, 0, 0, -2, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -8, 0, -7, -5, 0, -8,
    0, -2, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -4, -3, 0, -3, 0,
    -1, 0, -2, 0, -2, -1, 0, 0,
    0, 0, 0, 0, -4, 0, 0, 0,
    0, -1, -2, 0, 0, 0, -8, 0,
    -6, -4, -3, -8, -2, -2, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -1,
    -1, -2, -2, -2, 0, 0, 0, 0,
    0, 0, -1, 0, 0, 0, -3, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -2, 0, -1, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -3, 0, 0,
    0, 0, -4, 0, 0, 0, 0, 0,
    0, -6, -12, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -2,
    0, -1, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -4,
    0, 0, -2, 0, 0, 0, -4, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -4, 0, -4, -3, 0, -3,
    0, 0, 0, -4, 0, -3, 0, 0,
    -3, 0, 0, 0, 0, 0, 0, -2,
    0, 0, 0, 0, -9, 0, 0, 0,
    0, 0, -4, 0, 0, 0, 0, 0,
    0, -7, -12, 0, 0, 0, -4, 0,
    0, 0, 0, -2, 0, 0, 0, -2,
    2, -1, 0, 0, 0, 2, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -4, 0, -3, -4, 0, -4,
    -1, 0, 0, 0, 0, -1, 0, 0,
    -1, 0, 0, -1, 0, 0, 0, -1,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -2, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -2,
    0, -1, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -4,
    -10, -2, -1, 0, 0, 0, -2, 0,
    0, 0, 0, 0, 0, -5, -10, 0,
    0, 0, -5, 0, 0, 0, -4, 0,
    -4, 0, 0, -1, 0, -1, 0, -3,
    -3, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -2, -4, -2, 0, 0,
    0, 0, -3, 0, 0, 0, 0, 0,
    0, -5, -9, 0, 0, 0, -1, 0,
    0, 0, -2, 0, -2, 0, 0, -1,
    0, -1, 0, -1, -2, 0, 0, 0,
    0, 0, 0, -1, 0, 0, 0, -3,
    0, 0, -1, 0, 0, 0, -3, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -2, 0, -3, -2, 0, -3,
    0, 0, 0, -2, 0, -1, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -2, -6, -2, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -5, -5, 0, 0, 0, -2, 0,
    0, 0, -1, 0, -2, 0, 0, -2,
    0, -1, 0, -2, -1, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -2,
    0, 0, -1, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -6, 0, -7, -3, 0, -5,
    0, -3, 0, -3, 0, -2, 0, 0,
    -1, 0, 0, -2, -2, 0, -1, 0
};


/*Collect the kern class' data in one place*/
static const lv_font_fmt_txt_kern_classes_t kern_classes =
{
    .class_pair_values   = kern_class_values,
    .left_class_mapping  = kern_left_class_mapping,
    .right_class_mapping = kern_right_class_mapping,
    .left_class_cnt      = 50,
    .right_class_cnt     = 44,
};

/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LVGL_VERSION_MAJOR == 8
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
#endif

#if LVGL_VERSION_MAJOR >= 8
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = &kern_classes,
    .kern_scale = 16,
    .cmap_num = 1,
    .bpp = 4,
    .kern_classes = 1,
    .bitmap_format = 0,
#if LVGL_VERSION_MAJOR == 8
    .cache = &cache
#endif
};



/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LVGL_VERSION_MAJOR >= 8
const lv_font_t F1_Display_Regular_6px_4bpp = {
#else
lv_font_t F1_Display_Regular_6px_4bpp = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 8,          /*The maximum line height required by the font*/
    .base_line = 1,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = 0,
    .underline_thickness = 0,
#endif
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
#if LV_VERSION_CHECK(8, 2, 0) || LVGL_VERSION_MAJOR >= 9
    .fallback = NULL,
#endif
    .user_data = NULL,
};



#endif /*#if F1_DISPLAY_REGULAR_6PX_4BPP*/

