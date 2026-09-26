cmake_minimum_required(VERSION 3.5)
project(fifa14_redirect_plugin)

# Pull in the official toolchain helper properties
include("${VITASDK}/share/vita.cmake" REQUIRED)

set(VITA_MODULE_NAME "fifa14_redirect")

# Define the build as an executable layout to satisfy toolchain packing steps
add_executable(${PROJECT_NAME} main.c)

# Explicitly pass PRX flags to override standard app binary structures
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-shared -nodefaultlibs")

# Link using standard system networking stubs
target_link_libraries(${PROJECT_NAME} SceNet_stub SceNetCtl_stub)

# Generate the PRX structures 
vita_create_self(${PROJECT_NAME}.unsigned.suprx ${PROJECT_NAME} UNSIGN_PRX)

add_custom_command(
    OUTPUT ${PROJECT_NAME}.suprx
    COMMAND vitasdk-elf2prx ${PROJECT_NAME} ${PROJECT_NAME}.suprx
    DEPENDS ${PROJECT_NAME}
)
add_custom_target(suprx_target ALL DEPENDS ${PROJECT_NAME}.suprx)
