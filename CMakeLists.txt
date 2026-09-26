cmake_minimum_required(VERSION 3.5)
project(fifa14_redirect_plugin)

# Pull in the core toolchain macros
include("${VITASDK}/share/vita.cmake" REQUIRED)

set(VITA_MODULE_NAME "fifa14_redirect")

# 1. Correctly formatted SCEDEF structure schema for vita-libs-gen
set(TAIHEN_STUB_SCEDEF "
modules:
  taihen:
    attributes: 0
    version: 1
    nid: 0x48151623
    functions:
      taiHookFunctionImport: 0x5D3AA146
      taiHookRelease: 0x44AC3691
")
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/taihen_stub.yaml "${TAIHEN_STUB_SCEDEF}")

# 2. Directly call the toolchain stub generator tool
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/libtaihen_stub.a
    COMMAND vita-libs-gen ${CMAKE_CURRENT_BINARY_DIR}/taihen_stub.yaml ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/taihen_stub.yaml
)

# 3. Create a clean, formal static library target out of the generated stub file
add_library(taihen_stub STATIC IMPORTED GLOBAL)
set_target_properties(taihen_stub PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/libtaihen_stub.a)

# 4. Standard Vita executable block targeting main.c
add_executable(${PROJECT_NAME} main.c)

# Force dependency mapping tracking so the library builds before the executable attempts linking
add_custom_target(build_taihen_stub DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/libtaihen_stub.a)
add_dependencies(${PROJECT_NAME} build_taihen_stub)

# 5. Link against the networking stubs, kernel stubs, and our fixed taihen stub target
target_link_libraries(${PROJECT_NAME} SceNet_stub SceNetCtl_stub SceLibKernel_stub taihen_stub)

# Generate the PRX structures and package them into the final plugin layout
vita_create_self(${PROJECT_NAME}.suprx ${PROJECT_NAME} UNSIGN_PRX)
add_custom_target(suprx_target ALL DEPENDS ${PROJECT_NAME}.suprx)
