cmake_minimum_required(VERSION 3.5)
project(fifa14_redirect_plugin)

# Pull in the core toolchain macros
include("${VITASDK}/share/vita.cmake" REQUIRED)

set(VITA_MODULE_NAME "fifa14_redirect")

# 1. Manually write the scedef configuration format to file
set(TAIHEN_STUB_SCEDEF "
taihen:
  attributes: 0
  version: 1
  nid: 0x48151623
  functions:
    taiHookFunctionImport: 0x5D3AA146
    taiHookRelease: 0x44AC3691
")
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/taihen_stub.scedef "${TAIHEN_STUB_SCEDEF}")

# 2. Directly call the toolchain stub generator tool to bypass macro argument issues
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/libtaihen_stub.a
    COMMAND vita-libs-gen ${CMAKE_CURRENT_BINARY_DIR}/taihen_stub.scedef ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/taihen_stub.scedef
)
add_custom_target(generate_taihen_stub DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/libtaihen_stub.a)

# 3. Standard Vita executable block targeting only main.c
add_executable(${PROJECT_NAME} main.c)
add_dependencies(${PROJECT_NAME} generate_taihen_stub)

# 4. Include the build directory paths so the linker can look up our generated stub library file
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(${PROJECT_NAME} SceNet_stub SceNetCtl_stub SceLibKernel_stub ${CMAKE_CURRENT_BINARY_DIR}/libtaihen_stub.a)

# Generate the PRX structures and package them into the final plugin layout
vita_create_self(${PROJECT_NAME}.suprx ${PROJECT_NAME} UNSIGN_PRX)
add_custom_target(suprx_target ALL DEPENDS ${PROJECT_NAME}.suprx)
