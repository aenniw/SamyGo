cmake_minimum_required(VERSION 2.5)
project(SamyGo)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -Wall -std=c11")

add_subdirectory(Ambi-Light)
add_subdirectory(Virtual-Usb-Emulator)
add_subdirectory(Template)
