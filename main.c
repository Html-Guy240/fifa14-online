/*
 * FIFA 14 PS Vita - Legacy Server Redirect Plugin
 * =================================================
 * Pure C Implementation - GCC 15 Safe & Stable
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

#define PROXY_IP   "192.168.1.102"
#define PROXY_PORT 9000
#define SCE_NET_CONNECT_NID 0x7A4C6262

static const char *g_legacy_ea_ips[] = {
    "159.153.",     
    "159.253.",     
    "20.50.",       
    NULL            
};

static tai_hook_ref_t g_hook_ref;
static SceUID g_hook_uid;

/* Module information required by the toolchain to produce valid user modules */
unsigned int vita_log_mask = 0xFFFFFFFF;
int _newlib_heap_size_user = 1024 * 1024;

static int is_legacy_ea_ip(const char *ip_str) {
    for (int i = 0; g_legacy_ea_ips[i] != NULL; i++) {
        size_t prefix_len = strlen(g_legacy_ea_ips[i]);
        if (strncmp(ip_str, g_legacy_ea_ips[i], prefix_len) == 0) {
            return 1;
        }
    }
    return 0;
}

/* Patch function matching exact sceNetConnect signature */
static int sceNetConnect_patched(int s, SceNetSockaddr *addr, unsigned int addrlen) {
    if (addr != NULL && addr->sa_family == SCE_NET_AF_INET) {
        SceNetSockaddrIn *addr_in = (SceNetSockaddrIn *)addr;
        char ip_str[46]; // Fixed syntax: converted single char to string buffer
        
        sceNetInetNtop(SCE_NET_AF_INET, &addr_in->sin_addr, ip_str, sizeof(ip_str));
        unsigned short dst_port = sceNetHtons(addr_in->sin_port);
        sceClibPrintf("[FIFA14Redirect] connect() -> %s:%u\n", ip_str, dst_port);

        if (is_legacy_ea_ip(ip_str)) {
            sceClibPrintf("[FIFA14Redirect] Redirecting to proxy %s:%u\n", PROXY_IP, (unsigned)PROXY_PORT);
            SceNetInAddr proxy_addr;
            sceNetInetPton(SCE_NET_AF_INET, PROXY_IP, &proxy_addr);
            addr_in->sin_addr = proxy_addr;
            addr_in->sin_port = sceNetHtons(PROXY_PORT);
        }
    }

    /* Bypassing the broken TAI_CONTINUE macro with a clean typecast function pointer */
    struct tai_hook_layout {
        void *next;
        void *func;
        void *old;
    } *cur = (struct tai_hook_layout *)g_hook_ref;

    int (*real_connect)(int, SceNetSockaddr *, unsigned int) = 
        (cur->next == NULL) ? (int (*)(int, SceNetSockaddr *, unsigned int))cur->old 
                            : (int (*)(int, SceNetSockaddr *, unsigned int))((struct tai_hook_layout *)cur->next)->func;

    return real_connect(s, addr, addrlen);
}

int module_start(SceSize argc, const void *args) {
    (void)argc; (void)args;
    sceClibPrintf("[FIFA14Redirect] Starting...\n");

    g_hook_uid = taiHookFunctionImport(
        &g_hook_ref,
        TAI_MAIN_MODULE,      
        TAI_ANY_LIBRARY,      
        SCE_NET_CONNECT_NID,
        sceNetConnect_patched
    );

    return SCE_KERNEL_START_SUCCESS;
}

int module_stop(SceSize argc, const void *args) {
    (void)argc; (void)args;
    if (g_hook_uid >= 0) {
        taiHookRelease(g_hook_uid, g_hook_ref);
    }
    return SCE_KERNEL_STOP_SUCCESS;
}
