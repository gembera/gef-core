cmake_minimum_required(VERSION 3.21)
project(gef-core)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions("-DENABLE_MEM_PROFILE")
    add_definitions("-DENABLE_MEM_RECORD")
endif()

#add_compile_options(-Wall -Wextra -Wpedantic -Wno-unused-parameter -Wreturn-type -Wno-pointer-arith -g)
add_compile_options(-Wreturn-type)
aux_source_directory(src DIR_SRC_GEF_CORE)

add_library(gef-core STATIC ${DIR_SRC_GEF_CORE} libs/mjson/mjson.c libs/printf/printf.c)

target_include_directories(gef-core PUBLIC inc)
target_include_directories(gef-core PRIVATE libs/mjson libs/printf)

if(PROJECT_IS_TOP_LEVEL AND CMAKE_BUILD_TYPE MATCHES Debug)
    message(STATUS "${PROJECT_NAME} enables testing.")
    enable_testing()
    add_subdirectory(tests)
endif()