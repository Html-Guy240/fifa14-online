cmake_minimum_required(VERSION 3.5)
project(fifa14_redirect_plugin)

# Essential Vita SDK environment macros
include("${VITASDK}/share/vita.cmake" REQUIRED)

set(VITA_MODULE_NAME "fifa14_redirect")

# Set position independent layout parameters to stop the dangerous relocation error
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -fPIE -Wl,-q -O2 -Wall")

# Generate target executable layout structure maps
add_executable(${PROJECT_NAME} main.c taihen_stub.json)

# Force the linker to compile a custom shared plugin layout module
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-shared -pie -nodefaultlibs")

# Link using standard system network libraries
target_link_libraries(${PROJECT_NAME} SceNet_stub SceNetCtl_stub)

# Generate the PRX binaries
vita_create_self(${PROJECT_NAME}.unsigned.suprx ${PROJECT_NAME} UNSIGN_PRX)

add_custom_command(
    OUTPUT ${PROJECT_NAME}.suprx
    COMMAND vitasdk-elf2prx ${PROJECT_NAME} ${PROJECT_NAME}.suprx
    DEPENDS ${PROJECT_NAME}
)
add_custom_target(suprx_target ALL DEPENDS ${PROJECT_NAME}.suprx)
