/*
 * FIFA 14 PS Vita - Legacy Server Redirect Plugin
 * =================================================
 * Verified for FW 3.65 / VitaSDK
 */

#include <psp2/kernel/modulemgr.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/kernel/threadmgr.h>
#include <psp2/net/net.h>
#include <psp2/net/netctl.h>
#include <psp2/kernel/clib.h>
#include <taihen.h>
#include <string.h>
#include <stdio.h>

/* ------------------------------------------------------------------------
 * Configuration
 * ------------------------------------------------------------------------ */

/* Your PC's LAN IP address, running backend/proxy_server.py */
#define PROXY_IP   "192.168.1.102"

/* Must match PROXY_TCP_PORT in your proxy configuration (default 9000) */
#define PROXY_PORT 9000

/*
 * Legacy EA server IP prefixes.
 * Intercepts all traffic routed to EA's global infrastructure blocks.
 */
static const char *g_legacy_ea_ips[] = {
    "159.153.",     /* Main EA global server block (Blaze/Matchmaking) */
    "159.253.",     /* Secondary EA infrastructure block */
    "20.50.",       /* Legacy EA Origin / Account authentication services */
    NULL            /* Sentinel - marks the end of the list */
};

/* ------------------------------------------------------------------------
 * Hook bookkeeping
 * ------------------------------------------------------------------------ */

#define HOOKS_NUM 1

/*
 * Verified NID for sceNetConnect on standard 3.65 VitaSDK builds.
 */
#define SCE_NET_CONNECT_NID 0x7A4C6262

static tai_hook_ref_t g_hook_refs[HOOKS_NUM];
static SceUID g_hook_uids[HOOKS_NUM];

/* ------------------------------------------------------------------------
 * Helpers
 * ------------------------------------------------------------------------ */

static int is_legacy_ea_ip(const char *ip_str) {
    for (int i = 0; g_legacy_ea_ips[i] != NULL; i++) {
        size_t prefix_len = strlen(g_legacy_ea_ips[i]);
        if (strncmp(ip_str, g_legacy_ea_ips[i], prefix_len) == 0) {
            return 1;
        }
    }
    return 0;
}

/* ------------------------------------------------------------------------
 * Hooked function
 * ------------------------------------------------------------------------ */

static int sceNetConnect_patched(int s, SceNetSockaddr *addr, unsigned int addrlen) {
    if (addr != NULL && addr->sa_family == SCE_NET_AF_INET) {
        SceNetSockaddrIn *addr_in = (SceNetSockaddrIn *)addr;

        char ip_str[46];
        sceNetInetNtop(SCE_NET_AF_INET, &addr_in->sin_addr, ip_str, sizeof(ip_str));

        unsigned short dst_port = sceNetHtons(addr_in->sin_port);
        sceClibPrintf("[FIFA14Redirect] connect() -> %s:%u\n", ip_str, dst_port);

        if (is_legacy_ea_ip(ip_str)) {
            sceClibPrintf("[FIFA14Redirect] Legacy EA host detected (%s) - redirecting to proxy %s:%u\n",
                          ip_str, PROXY_IP, (unsigned)PROXY_PORT);

            SceNetInAddr proxy_addr;
            sceNetInetPton(SCE_NET_AF_INET, PROXY_IP, &proxy_addr);

            addr_in->sin_addr = proxy_addr;
            addr_in->sin_port = sceNetHtons(PROXY_PORT);
        }
    }

    return TAI_CONTINUE(int, g_hook_refs[0], s, addr, addrlen);
}

/* ------------------------------------------------------------------------
 * Hook install / teardown
 * ------------------------------------------------------------------------ */

static void install_hooks(void) {
    g_hook_uids[0] = taiHookFunctionImport(
        &g_hook_refs[0],
        TAI_MAIN_MODULE,      
        TAI_ANY_LIBRARY,      
        SCE_NET_CONNECT_NID,
        sceNetConnect_patched
    );

    if (g_hook_uids[0] < 0) {
        sceClibPrintf("[FIFA14Redirect] Failed to hook sceNetConnect (0x%08X)\n", g_hook_uids[0]);
    } else {
        sceClibPrintf("[FIFA14Redirect] sceNetConnect hooked successfully.\n");
    }
}

static void remove_hooks(void) {
    for (int i = 0; i < HOOKS_NUM; i++) {
        if (g_hook_uids[i] >= 0) {
            taiHookRelease(g_hook_uids[i], g_hook_refs[i]);
        }
    }
}

/* ------------------------------------------------------------------------
 * Module entry points
 * ------------------------------------------------------------------------ */

int module_start(SceSize argc, const void *args) {
    (void)argc;
    (void)args;

    sceClibPrintf("[FIFA14Redirect] Plugin starting...\n");
    install_hooks();
    sceClibPrintf("[FIFA14Redirect] Plugin ready. Proxy target: %s:%u\n", PROXY_IP, (unsigned)PROXY_PORT);

    return SCE_KERNEL_START_SUCCESS;
}

int module_stop(SceSize argc, const void *args) {
    (void)argc;
    (void)args;

    remove_hooks();
    sceClibPrintf("[FIFA14Redirect] Plugin stopped.\n");

    return SCE_KERNEL_STOP_SUCCESS;
}

/* Dummy main function to satisfy basic compiler toolchain entry-point requirements */
int main(void) {
    return 0;
}
