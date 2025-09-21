# Project
# -----------------------------------------------------------------------------
set(NAME "background")
set(VERSION 0.1.0)
cmake_minimum_required(VERSION 3.10)
project(${NAME} VERSION ${VERSION} LANGUAGES CXX)

# Compiler and Build Options
# -----------------------------------------------------------------------------
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 26)

# Source Files
# -----------------------------------------------------------------------------
file(GLOB_RECURSE SRC src/*.cpp)
add_executable(${NAME} ${SRC})

string(TOUPPER "${NAME}" UPPER_NAME)
string(REPLACE "-" "_" UPPER_NAME ${UPPER_NAME})
set(VERSION_MACRO_NAME "${UPPER_NAME}_PROJECT_VERSION")
target_compile_definitions(${NAME} PRIVATE ${VERSION_MACRO_NAME}="${VERSION}")

# Dependencies
# -----------------------------------------------------------------------------
add_subdirectory(../../ wlfw)
target_link_libraries(${NAME} PRIVATE wlfw)
