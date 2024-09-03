/*
 * Copyright (c) 2023-2024, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors:  senye Liang <senye.liang@artinchip.com>
 */
#include "usbd_core.h"
#include "usbd_hid.h"
#include <rtthread.h>
#include <rtdevice.h>

#define TOUCH_MAX_POINT_NUM 5
#define TOUCH_LOGICAL_MAXNUM 4096
#define TOUCH_LOGICAL_MINNUM 0
#define TOUCH_PHYSICAL_MINNUM 0
/*!< endpoint address */
#define HID_INT_EP          0x81
#define HID_INT_EP_SIZE     (TOUCH_MAX_POINT_NUM * 10 + 4)
#define HID_INT_EP_INTERVAL 10

#define USBD_VID           0x33C3
#define USBD_PID           0x6789
#define USBD_MAX_POWER     100
#define USBD_LANGID_STRING 1033

/*!< config descriptor size */
#define USB_HID_CONFIG_DESC_SIZ 34
/*!< report descriptor size */
#define HID_MOUSE_REPORT_DESC_SIZE (73 + 90 * TOUCH_MAX_POINT_NUM)

#define FINGER_REPORT_DESC(logical_max, physical_max, physical_min) \
    0x09, 0x22,                         /*USAGE (Finger) */         \
    0xa1, 0x02,                         /*COLLECTION (Logical) */   \
    0x09, 0x42,                         /*USAGE (Tip Switch)   */   \
    0x15, 0x00,                         /*LOGICAL_MINIMUM (0)  */   \
    0x25, 0x01,                         /*LOGICAL_MAXIMUM (1)  */   \
    0x75, 0x01,                         /*REPORT_SIZE (1)      */   \
    0x95, 0x01,                         /*REPORT_COUNT (1)     */   \
    0x81, 0x02,                         /*INPUT (Data,Var,Abs) */   \
    0x95, 0x07,                         /*REPORT_COUNT (7)     */   \
    0x81, 0x03,                         /*INPUT (Cnst,Ary,Abs) */   \
    0x75, 0x08,                         /*REPORT_SIZE (8)      */   \
    0x09, 0x51,                         /*USAGE (Contact Identifier) */\
    0x95, 0x01,                         /*REPORT_COUNT (1)     */   \
    0x81, 0x02,                         /*INPUT (Data,Var,Abs) */   \
    0x05, 0x01,                         /*USAGE_PAGE (Generic Desk..*/ \
    0x26, (uint8_t)logical_max, (uint8_t)(logical_max >> 8), /*LOGICAL_MAXIMUM (4095) */\
    0x75, 0x10,                         /*REPORT_SIZE (16)    */    \
    0x55, 0x0e,                         /*UNIT_EXPONENT (-2)      */\
    0x65, 0x11,                         /*UNIT(Inch,EngLinear)*/    \
    0x09, 0x30,                         /*USAGE (X)*/               \
    0x35, 0x00,                         /*PHYSICAL_MINIMUM (0)*/    \
    0x46, (uint8_t)physical_max, (uint8_t)(physical_max >> 8), /*PHYSICAL_MAXIMUM (1205)*/\
    0x95, 0x01,                         /*REPORT_COUNT (2) PS: T C devices*/\
    0x81, 0x02,                         /*INPUT (Data,Var,Abs)*/    \
    /*PHYSICAL_MAXIMUM (906)*/\
    0x46, (uint8_t)TOUCH_Y_PHYSICAL_MAXNUM, (uint8_t)(TOUCH_Y_PHYSICAL_MAXNUM >> 8), \
    0x09, 0x31,                         /*USAGE (Y)*/               \
    0x81, 0x02,                         /*INPUT (Data,Var,Abs)*/    \
    0x05, 0x0d,                         /*USAGE_PAGE (Digitizers)*/ \
    0x09, 0x48,                         /*USAGE (Width)*/           \
    0x09, 0x49,                         /*USAGE (Height)*/          \
    0x81, 0x02,                         /*INPUT (Data,Var,Abs)*/    \
    0x95, 0x01,                         /*REPORT_COUNT (1)  */      \
    0x55, 0x0C,                         /*UNIT_EXPONENT (-4)    */  \
    0x65, 0x12,                         /*UNIT (Radians,SIROtation)*/\
    0x35, 0x00,                         /*PHYSICAL_MINIMUM (0)*/    \
    0x47, 0x6f, 0xf5, 0x00, 0x00,       /*PHYSICAL_MAXIMUM (62831)*/\
    0x15, 0x00,                         /*LOGICAL_MINIMUM (0) */    \
    0x27, 0x6f, 0xf5, 0x00, 0x00,       /*LOGICAL_MAXIMUM (62831)*/ \
    0x09, 0x3f,                         /*USAGE (Azimuth[Orientation])*/\
    0x81, 0x02,                         /*INPUT (Data,Var,Abs)*/    \
    0xc0,                               /*END_COLLECTION */         \

/*!<global descriptor */
const uint8_t hid_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0x00, 0x00, 0x00, USBD_VID, USBD_PID, 0x0002, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(USB_HID_CONFIG_DESC_SIZ, 0x01, 0x01,
                                USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),

    /************** Descriptor of Joystick Touch interface ****************/
    /* 09 */
    0x09,                           /* bLength: Interface Descriptor size */
    USB_DESCRIPTOR_TYPE_INTERFACE,  /* bDescriptorType: Interface descriptor type */
    0x00,                           /* bInterfaceNumber: Number of Interface */
    0x00,                           /* bAlternateSetting: Alternate setting */
    0x01,                           /* bNumEndpoints */
    0x03,                           /* bInterfaceClass: HID */
    0x01,                           /* bInterfaceSubClass : 1=BOOT, 0=no boot */
    0x02,                           /* nInterfaceProtocol : 0=none, 1=keyboard, 2=touch */
    0,                              /* iInterface: Index of string descriptor */
    /******************** Descriptor of Joystick Touch HID ********************/
    /* 18 */
    0x09,                           /* bLength: HID Descriptor size */
    HID_DESCRIPTOR_TYPE_HID,        /* bDescriptorType: HID */
    0x11,                           /* bcdHID: HID Class Spec release number */
    0x01,
    0x00,                           /* bCountryCode: Hardware target country */
    0x01,                           /* bNumDescriptors: Number of HID class descriptors to follow */
    0x22,                           /* bDescriptorType */
    (uint8_t)HID_MOUSE_REPORT_DESC_SIZE,     /* wItemLength: Total length of Report descriptor */
    (uint8_t)(HID_MOUSE_REPORT_DESC_SIZE >> 8),
    /******************** Descriptor of Touch endpoint ********************/
    /* 27 */
    0x07,                           /* bLength: Endpoint Descriptor size */
    USB_DESCRIPTOR_TYPE_ENDPOINT,   /* bDescriptorType: */
    HID_INT_EP,                     /* bEndpointAddress: Endpoint Address (IN) */
    0x03,                           /* bmAttributes: Interrupt endpoint */
    HID_INT_EP_SIZE,                /* wMaxPacketSize: 4 Byte max */
    0x00,
    HID_INT_EP_INTERVAL,            /* bInterval: Polling Interval */
    /* 34 */
    ///////////////////////////////////////
    /// string0 descriptor
    ///////////////////////////////////////
    USB_LANGID_INIT(USBD_LANGID_STRING),
    ///////////////////////////////////////
    /// string1 descriptor
    ///////////////////////////////////////
    0x14,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'C', 0x00,                  /* wcChar0 */
    'h', 0x00,                  /* wcChar1 */
    'e', 0x00,                  /* wcChar2 */
    'r', 0x00,                  /* wcChar3 */
    'r', 0x00,                  /* wcChar4 */
    'y', 0x00,                  /* wcChar5 */
    'U', 0x00,                  /* wcChar6 */
    'S', 0x00,                  /* wcChar7 */
    'B', 0x00,                  /* wcChar8 */
    ///////////////////////////////////////
    /// string2 descriptor
    ///////////////////////////////////////
    0x26,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'C', 0x00,                  /* wcChar0 */
    'h', 0x00,                  /* wcChar1 */
    'e', 0x00,                  /* wcChar2 */
    'r', 0x00,                  /* wcChar3 */
    'r', 0x00,                  /* wcChar4 */
    'y', 0x00,                  /* wcChar5 */
    'U', 0x00,                  /* wcChar6 */
    'S', 0x00,                  /* wcChar7 */
    'B', 0x00,                  /* wcChar8 */
    ' ', 0x00,                  /* wcChar9 */
    'H', 0x00,                  /* wcChar10 */
    'I', 0x00,                  /* wcChar11 */
    'D', 0x00,                  /* wcChar12 */
    ' ', 0x00,                  /* wcChar13 */
    'D', 0x00,                  /* wcChar14 */
    'E', 0x00,                  /* wcChar15 */
    'M', 0x00,                  /* wcChar16 */
    'O', 0x00,                  /* wcChar17 */
    ///////////////////////////////////////
    /// string3 descriptor
    ///////////////////////////////////////
    0x16,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    '2', 0x00,                  /* wcChar0 */
    '0', 0x00,                  /* wcChar1 */
    '2', 0x00,                  /* wcChar2 */
    '2', 0x00,                  /* wcChar3 */
    '1', 0x00,                  /* wcChar4 */
    '2', 0x00,                  /* wcChar5 */
    '3', 0x00,                  /* wcChar6 */
    '4', 0x00,                  /* wcChar7 */
    '5', 0x00,                  /* wcChar8 */
    '6', 0x00,                  /* wcChar9 */
#ifdef CONFIG_USB_HS
    ///////////////////////////////////////
    /// device qualifier descriptor
    ///////////////////////////////////////
    0x0a,
    USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER,
    0x00,
    0x02,
    0x00,
    0x00,
    0x00,
    0x40,
    0x01,
    0x00,
    // ///////////////////////////////////////
    // /// Other Speed Configuration Descriptor
    // ///////////////////////////////////////
    0x09,
    USB_DESCRIPTOR_TYPE_OTHER_SPEED,
    WBVAL(USB_HID_CONFIG_DESC_SIZ),
    0x01,
    0x01,
    0x00,
    USB_CONFIG_BUS_POWERED,
    USBD_MAX_POWER,
#endif
    0x00
};

#define REPORTID_MAX_COUNT 0x02
static const uint8_t hid_touch_report_desc[] = {
    0x05, 0x0d,                         // USAGE_PAGE (Digitizers)
    0x09, 0x04,                         // USAGE (Touch Screen)
    0xa1, 0x01,                         // COLLECTION (Application)
    0x85, 0x01,                         // REPORT_ID (Touch)
    FINGER_REPORT_DESC(TOUCH_LOGICAL_MAXNUM, TOUCH_X_PHYSICAL_MAXNUM, TOUCH_Y_PHYSICAL_MAXNUM)
    FINGER_REPORT_DESC(TOUCH_LOGICAL_MAXNUM, TOUCH_X_PHYSICAL_MAXNUM, TOUCH_Y_PHYSICAL_MAXNUM)
    FINGER_REPORT_DESC(TOUCH_LOGICAL_MAXNUM, TOUCH_X_PHYSICAL_MAXNUM, TOUCH_Y_PHYSICAL_MAXNUM)
    FINGER_REPORT_DESC(TOUCH_LOGICAL_MAXNUM, TOUCH_X_PHYSICAL_MAXNUM, TOUCH_Y_PHYSICAL_MAXNUM)
    FINGER_REPORT_DESC(TOUCH_LOGICAL_MAXNUM, TOUCH_X_PHYSICAL_MAXNUM, TOUCH_Y_PHYSICAL_MAXNUM)
    0x05, 0x0d,                         // USAGE_PAGE (Digitizers)
    0x55, 0x0C,                         // UNIT_EXPONENT (-4)
    0x66, 0x01, 0x10,                   // UNIT (Seconds)
    0x47, 0xff, 0xff, 0x00, 0x00,       // PHYSICAL_MAXIMUM (65535)
    0x27, 0xff, 0xff, 0x00, 0x00,       // LOGICAL_MAXIMUM (65535)
    0x75, 0x10,                         // REPORT_SIZE (16)
    0x95, 0x01,                         // REPORT_COUNT (1)
    0x09, 0x56,                         // USAGE (Scan Time)
    0x81, 0x02,                         // INPUT (Data,Var,Abs)

    0x09, 0x54,                         // USAGE (Contact count)
    0x25, 0x7f,                         // LOGICAL_MAXIMUM (127)
    0x95, 0x01,                         // REPORT_COUNT (1)
    0x75, 0x08,                         // REPORT_SIZE (8)
    0x81, 0x02,                         // INPUT (Data,Var,Abs)

    0x85, REPORTID_MAX_COUNT,           // REPORT_ID (Feature)
    0x09, 0x55,                         // USAGE(Contact Count Maximum)
    0x95, 0x01,                         // REPORT_COUNT (1)
    0x25, 0x02,                         // LOGICAL_MAXIMUM (2)
    0xb1, 0x02,                         // FEATURE (Data,Var,Abs)
    0x85, 0x44,                         // REPORT_ID (Feature)
    0x06, 0x00, 0xff,                   // USAGE_PAGE (Vendor Defined)
    0x09, 0xC5,                         // USAGE (Vendor Usage 0xC5)
    0x15, 0x00,                         // LOGICAL_MINIMUM (0)
    0x26, 0xff, 0x00,                   // LOGICAL_MAXIMUM (0xff)
    0x75, 0x08,                         // REPORT_SIZE (8)
    0x96, 0x00,  0x01,                  // REPORT_COUNT (0x100 (256))
    0xb1, 0x02,                         // FEATURE (Data,Var,Abs)
    0xc0,                               // END_COLLECTION
};
struct hid_touch_cfg_t {
    uint8_t tip;
    uint8_t id;
    int16_t x;
    int16_t y;
    uint16_t width;
    uint16_t azimuth;
};

struct hid_touch_report_t {
    uint8_t report_id;
    struct hid_touch_cfg_t touch[TOUCH_MAX_POINT_NUM];
    uint16_t scan_time;
    uint8_t count;
}__attribute__((packed));

struct hid_touch_t {
    rt_device_t dev;
    rt_sem_t sem;
    uint8_t last_event;
    uint8_t id[TOUCH_MAX_POINT_NUM];
    struct hid_touch_cfg_t finger[TOUCH_MAX_POINT_NUM];
    volatile uint8_t hid_state;
    volatile uint8_t cur_point_num;
    uint16_t hid_touch_rotate;
    struct rt_touch_data *data;
    struct rt_touch_info info;
    struct hid_touch_report_t *report;
    // test_action_settings
    // instruct_action_settings

} g_hid_touch;

struct usbd_interface touch_intf;
static rt_thread_t  hid_touch_tid;
/*!< touch report */
static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX struct hid_touch_report_t report;

#define HID_STATE_IDLE 0
#define HID_STATE_BUSY 1

void usbd_hid_get_report(uint8_t intf, uint8_t report_id, uint8_t report_type,
                            uint8_t **data, uint32_t *len)
{
    (*data)[0] = 2;
    (*data)[1] = TOUCH_MAX_POINT_NUM;
    *len = 2;
}

#ifdef LPKG_CHERRYUSB_DEVICE_COMPOSITE
void usbd_comp_touch_event_handler(uint8_t event)
#else
void usbd_event_handler(uint8_t event)
#endif
{
    switch (event) {
        case USBD_EVENT_RESET:
            break;
        case USBD_EVENT_CONNECTED:
            break;
        case USBD_EVENT_DISCONNECTED:
            break;
        case USBD_EVENT_RESUME:
            break;
        case USBD_EVENT_SUSPEND:
            break;
        case USBD_EVENT_CONFIGURED:
            break;
        case USBD_EVENT_SET_REMOTE_WAKEUP:
            break;
        case USBD_EVENT_CLR_REMOTE_WAKEUP:
            break;

        default:
            break;
    }
}

/* function ------------------------------------------------------------------*/
static struct hid_touch_t *get_hid_touch(void)
{
    return &g_hid_touch;
}

void usbd_hid_set_idle(uint8_t intf, uint8_t report_id, uint8_t duration)
{
    struct hid_touch_t *hid_touch = get_hid_touch();
    hid_touch->hid_state = HID_STATE_IDLE;
}

static void usbd_hid_int_callback(uint8_t ep, uint32_t nbytes)
{
    struct hid_touch_t *hid_touch = get_hid_touch();
    hid_touch->hid_state = HID_STATE_IDLE;
}

/*!< endpoint call back */
static struct usbd_endpoint hid_in_ep = {
    .ep_cb = usbd_hid_int_callback,
    .ep_addr = HID_INT_EP
};

void hid_touch_hw_info(void)
{
    struct hid_touch_t *hid_touch = get_hid_touch();
    USB_LOG_INFO("HID-TOUCH: info: %d %d %d %d %d\n",
                    (uint8_t)hid_touch->info.type,
                    (uint8_t)hid_touch->info.vendor,
                    (uint8_t)hid_touch->info.point_num,
                    (uint16_t)hid_touch->info.range_x,
                    (uint16_t)hid_touch->info.range_y);
}

void hid_touch_hw_data_dump(uint8_t id)
{
    struct hid_touch_t *hid_touch = get_hid_touch();
    USB_LOG_INFO("HID-TOUCH: hw: %d %d %d %d %d\n",
                    (uint8_t)hid_touch->data[id].track_id,
                    (uint16_t)hid_touch->data[id].x_coordinate,
                    (uint16_t)hid_touch->data[id].y_coordinate,
                    (uint16_t)hid_touch->data[id].timestamp,
                    (uint16_t)hid_touch->data[id].width);
}

void hid_touch_sw_data_dump(uint8_t id)
{
    USB_LOG_INFO("HID-TOUCH: sw: %d %d %d %d %d\n",
                    (uint8_t)report.touch[id].tip,
                    (uint8_t)report.touch[id].id,
                    (uint16_t)report.touch[id].x,
                    (uint16_t)report.touch[id].y,
                    (uint16_t)report.touch[id].width);
}

static int _touch_param_transform(uint8_t i)
{
    uint16_t temp = 0;
    uint32_t x = 0, y = 0;
    struct hid_touch_t *hid_touch = get_hid_touch();

    // 1. touch screen rotation
    if ((hid_touch->hid_touch_rotate == 90) ||
        (hid_touch->hid_touch_rotate == 270)) {
        temp = hid_touch->finger[i].y;
        hid_touch->finger[i].y = hid_touch->finger[i].x;
        hid_touch->finger[i].x = temp;
    }

    if (hid_touch->finger[i].x > TOUCH_X_PHYSICAL_MAXNUM ||
        hid_touch->finger[i].y > TOUCH_Y_PHYSICAL_MAXNUM)
        USB_LOG_WRN("WRN 1: The hardware x/y coordinates do not match the software scaling x/y reference values.\n"
                    "You can try rotating 90 degrees or 270 degrees.\n");

    // 2. resolution scaling
    x = hid_touch->finger[i].x;
    y = hid_touch->finger[i].y;
    x = x * TOUCH_LOGICAL_MAXNUM / TOUCH_X_PHYSICAL_MAXNUM;
    y = y * TOUCH_LOGICAL_MAXNUM / TOUCH_Y_PHYSICAL_MAXNUM;
    hid_touch->finger[i].x = (uint16_t)x;
    hid_touch->finger[i].y = (uint16_t)y;

    if (hid_touch->finger[i].x > TOUCH_LOGICAL_MAXNUM ||
        hid_touch->finger[i].y > TOUCH_LOGICAL_MAXNUM)
        USB_LOG_WRN("WRN 2: The x/y values have been scaled incorrectly.\n"
                    "You can try swapping the x/y logical resolution.\n");

    // 3. flip
    #ifdef HID_TOUCH_X_FLIP
    hid_touch->finger[i].x = TOUCH_LOGICAL_MAXNUM - hid_touch->finger[i].x;
    #endif
    #ifdef HID_TOUCH_Y_FLIP
    hid_touch->finger[i].y = TOUCH_LOGICAL_MAXNUM - hid_touch->finger[i].y;
    #endif

    if (report.touch[i].x < 0 || report.touch[i].y < 0)
        USB_LOG_WRN("WRN 3: The x/y coordinates flip set are incorret.\n"
                    "You can try filpping the x/y coordinates.\n");

    #ifdef HID_TOUCH_DEBUG
    hid_touch_sw_data_dump(i);
    hid_touch_hw_data_dump(i);
    #endif
    return 0;
}

uint8_t _finger_alloc(uint8_t id)
{
    uint8_t ret;
    struct hid_touch_t *hid_touch = get_hid_touch();

    ret = hid_touch->cur_point_num;
    hid_touch->id[hid_touch->cur_point_num] = id;
    hid_touch->cur_point_num++;

    return ret;
}

uint8_t _finger_get(uint8_t id)
{
    struct hid_touch_t *hid_touch = get_hid_touch();

    for (int i = 0; i < hid_touch->cur_point_num; i++) {
        if (hid_touch->id[i] == id) {
            return i;
        }
    }

    return id;
}

uint8_t _finger_release(uint8_t id)
{
    struct hid_touch_t *hid_touch = get_hid_touch();

    hid_touch->cur_point_num--;
    for (int i = 0; i < hid_touch->cur_point_num; i++) {
        if (hid_touch->id[i] != id)
            continue;
        for (int j = i; j < hid_touch->cur_point_num; j++) {
            hid_touch->id[j] = hid_touch->id[j + 1];
        }
        break;
    }

    return 0;
}

static int _touch_down(uint8_t i)
{
    uint8_t index;
    struct hid_touch_t *hid_touch = get_hid_touch();

    if (hid_touch->last_event == RT_TOUCH_EVENT_DOWN ||
        hid_touch->last_event == RT_TOUCH_EVENT_MOVE)
        rt_thread_mdelay(30);

    index = _finger_alloc(hid_touch->data[i].track_id);

    hid_touch->finger[index].tip = 0;
    hid_touch->finger[index].id = hid_touch->data[i].track_id;
    hid_touch->finger[index].x = hid_touch->data[i].x_coordinate;
    hid_touch->finger[index].y = hid_touch->data[i].y_coordinate;
    hid_touch->finger[index].width = hid_touch->data[i].width;

    _touch_param_transform(index);

    return 0;
}

static int _touch_move(uint8_t i)
{
    uint8_t index;
    struct hid_touch_t *hid_touch = get_hid_touch();

    if (hid_touch->last_event == RT_TOUCH_EVENT_DOWN)
        rt_thread_mdelay(30);

    index = _finger_get(hid_touch->data[i].track_id);

    hid_touch->finger[index].tip = 1;
    hid_touch->finger[index].id = hid_touch->data[i].track_id;
    hid_touch->finger[index].x = hid_touch->data[i].x_coordinate;
    hid_touch->finger[index].y = hid_touch->data[i].y_coordinate;
    hid_touch->finger[index].width = hid_touch->data[i].width;

    _touch_param_transform(index);

    return 0;
}

static int _touch_up(uint8_t i)
{
    struct hid_touch_t *hid_touch = get_hid_touch();

    _finger_release(hid_touch->data[i].track_id);
    memset((uint8_t *)&report, 0, sizeof(struct hid_touch_report_t));

    return 0;
}

static void hid_touch_send_data(void)
{
    int ret = 0;
    struct hid_touch_t *hid_touch = get_hid_touch();

    report.report_id = 0x01;
    report.scan_time = hid_touch->data->timestamp;
    report.count = hid_touch->cur_point_num;


    for (int i = 0; i < hid_touch->cur_point_num; i++)
        memcpy(&report.touch[i], &hid_touch->finger[i],
                sizeof(struct hid_touch_cfg_t));


    if (hid_touch->hid_state == HID_STATE_IDLE) {
        hid_touch->hid_state = HID_STATE_BUSY;
        ret = usbd_ep_start_write(hid_in_ep.ep_addr, (uint8_t *)&report,
                                    HID_INT_EP_SIZE);
        #ifdef HID_TOUCH_DEBUG
        USB_LOG_RAW("HID-TOUCH: Current effective finger count: %#lx \n",
                (long)hid_touch->cur_point_num);
        #endif
        if (ret < 0)
            return;
    }
}

static void hid_touch_thread(void *parameter)
{
    struct hid_touch_t *hid_touch = get_hid_touch();

    hid_touch->data = (struct rt_touch_data *)rt_malloc(
                        sizeof(struct rt_touch_data) *
                        hid_touch->info.point_num);

    while(1) {
        rt_sem_take(hid_touch->sem, RT_WAITING_FOREVER);

        if (rt_device_read(hid_touch->dev, 0, hid_touch->data, hid_touch->info.point_num)
                            == hid_touch->info.point_num) {
            for (rt_uint8_t i = 0; i < TOUCH_MAX_POINT_NUM; i++) {
                if (i > hid_touch->info.point_num)
                    break;
                switch(hid_touch->data[i].event) {
                    case RT_TOUCH_EVENT_MOVE:
                        USB_LOG_DBG("HID-TOUCH: RT_TOUCH_EVENT_MOVE\n");
                        _touch_move(i);
                        hid_touch_send_data();
                    break;
                    case RT_TOUCH_EVENT_DOWN:
                        USB_LOG_DBG("HID-TOUCH: RT_TOUCH_EVENT_DOWN\n");
                        _touch_down(i);
                        hid_touch_send_data();
                    break;
                    case RT_TOUCH_EVENT_UP:
                        USB_LOG_DBG("HID-TOUCH: RT_TOUCH_EVENT_UP\n");
                        _touch_up(i);
                        hid_touch_send_data();
                    break;
                    default:
                        continue;
                }
                hid_touch->last_event = hid_touch->data[i].event;
            }

        }
        rt_device_control(hid_touch->dev, RT_TOUCH_CTRL_ENABLE_INT, RT_NULL);
    }
}

static rt_err_t rx_callback(rt_device_t dev, rt_size_t size)
{
    struct hid_touch_t *hid_touch = get_hid_touch();
    rt_device_control(hid_touch->dev, RT_TOUCH_CTRL_DISABLE_INT, RT_NULL);
    rt_sem_release(hid_touch->sem);
    return 0;
}

static int hid_touch_hw_init(char *dev_name)
{
    int ret = 0;
    uint8_t id[10];
    struct hid_touch_t *hid_touch = get_hid_touch();

    hid_touch->dev = rt_device_find(dev_name);
    if (hid_touch->dev == RT_NULL) {
        USB_LOG_ERR("HID-TOUCH: can't find device:%s\n", (char *)hid_touch->dev);
        return -1;
    }

    rt_device_close(hid_touch->dev);
#ifdef HID_TOUCH_INT_MODE
    ret = rt_device_open(hid_touch->dev, RT_DEVICE_FLAG_INT_RX);
#else
    ret = rt_device_open(hid_touch->dev, RT_DEVICE_FLAG_RDONLY);
#endif
    if (ret != RT_EOK) {
        USB_LOG_ERR("HID-TOUCH: open device failed!");
        return -1;
    }

    rt_device_control(hid_touch->dev, RT_TOUCH_CTRL_GET_ID, id);
    rt_device_control(hid_touch->dev, RT_TOUCH_CTRL_GET_INFO, &hid_touch->info);
    hid_touch_hw_info();

    rt_device_set_rx_indicate(hid_touch->dev, rx_callback);
    hid_touch->sem = rt_sem_create("hid_touch_sem", 0, RT_IPC_FLAG_FIFO);
    if (hid_touch->sem == RT_NULL) {
        USB_LOG_ERR("HID-TOUCH: create dynamic semaphore failed.\n");
        return -1;
    }

    hid_touch_tid = rt_thread_create("hid_touch", hid_touch_thread,
                                     RT_NULL, 1024 * 2,
                                     RT_THREAD_PRIORITY_MAX - 2, 20);

    if (hid_touch_tid != RT_NULL)
        rt_thread_startup(hid_touch_tid);

    return 0;
}

#ifdef LPKG_CHERRYUSB_DEVICE_COMPOSITE
#include "composite_template.h"
int usbd_comp_touch_init(uint8_t *ep_table, void *data)
{
    hid_in_ep.ep_addr = ep_table[0];
    usbd_add_interface(usbd_hid_init_intf(&touch_intf,
                                            hid_touch_report_desc,
                                            HID_MOUSE_REPORT_DESC_SIZE));
    usbd_add_endpoint(&hid_in_ep);
    return 0;
}
#endif

static void hid_touch_init(void)
{
    /*!< init touch report data */
    struct hid_touch_t *hid_touch = get_hid_touch();

    memset(hid_touch, 0, sizeof(struct hid_touch_t));
    hid_touch->hid_touch_rotate = AIC_HID_ROTATE_DEGREE;
    hid_touch_hw_init(HID_TOUCH_NAME);
#ifndef LPKG_CHERRYUSB_DEVICE_COMPOSITE
    usbd_desc_register(hid_descriptor);
    usbd_add_interface(usbd_hid_init_intf(&touch_intf,
                                            hid_touch_report_desc,
                                            HID_MOUSE_REPORT_DESC_SIZE));
    usbd_add_endpoint(&hid_in_ep);
    usbd_initialize();
#else
    usbd_comp_func_register(hid_descriptor,
                            usbd_comp_touch_event_handler,
                            usbd_comp_touch_init, "touch");
#endif
}

#if defined(KERNEL_RTTHREAD)
#include <rtthread.h>
#include <rtdevice.h>

int usbd_hid_touch_init(void)
{
    hid_touch_init();
    return 0;
}
#if !defined(LPKG_CHERRYUSB_DYNAMIC_REGISTRATION_MODE)
INIT_APP_EXPORT(usbd_hid_touch_init);
#endif

#endif
