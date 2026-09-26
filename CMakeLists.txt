cmake_minimum_required(VERSION 3.5)
project(fifa14_redirect_plugin)

# Pull in the required Vita SDK helper functions
include("${VITASDK}/share/vita.cmake" REQUIRED)

set(VITA_MODULE_NAME "fifa14_redirect")

# Create a SHARED library module for user-mode plugins
add_library(${PROJECT_NAME} SHARED main.c)

# Link against vital networking and taiHEN hooking libraries
target_link_libraries(${PROJECT_NAME} taihen SceNet_stub SceNetCtl_stub)

# Generate the PRX binaries
vita_create_self(${PROJECT_NAME}.unsigned.suprx ${PROJECT_NAME} UNSIGN_PRX)

add_custom_command(
    OUTPUT ${PROJECT_NAME}.suprx
    COMMAND vitasdk-elf2prx ${PROJECT_NAME} ${PROJECT_NAME}.suprx
    DEPENDS ${PROJECT_NAME}
)
add_custom_target(suprx_target ALL DEPENDS ${PROJECT_NAME}.suprx)
