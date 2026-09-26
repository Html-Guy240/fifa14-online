cmake_minimum_required(VERSION 3.5)
project(fifa14_redirect_plugin)

# Pull in the required core Vita SDK environment macros
include("${VITASDK}/share/vita.cmake" REQUIRED)

set(VITA_MODULE_NAME "fifa14_redirect")

# Define target using the official Vita toolchain program compiler block
add_vita_program(${PROJECT_NAME} main.c taihen_stub.json)

# Link using standard system networking stubs (Kernel libs are linked automatically now)
target_link_libraries(${PROJECT_NAME} SceNet_stub SceNetCtl_stub)

# Automatically converts the binary target into your final user-mode plugin layout
vita_create_self(${PROJECT_NAME}.suprx ${PROJECT_NAME} UNSIGN_PRX)
add_custom_target(suprx_target ALL DEPENDS ${PROJECT_NAME}.suprx)
