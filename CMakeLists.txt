cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 20)

project(Sparrow LANGUAGES CXX C)

# platform setup
if (APPLE)
    execute_process(
            COMMAND uname -m
            OUTPUT_VARIABLE ARCH
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if (ARCH STREQUAL "arm64")
        set(IS_ARM_MAC TRUE)
    else()
        set(IS_ARM_MAC FALSE)
    endif()
else()
    set(IS_ARM_MAC FALSE)
endif()

set(IS_WIN_64 FALSE)
if(NOT "${CMAKE_GENERATOR}" MATCHES "(Win64|IA64)")
    set(IS_WIN_64 TRUE)
endif()

if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "8")
    set(IS_WIN_64 TRUE)
endif()

if(CMAKE_CL_64)
    set(IS_WIN_64 TRUE)
endif()
set(IS_WIN_64 TRUE)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
else()
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    message(STATUS "Set up ccache ...")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif()

set(ProjectRootDir "${CMAKE_CURRENT_SOURCE_DIR}")
set(SparrowResourcesDir "${ProjectRootDir}/resources")
set(SparrowSRCDir "${ProjectRootDir}/src")
set(SparrowCoreDir "${SparrowSRCDir}/core")
set(SparrowEditorDir "${SparrowSRCDir}/editor")
set(SparrowExamplesDir "${SparrowSRCDir}/examples")
set(SparrowExternalDir "${ProjectRootDir}/external")
set(SparrowLuaLib "${SparrowSRCDir}/LuaLib")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DSPW_DEBUG)
endif()

if (MSVC)
  add_compile_options(/bigobj)
else ()
  add_compile_options(-Wa,-mbig-obj)
endif ()

add_custom_target(copy_resources)
add_custom_command(TARGET copy_resources PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${SparrowResourcesDir} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources/)
        add_custom_command(TARGET copy_resources PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${SparrowLuaLib} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/LuaLib/)

add_subdirectory(external)
add_subdirectory(src)