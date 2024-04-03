/*
 * Copyright (c) 2023, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 * 
 * Authors: lv.wu <lv.wu@artinchip.com>      
 */

#include "lwip/netif.h"
#include "stdio.h"
#include "string.h"
#include "rtthread.h"
#include "lwip/dhcp.h"

static void list_ifconfig(void)
{
    struct netif *netif;

    for (netif = netif_list; netif != NULL; netif = netif->next) {
        printf("%s%d:\n",netif->name, netif->num);
        printf("    IPv4 Address   : %s\n", ip4addr_ntoa(&netif->ip_addr));
        printf("    Default Gateway: %s\n", ip4addr_ntoa(&netif->gw));
        printf("    Subnet mask    : %s\n", ip4addr_ntoa(&netif->netmask));
        printf("    MAC addr       : %02x:%02x:%02x:%02x:%02x:%02x\n",
                netif->hwaddr[0],
                netif->hwaddr[1],
                netif->hwaddr[2],
                netif->hwaddr[3],
                netif->hwaddr[4],
                netif->hwaddr[5]);
    }
}

static void ifconfig(int argc, char *argv[])
{
    struct netif *netif;
    ip4_addr_t ip;
    ip4_addr_t gw;
    ip4_addr_t netmask;

    if (argc == 1) {
        list_ifconfig();
        return;
    }

    if (argc != 5)
        goto usage;

    netif = netif_find(argv[1]);
    if (netif == NULL) {
        printf("Can't find interface name %s\n", argv[1]);
        return;
    }

    if (!(ip4addr_aton(argv[2], &ip) && \
          ip4addr_aton(argv[3], &gw) && \
          ip4addr_aton(argv[4], &netmask))) {
        printf("Error ip information\n\tip:%s, gw:%s, netmask: %s\n",
               argv[2], argv[3], argv[4]);
        return;
    }

#if LWIP_DHCP
    dhcp_stop(netif);
#endif
    netif_set_down(netif);

    netif_set_gw(netif, &gw);
    netif_set_netmask(netif, &netmask);
    netif_set_ipaddr(netif, &ip);

    netif_set_up(netif);

    return;
usage:
    printf("ifconfig [interface] [ip] [gw] [netmask]\n");
}

#ifdef RT_USING_FINSH
MSH_CMD_EXPORT(ifconfig, list/config all net information);
#endif

#if LWIP_DHCP
static void dhcpc(int argc, char *argv[])
{
    struct netif *netif;

    if (argc != 3)
        goto usage;

    netif = netif_find(argv[1]);
    if (netif == NULL) {
        printf("Can't find interface name %s\n", argv[1]);
        return;
    }

    if (!strcmp(argv[2], "start"))
        dhcp_start(netif);
    else if (!strcmp(argv[2], "stop"))
       dhcp_stop(netif);

    return;

usage:
    printf("dhcpc [interface] [start/stop]\n");
}
#ifdef RT_USING_FINSH
MSH_CMD_EXPORT(dhcpc, dhcp start or stop);
#endif
#endif /* LWIP_DHCP */
