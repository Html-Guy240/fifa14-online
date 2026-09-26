cmake_minimum_required(VERSION 3.5)
project(fifa14_redirect_plugin)

# Pull in the core toolchain macros
include("${VITASDK}/share/vita.cmake" REQUIRED)

set(VITA_MODULE_NAME "fifa14_redirect")

# 1. Generate a native compile-time stub library for taiHEN inside the build folder
set(TAIHEN_STUB_SCEDEF "
# taihen stubs
info:
  name: taihen
  version: 1
  nid: 0x48151623
export:
  version: 1
  functions:
    taiHookFunctionImport: 0x5D3AA146
    taiHookRelease: 0x44AC3691
")
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/taihen_stub.scedef "${TAIHEN_STUB_SCEDEF}")
vita_create_stubs(taihen_stub taihen_stub.scedef)

# 2. Standard Vita executable block targeting only main.c
add_executable(${PROJECT_NAME} main.c)

# 3. Link against the networking stubs, kernel stubs, and our newly generated taihen library stub
target_link_libraries(${PROJECT_NAME} SceNet_stub SceNetCtl_stub SceLibKernel_stub taihen_stub)

# Generate the PRX structures and package them into the final plugin layout
vita_create_self(${PROJECT_NAME}.suprx ${PROJECT_NAME} UNSIGN_PRX)
add_custom_target(suprx_target ALL DEPENDS ${PROJECT_NAME}.suprx)
