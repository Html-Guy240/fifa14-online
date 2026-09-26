cmake_minimum_required(VERSION 3.5)
project(fifa14_redirect_plugin)

include("${VITASDK}/share/vita.cmake" REQUIRED)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,-q -O2 -Wall")

add_executable(${PROJECT_NAME} main.c)
target_link_libraries(${PROJECT_NAME} taihen SceNet_stub SceNetCtl_stub)

vita_create_self(${PROJECT_NAME}.suprx ${PROJECT_NAME} UNSIGN_PRX)
add_custom_target(suprx_target ALL DEPENDS ${PROJECT_NAME}.suprx)
