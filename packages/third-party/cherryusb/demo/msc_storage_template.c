/*
 * Copyright (c) 2023, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "usbd_core.h"
#include "usbd_msc.h"

#define MSC_IN_EP  0x81
#define MSC_OUT_EP 0x02

#define USBD_VID           0x33C3
#define USBD_PID           0xFFFF
#define USBD_MAX_POWER     100
#define USBD_LANGID_STRING 1033

#define USB_CONFIG_SIZE (9 + MSC_DESCRIPTOR_LEN)

#ifdef CONFIG_USB_HS
#define MSC_MAX_MPS 512
#else
#define MSC_MAX_MPS 64
#endif


const uint8_t msc_storage_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_1_1, 0x00, 0x00, 0x00, USBD_VID, USBD_PID, 0x0200, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x01, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    MSC_DESCRIPTOR_INIT(0x00, MSC_OUT_EP, MSC_IN_EP, MSC_MAX_MPS, 0x02),
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
    'M', 0x00,                  /* wcChar10 */
    'S', 0x00,                  /* wcChar11 */
    'C', 0x00,                  /* wcChar12 */
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
#endif
    0x00
};

#include <string.h>
#include <string.h>
#include "ff.h"			/* Declarations of FatFs API */
/* ELM FatFs provide a DIR struct */
#define HAVE_DIR_STRUCTURE
#include <dfs_fs.h>
#include <dfs_file.h>
#include <dfs_elm.h>
#include "diskio.h"		/* Declarations of device I/O functions */

struct usbd_storage_p {
    struct dfs_filesystem *fs;
    rt_device_t dev;
    uint32_t block_num;
    uint32_t block_size;
    unsigned char pdrv;
    char fs_path[10];
    char fs_type[5];
    bool suspended;
};

struct usbd_interface intf0;
struct usbd_storage_p usbd_storage;
static void usbd_msc_suspend(void);
static void usbd_msc_configured(void);

void usbd_event_handler(uint8_t event)
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
            usbd_msc_suspend();
            break;
        case USBD_EVENT_CONFIGURED:
            usbd_msc_configured();
            break;
        case USBD_EVENT_SET_REMOTE_WAKEUP:
            break;
        case USBD_EVENT_CLR_REMOTE_WAKEUP:
            break;

        default:
            break;
    }
}

struct usbd_storage_p *get_usbd_storage(void)
{
    return &usbd_storage;
}

static void usbd_msc_suspend(void)
{
    int ret = 0;
    long device_type = 0;
    struct usbd_storage_p *usbd_storage = get_usbd_storage();

    if ((FATFS *)usbd_storage->fs->data == NULL) {
#ifndef KERNEL_RTTHREAD
        disk_ioctl(usbd_storage->pdrv, GET_DEVICE_TYPE, &device_type);
#endif
        ret = usbd_storage->fs->ops->mount(usbd_storage->fs, 0, (void *)device_type);
        if (ret < 0)
            USB_LOG_ERR("Failed to mount %s to %s\n", (char *)usbd_storage->dev, usbd_storage->fs_path);
        else
            USB_LOG_INFO("Mount %s to %s\n", (char *)usbd_storage->dev, usbd_storage->fs_path);
    }

}

static void usbd_msc_configured(void)
{
    int ret;
    struct usbd_storage_p *usbd_storage = get_usbd_storage();

    if ((FATFS *)usbd_storage->fs->data == NULL)
        return;

    ret = usbd_storage->fs->ops->unmount(usbd_storage->fs);

    if (ret < 0)
        USB_LOG_ERR("Failed to unmount %s !\n", usbd_storage->fs_path);
    else
        USB_LOG_INFO("Unmount %s \n", usbd_storage->fs_path);
}

void usbd_msc_get_cap(uint8_t lun, uint32_t *block_num, uint16_t *block_size)
{
    struct usbd_storage_p *usbd_storage = get_usbd_storage();

    *block_num = usbd_storage->block_num;
    *block_size = usbd_storage->block_size;

    USB_LOG_DBG("block_num:%ld block_size:%d\n", *block_num, *block_size);
}

int usbd_msc_sector_read(uint32_t sector, uint8_t *buffer, uint32_t length)
{
    int ret = -1;
    struct usbd_storage_p *usbd_storage = get_usbd_storage();
#if defined(KERNEL_RTTHREAD)
    ret = rt_device_read(usbd_storage->dev, sector, buffer, 1);
    if (ret == length / usbd_storage->block_size)
        return 0;
#else
    ret = disk_read(usbd_storage->pdrv, buffer, sector, 1);
#endif
    return ret;
}

int usbd_msc_sector_write(uint32_t sector, uint8_t *buffer, uint32_t length)
{
    int ret = -1;
    struct usbd_storage_p *usbd_storage = get_usbd_storage();
#if defined(KERNEL_RTTHREAD)
    ret = rt_device_write(usbd_storage->dev, sector, buffer, 1);
    if (ret == length / usbd_storage->block_size)
        return 0;
#else
    ret = disk_write(usbd_storage->pdrv, buffer, sector, 1);
#endif
    return ret;
}

extern struct dfs_filesystem filesystem_table[];
int usbd_storage_init(char *path)
{
    FATFS *f; // using elmfat
    struct dfs_filesystem *iter;
    struct usbd_storage_p *usbd_storage = get_usbd_storage();

    for (iter = &filesystem_table[0];
            iter < &filesystem_table[DFS_FILESYSTEMS_MAX]; iter++) {
        if ((iter != NULL) && (iter->path != NULL)) {
            if ((strcmp(iter->path, path) == 0)
                 && strcmp(iter->ops->name, "elm") == 0)
                goto __init;
        }
    }
    USB_LOG_WRN("Invalid file system!\n");

    return -1;

__init:

    usbd_storage->fs = dfs_filesystem_lookup(path);
    f = (FATFS *)usbd_storage->fs->data;
    usbd_storage->pdrv = f->pdrv;
    usbd_storage->dev = usbd_storage->fs->dev_id;

#if defined(KERNEL_RTTHREAD)
    int ret;
    struct rt_device_blk_geometry geometry;

    rt_device_close(usbd_storage->dev);
    ret = rt_device_open(usbd_storage->dev, RT_DEVICE_FLAG_RDWR);
    if (ret != RT_EOK) {
        USB_LOG_ERR("Open device failed !\n");
        return -1;
    }

    rt_memset(&geometry, 0, sizeof(geometry));
    ret = rt_device_control(usbd_storage->dev, RT_DEVICE_CTRL_BLK_GETGEOME, &geometry);
    if (ret != RT_EOK) {
        USB_LOG_ERR("Get geometry failed!\n");
        return -1;
    }

    usbd_storage->block_num = geometry.sector_count;
    usbd_storage->block_size = geometry.block_size;
#else
    disk_ioctl(usbd_storage->pdrv, GET_SECTOR_COUNT, &usbd_storage->block_num);
    disk_ioctl(usbd_storage->pdrv, GET_SECTOR_SIZE, &usbd_storage->block_size);
#endif
    strcpy(usbd_storage->fs_type, iter->ops->name);
    strcpy(usbd_storage->fs_path, iter->path);

    return 0;
}

int msc_storage_init(char *path)
{
    int ret = 0;

    struct usbd_storage_p *usbd_storage = get_usbd_storage();

    if (usbd_storage->fs != NULL) {
        USB_LOG_WRN("Msc storage already initialized ! (%s)\n", usbd_storage->fs_path);
        return 0;
    }

    ret = usbd_storage_init(path);
    if (ret < 0)
        return -1;

    usbd_desc_register(msc_storage_descriptor);
    usbd_add_interface(usbd_msc_init_intf(&intf0, MSC_OUT_EP, MSC_IN_EP));

    usbd_initialize();

    return 0;
}

static void cmd_msc_usage(void)
{
    printf("Usage: msc_init [mountpoint]\n");
    printf("Example: msc_init /rodata\n");
}

static int cmd_msc_storage_init(int argc, char *argv[])
{
    if (argc < 2) {
        cmd_msc_usage();
        return -1;
    }

    msc_storage_init(argv[1]);
    return 0;
}

#if defined(RT_USING_FINSH)
MSH_CMD_EXPORT_ALIAS(cmd_msc_storage_init, msc_init, Mount usb massstorage);
#elif defined(AIC_CONSOLE_BARE_DRV)
#include <console.h>
CONSOLE_CMD(msc_init, cmd_msc_storage_init, "Mount usb massstorage");
#endif

#if defined(KERNEL_RTTHREAD)
rt_thread_t usbd_msc_tid;

static void usbd_msc_detection_thread(void *parameter)
{
    int ret = -1;

    // waiting to mount the correspongding file system
    rt_thread_mdelay(400);

    ret = msc_storage_init(MSC_STORAGE_PATH);
    if (ret < 0)
        USB_LOG_ERR("Failed to detect %s !\n", MSC_STORAGE_PATH);

}

int usbd_msc_detection(void)
{
    usbd_msc_tid = rt_thread_create("usbd_msc_detection", usbd_msc_detection_thread, RT_NULL,
                           1536, RT_THREAD_PRIORITY_MAX - 2, 20);
    if (usbd_msc_tid != RT_NULL)
        rt_thread_startup(usbd_msc_tid);
    else
        printf("create usbd_msc_detection thread err!\n");

    return RT_EOK;
}
INIT_APP_EXPORT(usbd_msc_detection);
#endif
