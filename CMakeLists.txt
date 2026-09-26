cmake_minimum_required(VERSION 3.5)
project(fifa14_redirect_plugin)

# Pull in the core toolchain packaging macros
include("${VITASDK}/share/vita.cmake" REQUIRED)

set(VITA_MODULE_NAME "fifa14_redirect")

# Tell CMake to compile both main.c and your new assembly function bridge file
add_executable(${PROJECT_NAME} main.c taihen_asm.s)

# Link against standard networking and system kernel libraries
target_link_libraries(${PROJECT_NAME} SceNet_stub SceNetCtl_stub SceLibKernel_stub)

# Packaging instructions to generate the final .suprx layout
vita_create_self(${PROJECT_NAME}.suprx ${PROJECT_NAME} UNSIGN_PRX)
add_custom_target(suprx_target ALL DEPENDS ${PROJECT_NAME}.suprx)
