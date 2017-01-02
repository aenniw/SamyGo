cmake_minimum_required(VERSION 2.5)
project(Template)

#set(DEBUG_BUILD true)
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

if (DEBUG_BUILD)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

    add_executable(template_debug ${SOURCE_DIR}/template.c
            ${SOURCE_DIR}/log.h ${SOURCE_DIR}/log.c)
    TARGET_LINK_LIBRARIES(template_debug SDL SDL_ttf SDL_image)
else ()
    set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../include)
    set(LIBRARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../lib)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/app)

    set(CMAKE_C_COMPILER arm-linux-gnueabi-gcc)
    include_directories(BEFORE ${INCLUDE_DIR})
    add_library(loader SHARED
            ${SOURCE_DIR}/loader.c
            ${SOURCE_DIR}/log.h ${SOURCE_DIR}/log.c)
    set_target_properties(loader PROPERTIES PREFIX "")


    add_library(template SHARED
            ${SOURCE_DIR}/template.c
            ${SOURCE_DIR}/buttons.h ${SOURCE_DIR}/buttons.c
            ${SOURCE_DIR}/log.h ${SOURCE_DIR}/log.c)
    set_target_properties(template PROPERTIES PREFIX "")
    target_link_libraries(template
            ${LIBRARY_DIR}/libSDL.a
            ${LIBRARY_DIR}/libSDL_image.a
            ${LIBRARY_DIR}/libSDL_ttf.a
            ${LIBRARY_DIR}/libjpeg.a        # SDL_image dependency
            ${LIBRARY_DIR}/libpng.a         # SDL_image dependency
            ${LIBRARY_DIR}/libtiff.a         # SDL_image dependency
            ${LIBRARY_DIR}/libz.a         # SDL_image dependency
            ${LIBRARY_DIR}/libfreetype.a    # SDL_ttf dependency
            )
endif ()

