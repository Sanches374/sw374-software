/*
 * Copyright (c) 2024, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "usbd_core.h"
#include "usbd_audio.h"
#include <hal_audio.h>


#if !defined(KERNEL_RTTHREAD)
#include "ringbuffer.h"
#else
#include <rtthread.h>
#include <rtdevice.h>
#endif

#define USBD_VID           0x33C3
#define USBD_PID           0xffff
#define USBD_MAX_POWER     100
#define USBD_LANGID_STRING 1033

#ifdef CONFIG_USB_HS
#define EP_INTERVAL 0x04
#else
#define EP_INTERVAL 0x01
#endif

#define AUDIO_OUT_EP 0x01

#define AUDIO_OUT_CLOCK_ID 0x01
#define AUDIO_OUT_FU_ID    0x03

#define AUDIO_FREQ      48000
#define HALF_WORD_BYTES 2  //2 half word (one channel)
#define SAMPLE_BITS     16 //16 bit per channel
#define UAC_MAX_VOLUME MAX_VOLUME_0DB
#define UAC_MIN_VOLUME 80


#define BMCONTROL (AUDIO_V2_FU_CONTROL_MUTE | AUDIO_V2_FU_CONTROL_VOLUME)

#define OUT_CHANNEL_NUM 2

#if OUT_CHANNEL_NUM == 1
#define OUTPUT_CTRL      DBVAL(BMCONTROL), DBVAL(BMCONTROL)
#define OUTPUT_CH_ENABLE 0x00000000
#elif OUT_CHANNEL_NUM == 2
#define OUTPUT_CTRL      DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL)
#define OUTPUT_CH_ENABLE 0x00000003
#elif OUT_CHANNEL_NUM == 3
#define OUTPUT_CTRL      DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL)
#define OUTPUT_CH_ENABLE 0x00000007
#elif OUT_CHANNEL_NUM == 4
#define OUTPUT_CTRL      DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL)
#define OUTPUT_CH_ENABLE 0x0000000f
#elif OUT_CHANNEL_NUM == 5
#define OUTPUT_CTRL      DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL)
#define OUTPUT_CH_ENABLE 0x0000001f
#elif OUT_CHANNEL_NUM == 6
#define OUTPUT_CTRL      DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL)
#define OUTPUT_CH_ENABLE 0x0000003F
#elif OUT_CHANNEL_NUM == 7
#define OUTPUT_CTRL      DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL)
#define OUTPUT_CH_ENABLE 0x0000007f
#elif OUT_CHANNEL_NUM == 8
#define OUTPUT_CTRL      DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL), DBVAL(BMCONTROL)
#define OUTPUT_CH_ENABLE 0x000000ff
#endif

#define AUDIO_OUT_PACKET ((uint32_t)((AUDIO_FREQ * HALF_WORD_BYTES * OUT_CHANNEL_NUM) / 1000))

#define USB_AUDIO_CONFIG_DESC_SIZ (9 +                                                     \
                                   AUDIO_V2_AC_DESCRIPTOR_INIT_LEN +                       \
                                   AUDIO_V2_SIZEOF_AC_CLOCK_SOURCE_DESC +                  \
                                   AUDIO_V2_SIZEOF_AC_INPUT_TERMINAL_DESC +                \
                                   AUDIO_V2_SIZEOF_AC_FEATURE_UNIT_DESC(OUT_CHANNEL_NUM) + \
                                   AUDIO_V2_SIZEOF_AC_OUTPUT_TERMINAL_DESC +               \
                                   AUDIO_V2_AS_DESCRIPTOR_INIT_LEN)

#define AUDIO_AC_SIZ (AUDIO_V2_SIZEOF_AC_HEADER_DESC +                        \
                      AUDIO_V2_SIZEOF_AC_CLOCK_SOURCE_DESC +                  \
                      AUDIO_V2_SIZEOF_AC_INPUT_TERMINAL_DESC +                \
                      AUDIO_V2_SIZEOF_AC_FEATURE_UNIT_DESC(OUT_CHANNEL_NUM) + \
                      AUDIO_V2_SIZEOF_AC_OUTPUT_TERMINAL_DESC)

uint8_t audio_v2_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0x00, 0x00, 0x00, USBD_VID, USBD_PID, 0x0001, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(USB_AUDIO_CONFIG_DESC_SIZ, 0x02, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    AUDIO_V2_AC_DESCRIPTOR_INIT(0x00, 0x02, AUDIO_AC_SIZ, AUDIO_CATEGORY_SPEAKER, 0x00, 0x00),
    AUDIO_V2_AC_CLOCK_SOURCE_DESCRIPTOR_INIT(AUDIO_OUT_CLOCK_ID, 0x03, 0x03),
    AUDIO_V2_AC_INPUT_TERMINAL_DESCRIPTOR_INIT(0x02, AUDIO_TERMINAL_STREAMING, 0x01, OUT_CHANNEL_NUM, OUTPUT_CH_ENABLE, 0x0000),
    AUDIO_V2_AC_FEATURE_UNIT_DESCRIPTOR_INIT(AUDIO_OUT_FU_ID, 0x02, OUTPUT_CTRL),
    AUDIO_V2_AC_OUTPUT_TERMINAL_DESCRIPTOR_INIT(0x04, AUDIO_OUTTERM_SPEAKER, 0x03, 0x01, 0x0000),
    AUDIO_V2_AS_DESCRIPTOR_INIT(0x01, 0x02, OUT_CHANNEL_NUM, OUTPUT_CH_ENABLE, HALF_WORD_BYTES, SAMPLE_BITS, AUDIO_OUT_EP, 0x09, AUDIO_OUT_PACKET, EP_INTERVAL),
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
    'U', 0x00,                  /* wcChar10 */
    'A', 0x00,                  /* wcChar11 */
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
    '1', 0x00,                  /* wcChar3 */
    '0', 0x00,                  /* wcChar4 */
    '3', 0x00,                  /* wcChar5 */
    '1', 0x00,                  /* wcChar6 */
    '0', 0x00,                  /* wcChar7 */
    '0', 0x00,                  /* wcChar8 */
    '3', 0x00,                  /* wcChar9 */
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

static const uint8_t default_sampling_freq_table[] = {
    AUDIO_SAMPLE_FREQ_NUM(5),
    AUDIO_SAMPLE_FREQ_4B(8000),
    AUDIO_SAMPLE_FREQ_4B(8000),
    AUDIO_SAMPLE_FREQ_4B(0x00),
    AUDIO_SAMPLE_FREQ_4B(16000),
    AUDIO_SAMPLE_FREQ_4B(16000),
    AUDIO_SAMPLE_FREQ_4B(0x00),
    AUDIO_SAMPLE_FREQ_4B(32000),
    AUDIO_SAMPLE_FREQ_4B(32000),
    AUDIO_SAMPLE_FREQ_4B(0x00),
    AUDIO_SAMPLE_FREQ_4B(48000),
    AUDIO_SAMPLE_FREQ_4B(48000),
    AUDIO_SAMPLE_FREQ_4B(0x00),
    AUDIO_SAMPLE_FREQ_4B(96000),
    AUDIO_SAMPLE_FREQ_4B(96000),
    AUDIO_SAMPLE_FREQ_4B(0x00),
};

#define TX_FIFO_PERIOD_COUNT            20
#define TX_FIFO_SIZE                    AUDIO_OUT_PACKET*TX_FIFO_PERIOD_COUNT//3840

void usbd_audio_iso_out_callback(uint8_t ep, uint32_t nbytes);

struct audio_volume_mute {
    int volume;
    bool     mute;
} usbd_volume_mute;

uint8_t audio_fifo_tx[TX_FIFO_SIZE] __attribute__((aligned(64)));
static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t read_buffer[AUDIO_OUT_PACKET];

aic_audio_ctrl audio_ctrl0;
volatile bool rx_flag = 0;
uint32_t total_len = 0;
uint8_t usbd_start_flag = 0;

static struct usbd_endpoint audio_out_ep = {
    .ep_cb = usbd_audio_iso_out_callback,
    .ep_addr = AUDIO_OUT_EP
};

struct usbd_interface uac_intf0;
struct usbd_interface uac_intf1;

struct audio_entity_info audio_entity_table[] = {
    { .bEntityId = AUDIO_OUT_CLOCK_ID,
      .bDescriptorSubtype = AUDIO_CONTROL_CLOCK_SOURCE,
      .ep = AUDIO_OUT_EP },
    { .bEntityId = AUDIO_OUT_FU_ID,
      .bDescriptorSubtype = AUDIO_CONTROL_FEATURE_UNIT,
      .ep = AUDIO_OUT_EP },
};

void usbd_clear_buffer(void)
{
    static uint8_t reset_num = 1;
    /* The usb enumeration generates two reset operations.*/
    if (reset_num++>2) {
        memset(read_buffer, 0, AUDIO_OUT_PACKET);
    }
}

#ifdef LPKG_CHERRYUSB_DEVICE_COMPOSITE
void usbd_comp_uac_event_handler(uint8_t event)
#else
void usbd_event_handler(uint8_t event)
#endif
{
    switch (event) {
        case USBD_EVENT_RESET:
            usbd_clear_buffer();
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

void usbd_audio_set_volume(uint8_t ep, uint8_t ch, int volume)
{
    USB_LOG_DBG("set volume:%d\r\n", volume);
    usbd_volume_mute.volume = volume;
}

int usbd_audio_get_volume(uint8_t ep, uint8_t ch)
{
    int reg_volume;
    USB_LOG_DBG("get volume:%d\r\n", usbd_volume_mute.volume);
    reg_volume = usbd_volume_mute.volume * (UAC_MAX_VOLUME - UAC_MIN_VOLUME) / 100 + UAC_MIN_VOLUME;
    hal_audio_set_playback_volume(&audio_ctrl0, reg_volume);
    return usbd_volume_mute.volume;
}

void usbd_audio_set_mute(uint8_t ep, uint8_t ch, bool mute)
{
    usbd_volume_mute.mute = mute;
}

bool usbd_audio_get_mute(uint8_t ep, uint8_t ch)
{
    return usbd_volume_mute.mute;
}

void usbd_audio_open(uint8_t intf)
{
    rx_flag = 1;
    /* setup first out ep read transfer */
    usbd_ep_start_read(audio_out_ep.ep_addr, read_buffer, AUDIO_OUT_PACKET);
    USB_LOG_DBG("OPEN\r\n");
}

void usbd_audio_close(uint8_t intf)
{
    USB_LOG_DBG("CLOSE\r\n");
    rx_flag = 0;
}


void usbd_audio_set_sampling_freq(uint8_t ep, uint32_t sampling_freq)
{
    uint16_t packet_size = 0;
    USB_LOG_DBG("usb device audio sampling_freq %ld,\r\n", sampling_freq);
    if (ep == audio_out_ep.ep_addr) {
        packet_size = ((sampling_freq * 2 * OUT_CHANNEL_NUM) / 1000);
        audio_v2_descriptor[18 + USB_AUDIO_CONFIG_DESC_SIZ - 11] = packet_size;
        audio_v2_descriptor[18 + USB_AUDIO_CONFIG_DESC_SIZ - 10] = packet_size >> 8;
    }
}

void usbd_audio_get_sampling_freq_table(uint8_t ep, uint8_t **sampling_freq_table)
{
    if (ep == audio_out_ep.ep_addr) {
        *sampling_freq_table = (uint8_t *)default_sampling_freq_table;
    }
}

#if defined(KERNEL_RTTHREAD)
struct rt_ringbuffer  ring_buf0;
void audio_ringbuffer_init(void)
{
    rt_ringbuffer_init(&ring_buf0, audio_fifo_tx, TX_FIFO_SIZE);
}
#else
ringbuf_t ring_buf0;
void audio_ringbuffer_init(void)
{
    ring_buf0.buffer = audio_fifo_tx;
    ring_buf0.size = TX_FIFO_SIZE;
    ring_buf0.write = 0;
    ring_buf0.read = 0;
    ring_buf0.data_len = 0;
}
#endif

void usbd_audio_data_process(uint8_t* read_buffer)
{
    int length, wr_size;
    uint8_t *tmp_buf;

    length = AUDIO_OUT_PACKET;

    if (!usbd_start_flag) {
        wr_size = 0;
        tmp_buf = read_buffer;

        #if defined(KERNEL_RTTHREAD)
        wr_size = rt_ringbuffer_put(&ring_buf0, tmp_buf, length);
        #else
        wr_size = ringbuf_in(&ring_buf0, tmp_buf, length);
        #endif
        total_len += wr_size;
        if (total_len >= TX_FIFO_SIZE)
        {
            usbd_start_flag = 1;
            hal_audio_playback_start(&audio_ctrl0);
        }
    } else {
        tmp_buf = read_buffer;
        #if defined(KERNEL_RTTHREAD)
        wr_size = rt_ringbuffer_put_force(&ring_buf0, tmp_buf, length);
        #else
        wr_size = ringbuf_in(&ring_buf0, tmp_buf, length);
        #endif
    }
}

void usbd_hal_audio_callback(aic_audio_ctrl *pcodec, void *arg)
{
    unsigned long event = (unsigned long)arg;

    switch (event) {
        case AUDIO_TX_PERIOD_INT:
            #if !defined(KERNEL_RTTHREAD)
            ring_buf0.read += pcodec->tx_info.buf_info.period_len;
            ring_buf0.data_len -= pcodec->tx_info.buf_info.period_len;
            #else
            if (ring_buf0.buffer_size - ring_buf0.read_index > pcodec->tx_info.buf_info.period_len)
            {
                ring_buf0.read_index += pcodec->tx_info.buf_info.period_len;
            } else {
                ring_buf0.read_mirror = ~ring_buf0.read_mirror;
                ring_buf0.read_index = pcodec->tx_info.buf_info.period_len - (ring_buf0.buffer_size - ring_buf0.read_index);
            }
            #endif

            aicos_dcache_clean_range((void *)audio_fifo_tx, TX_FIFO_SIZE);
            break;
        default:
            USB_LOG_ERR("%s(%d)\n", __func__, __LINE__);
            break;
    }
}

void usbd_audio_config(aic_audio_ctrl *pcodec, uint32_t samplerate, uint32_t channel)
{
    pcodec->tx_info.buf_info.buf = (void *)audio_fifo_tx;
    pcodec->tx_info.buf_info.buf_len = TX_FIFO_SIZE;
    pcodec->tx_info.buf_info.period_len = TX_FIFO_SIZE / TX_FIFO_PERIOD_COUNT;

    hal_audio_init(pcodec);
    hal_audio_attach_callback(pcodec, usbd_hal_audio_callback, NULL);

    hal_audio_set_playback_channel(pcodec, channel);
    hal_audio_set_samplerate(pcodec, samplerate);
    pcodec->config.samplerate = samplerate;
    pcodec->config.channel = channel;
    pcodec->config.samplebits = 16;

#ifdef AIC_AUDIO_SPK_0
    hal_audio_set_playback_by_spk0(pcodec);
#endif
#ifdef AIC_AUDIO_SPK_1
    hal_audio_set_playback_by_spk1(pcodec);
#endif
#ifdef AIC_AUDIO_SPK_0_1
    hal_audio_set_playback_by_spk0(pcodec);
    hal_audio_set_playback_by_spk1(pcodec);
#ifdef AIC_AUDIO_SPK0_OUTPUT_DIFFERENTIAL
    hal_audio_set_pwm0_differential(pcodec);
#endif
#ifdef AIC_AUDIO_SPK1_OUTPUT_DIFFERENTIAL
    hal_audio_set_pwm1_differential(pcodec);
#endif
#endif
}

void usbd_audio_hal_init(void)
{
    unsigned int pa, group, pin;

    usbd_audio_config(&audio_ctrl0, AUDIO_FREQ, OUT_CHANNEL_NUM);

    pa = hal_gpio_name2pin(AIC_AUDIO_PA_ENABLE_GPIO);
    group = GPIO_GROUP(pa);
    pin = GPIO_GROUP_PIN(pa);
    hal_gpio_set_func(group, pin, 1);
    hal_gpio_direction_output(group, pin);

#ifdef AIC_AUDIO_EN_PIN_HIGH
    hal_gpio_set_output(group, pin);
#else
    hal_gpio_clr_output(group, pin);
#endif
    hal_dma_init();
    aicos_request_irq(DMA_IRQn, hal_dma_irq, 0, NULL, NULL);

}

void usbd_audio_iso_out_callback(uint8_t ep, uint32_t nbytes)
{
    usbd_audio_data_process(read_buffer);
    usbd_ep_start_read(audio_out_ep.ep_addr, read_buffer, AUDIO_OUT_PACKET);
}


#ifdef LPKG_CHERRYUSB_DEVICE_COMPOSITE
#include "composite_template.h"
int usbd_comp_uac_init(uint8_t *ep_table, void *data)
{
    audio_entity_table[0].ep = ep_table[0];
    audio_entity_table[1].ep = ep_table[0];
    audio_out_ep.ep_addr = ep_table[0];
    usbd_add_interface(usbd_audio_init_intf(&uac_intf0, 0x0200, audio_entity_table, 2));
    usbd_add_interface(usbd_audio_init_intf(&uac_intf1, 0x0200, audio_entity_table, 2));
    usbd_add_endpoint(&audio_out_ep);
    return 0;
}
#endif

int audio_v2_init(void)
{
    audio_ringbuffer_init();
    usbd_audio_hal_init();
#ifndef LPKG_CHERRYUSB_DEVICE_COMPOSITE
    usbd_desc_register(audio_v2_descriptor);
    usbd_add_interface(usbd_audio_init_intf(&uac_intf0, 0x0200, audio_entity_table, 2));
    usbd_add_interface(usbd_audio_init_intf(&uac_intf1, 0x0200, audio_entity_table, 2));
    usbd_add_endpoint(&audio_out_ep);
    usbd_initialize();
#else
    usbd_comp_func_register(audio_v2_descriptor,
                            usbd_comp_uac_event_handler,
                            usbd_comp_uac_init, NULL);
#endif
    return 0;
}

#if defined(KERNEL_RTTHREAD)
rt_thread_t usb_audio_tid;

void audio_usb_thread_entry(void *parameter) {
   audio_v2_init();
}

int usbd_audio_v2_init(void)
{
    usb_audio_tid = rt_thread_create("usb-audio-test", audio_usb_thread_entry, RT_NULL, 2048, 10, 50);
    if (usb_audio_tid != RT_NULL)
        rt_thread_startup(usb_audio_tid);
    return 0;
}
#if !defined(LPKG_CHERRYUSB_DYNAMIC_REGISTRATION_MODE)
INIT_APP_EXPORT(usbd_audio_v2_init);
#endif
#endif
