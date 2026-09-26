# ---------------------------------------------------------------------------
# CMakeLists.txt - builds main.c into fifa14_redirect_plugin.suprx
#
# Requirements:
#   - VitaSDK installed, with the VITASDK environment variable set
#     (e.g. export VITASDK=/usr/local/vitasdk)
#   - taiHEN headers/stubs available in your VitaSDK install (they ship with
#     VitaSDK by default as of recent releases)
#
# Build:
#   mkdir build && cd build
#   cmake .. -DCMAKE_BUILD_TYPE=Release
#   make
#
# Output: build/fifa14_redirect_plugin.suprx
# ---------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.10)

if (NOT DEFINED VITASDK)
    if (DEFINED ENV{VITASDK})
        set(VITASDK $ENV{VITASDK})
    else ()
        message(FATAL_ERROR "Please set the VITASDK environment variable to your VitaSDK install path.")
    endif ()
endif ()

set(CMAKE_TOOLCHAIN_FILE "${VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")

project(fifa14_redirect_plugin C)
include("${VITASDK}/share/vita.cmake" REQUIRED)

set(VITA_APP_NAME "FIFA14 Legacy Server Redirect")
# Short, unique ID for this plugin - only relevant for VPK-style packaging;
# not required for a taiHEN-loaded .suprx, kept here for reference.
set(VITA_TITLEID "FIFA14RDR")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,-q -Wall -O3 -std=gnu99")

add_executable(fifa14_redirect_plugin
    main.c
)

target_link_libraries(fifa14_redirect_plugin
    taihen_stub
    SceNet_stub
    SceNetCtl_stub
    SceLibKernel_stub
    SceKernelModulemgr_stub
    SceKernelThreadMgr_stub
    SceSysclibForDriver_stub
)

# UNSAFE is required because this plugin hooks a system import - a "safe"
# .suprx cannot install this kind of hook.
vita_create_self(fifa14_redirect_plugin.suprx fifa14_redirect_plugin
    UNSAFE
)

# ---------------------------------------------------------------------------
# INSTALLING ON DEVICE
# ---------------------------------------------------------------------------
# 1. Copy fifa14_redirect_plugin.suprx to ux0:tai/ on your Vita.
# 2. Edit ux0:tai/config.txt and add a line under the FIFA 14 title ID's
#    section (or under *KERNEL / *MAIN to load it for every title while you
#    test), e.g.:
#
#      # FIFA 14 (replace with the real title ID for your region's copy)
#      *PCSx00000
#      ux0:tai/fifa14_redirect_plugin.suprx
#
# 3. Reboot the Vita (or reload plugins via HENkaku/taiHEN's config reload).
# ---------------------------------------------------------------------------
