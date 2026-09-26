cmake_minimum_required(VERSION 3.5)
project(fifa14_redirect_plugin)

# Pull in the core toolchain macros
include("${VITASDK}/share/vita.cmake" REQUIRED)

# FIX: Force CMake to activate the Assembly compiler toolchain
enable_language(ASM)

set(VITA_MODULE_NAME "fifa14_redirect")

# FIX: Add the critical -Wl,-q switch so the toolchain can generate elf structures
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,-q -O2 -Wall")
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -Wl,-q")

# Standard executable tracking block targeting both resource files
add_executable(${PROJECT_NAME} main.c taihen_asm.s)

# Link against the core networking and kernel stubs
target_link_libraries(${PROJECT_NAME} SceNet_stub SceNetCtl_stub SceLibKernel_stub)

# Automatically transform the executable into your finished user-mode plugin layout
vita_create_self(${PROJECT_NAME}.suprx ${PROJECT_NAME} UNSIGN_PRX)
add_custom_target(suprx_target ALL DEPENDS ${PROJECT_NAME}.suprx)
