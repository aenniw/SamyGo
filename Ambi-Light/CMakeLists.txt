cmake_minimum_required(VERSION 2.5)
project(AmbiLight)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -Wall")
set(CMAKE_C_COMPILER arm-linux-gnueabi-gcc)

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/app/)

add_library(loader SHARED
        ${SOURCE_DIR}/loader.c
        ${SOURCE_DIR}/common.h ${SOURCE_DIR}/common.c)
set_target_properties(loader PROPERTIES PREFIX "")

add_library(ambi SHARED
        ${SOURCE_DIR}/ambi.c
        ${SOURCE_DIR}/common.h ${SOURCE_DIR}/common.c
        ${SOURCE_DIR}/externs.h ${SOURCE_DIR}/buttons.h)
set_target_properties(ambi PROPERTIES PREFIX "")
