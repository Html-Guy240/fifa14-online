cmake_minimum_required(VERSION 3.5)
project(fifa14_redirect_plugin)

# Essential Vita SDK environment macros
include("${VITASDK}/share/vita.cmake" REQUIRED)

set(VITA_MODULE_NAME "fifa14_redirect")

add_library(${PROJECT_NAME} SHARED main.c)

target_link_libraries(${PROJECT_NAME} taihen SceNet_stub SceNetCtl_stub)

vita_create_self(${PROJECT_NAME}.unsigned.suprx ${PROJECT_NAME} UNSIGN_PRX)

add_custom_command(
    OUTPUT ${PROJECT_NAME}.suprx
    COMMAND vitasdk-elf2prx ${PROJECT_NAME} ${PROJECT_NAME}.suprx
    DEPENDS ${PROJECT_NAME}
)
add_custom_target(suprx_target ALL DEPENDS ${PROJECT_NAME}.suprx)
