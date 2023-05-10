cmake_minimum_required(VERSION 3.21)
project(gef-core)

aux_source_directory(src DIR_SRC_GEF_CORE)

add_library(gef-core STATIC ${DIR_SRC_GEF_CORE} libs/mjson/mjson.c)

target_include_directories(gef-core PUBLIC inc ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(gef-core PRIVATE libs/mjson)

if(PROJECT_IS_TOP_LEVEL AND CMAKE_BUILD_TYPE MATCHES Debug)
    message(STATUS "${PROJECT_NAME} enables testing.")
    add_definitions("-DENABLE_MEM_PROFILE")
    add_definitions("-DENABLE_MEM_RECORD")
    enable_testing()
    add_subdirectory(tests)
endif()