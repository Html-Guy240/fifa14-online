cmake_minimum_required(VERSION 3.5)
project(fifa14_redirect_plugin)

# Pull in the core toolchain macros
include("${VITASDK}/share/vita.cmake" REQUIRED)

set(VITA_MODULE_NAME "fifa14_redirect")

# Standard Vita executable block
add_executable(${PROJECT_NAME} main.c taihen_stub.json)

# Link against the correct, case-sensitive system libraries
target_link_libraries(${PROJECT_NAME} SceNet_stub SceNetCtl_stub SceLibKernel_stub)

# Generate the PRX structures and package them into the final plugin layout
vita_create_self(${PROJECT_NAME}.suprx ${PROJECT_NAME} UNSIGN_PRX)
add_custom_target(suprx_target ALL DEPENDS ${PROJECT_NAME}.suprx)
