cmake_minimum_required(VERSION 3.5)
project(fifa14_redirect_plugin)

# Essential Vita SDK environment macros
include("${VITASDK}/share/vita.cmake" REQUIRED)

set(VITA_MODULE_NAME "fifa14_redirect")

add_executable(${PROJECT_NAME} main.c)

# Force the linker to build a PRX layout instead of a regular app binary
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-shared -nodefaultlibs")

target_link_libraries(${PROJECT_NAME} taihen SceNet_stub SceNetCtl_stub)

vita_create_self(${PROJECT_NAME}.unsigned.suprx ${PROJECT_NAME} UNSIGN_PRX)

add_custom_command(
    OUTPUT ${PROJECT_NAME}.suprx
    COMMAND vitasdk-elf2prx ${PROJECT_NAME} ${PROJECT_NAME}.suprx
    DEPENDS ${PROJECT_NAME}
)
add_custom_target(suprx_target ALL DEPENDS ${PROJECT_NAME}.suprx)
