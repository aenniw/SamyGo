cmake_minimum_required(VERSION 3.2)
project(SDL_template)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    eventhandling.cpp
    eventhandling.h
    helloworld1.cpp
    main.cpp
    nesemu1.cpp
    nesemu1.h
    texthandler.cpp
    texthandler.h)

add_executable(SDL_template ${SOURCE_FILES})