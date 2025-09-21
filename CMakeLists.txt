# project
set(NAME "wlfw")
cmake_minimum_required(VERSION 3.10)
project(${NAME} C CXX)

# cmake variable
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 26)

# src
file(GLOB_RECURSE SRC src/*.cpp)
file(GLOB_RECURSE PROTOCOL_SRC protocols/src/*.c)
add_library(${NAME} ${SRC} ${PROTOCOL_SRC})

# headers
target_include_directories(${NAME} PUBLIC include)
target_include_directories(${NAME} PUBLIC protocols/include)

find_package(PkgConfig REQUIRED)
pkg_check_modules(WAYLAND_CLIENT REQUIRED wayland-client)
target_link_libraries(${NAME} PUBLIC ${WAYLAND_CLIENT_LIBRARIES})
target_include_directories(${NAME} PUBLIC ${WAYLAND_CLIENT_INCLUDE_DIRS})
