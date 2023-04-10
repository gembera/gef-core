cmake_minimum_required(VERSION 3.10)
project(gef-core)

include(CheckTypeSize)

aux_source_directory(src DIR_SRC)

add_library(gef-core STATIC ${DIR_SRC})

option(ENABLE_MEM_PROFILE "Enable memory profiling" OFF)
option(ENABLE_MEM_RECORD "Enable memory record" OFF)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    set(ENABLE_MEM_PROFILE ON)
    set(ENABLE_MEM_RECORD ON)
endif()

configure_file(inc/glibconfig.h.inc glibconfig.h)

target_include_directories(gef-core PUBLIC inc ${CMAKE_CURRENT_BINARY_DIR})

if(CMAKE_BUILD_TYPE MATCHES Debug)
    enable_testing()
    add_subdirectory(tests)
endif()