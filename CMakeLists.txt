cmake_minimum_required(VERSION 3.5)
project(fifa14_redirect_plugin)

# Pull in the core toolchain macros
include("${VITASDK}/share/vita.cmake" REQUIRED)

set(VITA_MODULE_NAME "fifa14_redirect")

# 1. Provide the exact, native structural DB format that vita-libs-gen expects
set(TAIHEN_STUB_SCEDEF "
libtaihen:
  functions:
    taiHookFunctionImport: 0x5D3AA146
    taiHookRelease: 0x44AC3691
")
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/taihen_stub.db "${TAIHEN_STUB_SCEDEF}")

# 2. Call the toolchain generator, making sure the output file matches exactly what the linker needs
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/libtaihen_stub.a
    COMMAND vita-libs-gen ${CMAKE_CURRENT_BINARY_DIR}/taihen_stub.db ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/taihen_stub.db
)

# 3. Formally register the output file as a target dependency to stop the 'No rule to make target' error
add_custom_target(build_taihen_stub DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/libtaihen_stub.a)

# 4. Standard Vita executable block targeting main.c
add_executable(${PROJECT_NAME} main.c)
add_dependencies(${PROJECT_NAME} build_taihen_stub)

# 5. Link against the networking stubs, kernel stubs, and the full absolute path of our stub file
target_link_libraries(${PROJECT_NAME} 
    SceNet_stub 
    SceNetCtl_stub 
    SceLibKernel_stub 
    ${CMAKE_CURRENT_BINARY_DIR}/libtaihen_stub.a
)

# Generate the PRX structures and package them into the final plugin layout
vita_create_self(${PROJECT_NAME}.suprx ${PROJECT_NAME} UNSIGN_PRX)
add_custom_target(suprx_target ALL DEPENDS ${PROJECT_NAME}.suprx)
