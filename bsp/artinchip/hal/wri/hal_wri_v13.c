/*
 * Copyright (c) 2023-2024, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors: matteo <duanmt@artinchip.com>
 */

#include "aic_core.h"
#include "aic_reboot_reason.h"
#include "hal_wri.h"

#define WRI_FLAG_SE_DM_NDM_RST      BIT(30)
#define WRI_FLAG_SE_WDOG_CPU_RST    BIT(29)
#define WRI_FLAG_SE_WDOG_SYS_RST    BIT(28)
#define WRI_FLAG_SC_DM_CPU_RST      BIT(27)
#define WRI_FLAG_SC_DM_NDM_RST      BIT(26)
#define WRI_FLAG_SC_WDOG_CPU_RST    BIT(25)
#define WRI_FLAG_SC_WDOG_SYS_RST    BIT(24)
#define WRI_FLAG_CS_DM_CPU_RST      BIT(23)
#define WRI_FLAG_CS_DM_NDM_RST      BIT(22)
#define WRI_FLAG_CS_WDOG_CPU_RST    BIT(21)
#define WRI_FLAG_CS_WDOG_SYS_RST    BIT(20)
#define WRI_FLAG_SP_DM_CPU_RST      BIT(19)
#define WRI_FLAG_SP_DM_NDM_RST      BIT(18)
#define WRI_FLAG_SP_WDOG_CPU_RST    BIT(17)
#define WRI_FLAG_SP_WDOG_SYS_RST    BIT(16)
#define WRI_FLAG_THS_RST            BIT(9)
#define WRI_FLAG_PIN_RST            BIT(8)
#define WRI_FLAG_RTC_POR            BIT(3)
#define WRI_FLAG_VDD11_C908_POR     BIT(2)
#define WRI_FLAG_VDD11_SW_POR       BIT(1)
#define WRI_FLAG_VDD11_SP_POR       BIT(0)

u32 wr_bit[WRI_TYPE_MAX] = {WRI_FLAG_VDD11_SP_POR,      WRI_FLAG_VDD11_SW_POR,
                            WRI_FLAG_VDD11_C908_POR,    WRI_FLAG_RTC_POR,
                            WRI_FLAG_PIN_RST,           WRI_FLAG_THS_RST,
                            WRI_FLAG_SP_WDOG_SYS_RST,   WRI_FLAG_SP_WDOG_CPU_RST,
                            WRI_FLAG_SP_DM_NDM_RST,     WRI_FLAG_SP_DM_CPU_RST,
                            WRI_FLAG_CS_WDOG_SYS_RST,   WRI_FLAG_CS_WDOG_CPU_RST,
                            WRI_FLAG_CS_DM_NDM_RST,     WRI_FLAG_CS_DM_CPU_RST,
                            WRI_FLAG_SC_WDOG_SYS_RST,   WRI_FLAG_SC_WDOG_CPU_RST,
                            WRI_FLAG_SC_DM_NDM_RST,     WRI_FLAG_SC_DM_CPU_RST,
                            WRI_FLAG_SE_WDOG_SYS_RST,   WRI_FLAG_SE_WDOG_CPU_RST,
                            WRI_FLAG_SE_DM_NDM_RST};

static enum aic_warm_reset_type get_reset_type(enum aic_warm_reset_type hw)
{
    u16 hw_reason[] = {WRI_TYPE_SP_WDOG_SYS_RST, WRI_TYPE_SP_WDOG_CPU_RST,
                       WRI_TYPE_CS_WDOG_SYS_RST, WRI_TYPE_CS_WDOG_CPU_RST,
                       WRI_TYPE_SC_WDOG_SYS_RST, WRI_TYPE_SC_WDOG_CPU_RST,
                       WRI_TYPE_SE_WDOG_SYS_RST, WRI_TYPE_SE_WDOG_CPU_RST,};

    for (int i = 0; i < ARRAY_SIZE(hw_reason); i++) {
        if (hw == hw_reason[i])
            return hw_reason[i];
    }
    return WRI_TYPE_ERR;
}

static void get_hw_reboot_action(enum aic_warm_reset_type hw)
{
    switch (hw) {
    case WRI_TYPE_SP_WDOG_SYS_RST:
        printf("Reboot action: SPSS Watchdog-system-Reset\n");
        break;
    case WRI_TYPE_SP_WDOG_CPU_RST:
        printf("Reboot action: SPSS Watchdog-CPU-Reset\n");
        break;
    case WRI_TYPE_CS_WDOG_SYS_RST:
        printf("Reboot action: CSYS Watchdog-system-Reset\n");
        break;
    case WRI_TYPE_CS_WDOG_CPU_RST:
        printf("Reboot action: CSYS Watchdog-CPU-Reset\n");
        break;
    case WRI_TYPE_SC_WDOG_SYS_RST:
        printf("Reboot action: SCSS Watchdog-system-Reset\n");
        break;
    case WRI_TYPE_SC_WDOG_CPU_RST:
        printf("Reboot action: SCSS Watchdog-CPU-Reset\n");
        break;
    case WRI_TYPE_SE_WDOG_SYS_RST:
        printf("Reboot action: SESS Watchdog-system-Reset\n");
        break;
    case WRI_TYPE_SE_WDOG_CPU_RST:
        printf("Reboot action: SESS Watchdog-CPU-Reset\n");
        break;
    default:
        printf("Reboot action: Unknown(%d)\n", hw);
        break;
    }
}

static void get_sw_reboot_reason(enum aic_reboot_reason sw,
                                 enum aic_reboot_reason r)
{
    switch (sw) {
    case REBOOT_REASON_UPGRADE:
        printf("Reboot reason: Upgrade-Mode\n");
        break;
    case REBOOT_REASON_SW_LOCKUP:
        printf("Reboot reason: Software-Lockup\n");
        break;
    case REBOOT_REASON_HW_LOCKUP:
        printf("Reboot reason: Hardware-Lockup\n");
        break;
    case REBOOT_REASON_PANIC:
        printf("Reboot reason: Kernel-Panic\n");
        break;
    case REBOOT_REASON_CS_CMD_REBOOT:
        printf("Reboot reason: CSYS Command-Reboot\n");
        break;
    case REBOOT_REASON_SC_CMD_REBOOT:
        printf("Reboot reason: SCSS Command-Reboot\n");
        break;
    case REBOOT_REASON_SP_CMD_REBOOT:
        printf("Reboot reason: SPSS Command-Reboot\n");
        break;
    case REBOOT_REASON_SE_CMD_REBOOT:
        printf("Reboot reason: SESS Command-Reboot\n");
        break;
    case REBOOT_REASON_RAMDUMP:
        printf("Ramdump\n");
        break;
    default:
        printf("Unknown(%d)\n", r);
        break;
    }
}

static void get_warm_reboot_reason(enum aic_warm_reset_type hw,
                                    enum aic_reboot_reason **r)
{
    switch (hw) {
    case WRI_TYPE_RTC_POR:
        printf("Reboot reason: RTC-Power-Down\n");
        **r = REBOOT_REASON_RTC_POR;
        break;
    case WRI_TYPE_PIN_RST:
        printf("Reboot reason: Extend-Reset\n");
        **r = REBOOT_REASON_PIN_RST;
        break;
    case WRI_TYPE_THS_RST:
        printf("Reboot reason: OTP-Reset\n");
        **r = REBOOT_REASON_THS_RST;
        break;
    case WRI_TYPE_SP_DM_NDM_RST:
        printf("Reboot reason: SPSS NDM Reset\n");
        **r = REBOOT_REASON_SP_DM_NDM_RST;
        break;
    case WRI_TYPE_SP_DM_CPU_RST:
        printf("Reboot reason: SPSS CPU Reset\n");
        **r = REBOOT_REASON_SP_DM_CPU_RST;
        break;
    case WRI_TYPE_CS_DM_NDM_RST:
        printf("Reboot reason: CSYS NDM Reset\n");
        **r = REBOOT_REASON_CS_DM_NDM_RST;
        break;
    case WRI_TYPE_CS_DM_CPU_RST:
        printf("Reboot reason: CSYS CPU Reset\n");
        **r = REBOOT_REASON_CS_DM_CPU_RST;
        break;
    case WRI_TYPE_SC_DM_NDM_RST:
        printf("Reboot reason: SCSS NDM Reset\n");
        **r = REBOOT_REASON_SC_DM_NDM_RST;
        break;
    case WRI_TYPE_SC_DM_CPU_RST:
        printf("Reboot reason: SCSS CPU Reset\n");
        **r = REBOOT_REASON_SC_DM_CPU_RST;
        break;
    case WRI_TYPE_SE_DM_NDM_RST:
        printf("Reboot reason: SESS NDM Reset\n");
        **r = REBOOT_REASON_SE_DM_NDM_RST;
        break;
    default:
        printf("Unknown(%d)\n", hw);
        break;
    }
}

static int get_hw_reboot_reason(enum aic_warm_reset_type hw,
                                enum aic_reboot_reason *r, u32 sw)
{
    if (*r == REBOOT_REASON_CMD_SHUTDOWN) {
        printf("Reboot reason: Command-Poweroff\n");
        return *r;
    }

    if (*r == REBOOT_REASON_SUSPEND) {
        printf("Reboot reason: Suspend\n");
        return *r;
    }

    if (*r == REBOOT_REASON_COLD) {
        if (hw == WRI_TYPE_VDD11_SP_POR) {
            printf("Startup reason: SPSS Power-On-Reset\n");
            return sw;
        }

        if (hw == WRI_TYPE_VDD11_SW_POR) {
            printf("Startup reason: SW Power-On-Reset\n");
            return sw;
        }

        if (hw == WRI_TYPE_VDD11_C908_POR) {
            printf("Startup reason: C908 Power-On-Reset\n");
            return sw;
        }

        printf("Reboot action: Warm-Reset, reason: ");
        get_warm_reboot_reason(hw, &r);
        return *r;
    }
    return -EINVAL;
}

const struct aic_wri_ops wri_ops = {
    .wri_bit = wr_bit,
    .reset_type = get_reset_type,
    .hw_reboot_action = get_hw_reboot_action,
    .sw_reboot_reason = get_sw_reboot_reason,
    .hw_reboot_reason = get_hw_reboot_reason,
};
