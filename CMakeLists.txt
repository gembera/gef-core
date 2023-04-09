cmake_minimum_required(VERSION 3.10)
project(gef-core)

include(CheckTypeSize)

aux_source_directory(src DIR_SRC)

add_library(core STATIC ${DIR_SRC})

configure_file(inc/glibconfig.h.inc glibconfig.h)

target_include_directories(core PUBLIC inc ${CMAKE_CURRENT_BINARY_DIR})
