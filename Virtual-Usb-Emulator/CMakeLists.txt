cmake_minimum_required(VERSION 2.5)
project(VirtualUsbEmulator)

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

set(SOURCE_FILES ${SOURCE_DIR}/main.c)
add_executable(mounter ${SOURCE_FILES})
TARGET_LINK_LIBRARIES(mounter pthread)