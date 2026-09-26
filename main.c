/*
 * FIFA 14 PS Vita - Legacy Server Redirect Plugin
 * =================================================
 * Pure C Self-Contained Implementation
 * Safe for FW 3.65 / GCC 15 + PIC Compliant
 */

#include <psp2/kernel/modulemgr.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/kernel/threadmgr.h>
#include <psp2/net/net.h>
#include <psp2/net/netctl.h>
#include <psp2/kernel/clib.h>
#include <string.h>
#include <stdio.h>

#define PROXY_IP   "192.168.1.102"
#define PROXY_PORT 9000
#define SCE_NET_CONNECT_NID 0x7A4C6262

/* taiHEN internal types mapped natively to bypass header dependencies */
typedef void* tai_hook_ref_t;

/* Define core functional entry point definitions natively */
int taiHookFunctionImport(tai_hook_ref_t *ref_out, SceUID pid, const char *library_name, unsigned int function_nid, const void *hook_func);
int taiHookRelease(SceUID hook_uid, tai_hook_ref_t ref);

static const char *g_legacy_ea_ips[] = {
    "159.153.",     /* Main EA global server block (Blaze/Matchmaking) */
    "159.253.",     /* Secondary EA infrastructure block */
    "20.50.",       /* Legacy EA Origin / Account authentication services */
    NULL            /* Sentinel */
};

static tai_hook_ref_t g_hook_ref;
static SceUID g_hook_uid;

static int is_legacy_ea_ip(const char *ip_str) {
    for (int i = 0; g_legacy_ea_ips[i] != NULL; i++) {
        size_t prefix_len = strlen(g_legacy_ea_ips[i]);
        if (strncmp(ip_str, g_legacy_ea_ips[i], prefix_len) == 0) {
            return 1;
        }
    }
    return 0;
}

/* 
 * GCC 15 Safe Wrapper: Explicitly replicates the continue structure,
 * providing the exact parameter maps to prevent function format warnings.
 */
static int tai_continue_sceNetConnect(tai_hook_ref_t ref, int s, SceNetSockaddr *addr, unsigned int addrlen) {
    struct tai_hook_layout {
        void *next;
        void *func;
        void *old;
    } *cur = (struct tai_hook_layout *)ref;
    
    struct tai_hook_layout *next = (struct tai_hook_layout *)cur->next;

    int (*target_call)(int, SceNetSockaddr *, unsigned int) = 
        (next == NULL) ? (int (*)(int, SceNetSockaddr *, unsigned int))cur->old 
                       : (int (*)(int, SceNetSockaddr *, unsigned int))next->func;

    return target_call(s, addr, addrlen);
}

/* Hook patch function matching exact sceNetConnect signature */
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

    return tai_continue_sceNetConnect(g_hook_ref, s, addr, addrlen);
}

int module_start(SceSize argc, const void *args) {
    (void)argc; (void)args;
    sceClibPrintf("[FIFA14Redirect] Plugin starting...\n");

    /* Intercept connections routed specifically into the main game module (-1) */
    g_hook_uid = taiHookFunctionImport(
        &g_hook_ref,
        ((SceUID)-1),      
        NULL,      
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

int main(void) {
    return 0;
}
