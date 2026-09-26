cmake_minimum_required(VERSION 3.5)
project(fifa14_redirect_plugin)

# Pull in the required core Vita SDK functions
include("${VITASDK}/share/vita.cmake" REQUIRED)

set(VITA_MODULE_NAME "fifa14_redirect")

# Compiles using standard toolchain safety configurations
add_executable(${PROJECT_NAME} main.c taihen_stub.json)

# Links the networking infrastructure stubs
target_link_libraries(${PROJECT_NAME} SceNet_stub SceNetCtl_stub SceLibc_stub SceKernel_stub)

# Automatically converts the executable target binary into a valid .suprx layout
vita_create_self(${PROJECT_NAME}.suprx ${PROJECT_NAME} UNSIGN_PRX)
add_custom_target(suprx_target ALL DEPENDS ${PROJECT_NAME}.suprx)
