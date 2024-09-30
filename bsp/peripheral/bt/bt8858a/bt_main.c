#include <rtthread.h>
#include "bt_api.h"

struct bt_debug_cmd {
    const char *cmd;
    const char *exp;
    int (*entry)(void);
};

static struct bt_debug_cmd cmd[] =
{
    /* vol opt */
    {"vol+", "vol+", bt_vol_add},
    {"vol-", "vol-", bt_vol_dec},
    {"mute", "mute", bt_vol_mute},
    {"unmute", "unmute", bt_vol_unmute},

    /* phone call */
    {"phone_break", "phone break", bt_phone_break},
    {"phone_reject", "phone reject", bt_phone_reject},
    {"phone_recall", "phone recall", bt_phone_recall},
    {"phone_incoming", "phone incoming", bt_phone_incoming},
    {"phone_break", "phone break", bt_phone_break},

    /* music */
    {"play", "music play", bt_audio_play},
    {"pause", "music pause", bt_audio_pause},
    {"stop", "audio stop", bt_audio_stop},
    {"prev", "audio prev", bt_audio_prev},
    {"next", "audio next", bt_audio_next},
};

static void bt_cmd(int argc, char *argv[])
{
    if (argc < 2)
        return ;

    for (int i = 0; i < sizeof(cmd) / sizeof(cmd[0]); i++) {
        if (strcmp(argv[1], cmd[i].cmd) == 0) {
            cmd[i].entry();
        }
    }
}

MSH_CMD_EXPORT_ALIAS(bt_cmd, bt_cmd, debug bt command);

INIT_DEVICE_EXPORT(bt_init);
