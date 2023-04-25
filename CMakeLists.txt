cmake_minimum_required(VERSION 3.21)
project(gef-core)

include(CheckTypeSize)

aux_source_directory(src DIR_SRC_GEF_CORE)

add_library(gef-core STATIC ${DIR_SRC_GEF_CORE} libs/mjson/mjson.c)

option(ENABLE_MEM_PROFILE "Enable memory profiling" OFF)
option(ENABLE_MEM_RECORD "Enable memory record" OFF)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    set(ENABLE_MEM_PROFILE ON)
    set(ENABLE_MEM_RECORD ON)
endif()

configure_file(inc/glibconfig.h.inc glibconfig.h)

target_include_directories(gef-core PUBLIC inc ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(gef-core PRIVATE libs/mjson)

if(PROJECT_IS_TOP_LEVEL AND CMAKE_BUILD_TYPE MATCHES Debug)
    message(STATUS "${PROJECT_NAME} enables testing.")
    enable_testing()
    add_subdirectory(tests)
endif()