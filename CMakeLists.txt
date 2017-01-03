cmake_minimum_required(VERSION 2.5)
project(SamyGo)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -Wall -std=c99")
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(LIBRARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)

add_subdirectory(Ambi-Light)
add_subdirectory(Virtual-Usb-Emulator)
add_subdirectory(VNC-Client)
add_subdirectory(Template)
