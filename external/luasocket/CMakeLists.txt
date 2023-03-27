# CMakeLists.txt for building LuaSocket

cmake_minimum_required(VERSION 3.5)

# Set project name
project(luasocket C)

# Add source files
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(SRC_FILES
        ${SRC_DIR}/auxiliar.c
        ${SRC_DIR}/buffer.c
        ${SRC_DIR}/compat.c
        ${SRC_DIR}/except.c
        ${SRC_DIR}/inet.c
        ${SRC_DIR}/io.c
        ${SRC_DIR}/luasocket.c
        ${SRC_DIR}/options.c
        ${SRC_DIR}/select.c
        ${SRC_DIR}/tcp.c
        ${SRC_DIR}/timeout.c
        ${SRC_DIR}/udp.c
        ${SRC_DIR}/wsocket.c
        )

# Add header files
set(INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Create static library
add_library(luasocket STATIC ${SRC_FILES})

# Add include directories
target_include_directories(luasocket PUBLIC ${INC_DIR})