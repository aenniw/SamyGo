cmake_minimum_required(VERSION 2.5)
project(NESemu)

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -flax-vector-conversions -DPATH_MAX=255 -D_FILE_OFFSET_BITS=64")

set(SOURCE_FILES ${SOURCE_FILES}
        ${SOURCE_DIR}/config.h ${SOURCE_DIR}/config.c
        ${SOURCE_DIR}/filesystem.h ${SOURCE_DIR}/filesystem.c
        ${SOURCE_DIR}/filters.h ${SOURCE_DIR}/filters.c
        ${SOURCE_DIR}/font.h ${SOURCE_DIR}/font.c
        ${SOURCE_DIR}/global.h ${SOURCE_DIR}/global.c
        ${SOURCE_DIR}/nes.h ${SOURCE_DIR}/nes.c
        ${SOURCE_DIR}/nespal.h ${SOURCE_DIR}/nespal.c
        ${SOURCE_DIR}/rom.h ${SOURCE_DIR}/rom.c
        ${SOURCE_DIR}/sound.h ${SOURCE_DIR}/sound.c
        ${SOURCE_DIR}/sys.h ${SOURCE_DIR}/sys.c
        ${SOURCE_DIR}/ui.h ${SOURCE_DIR}/ui.c
        ${SOURCE_DIR}/utility.h ${SOURCE_DIR}/utility.c
        ${SOURCE_DIR}/vid.h ${SOURCE_DIR}/vid.c
        ${SOURCE_DIR}/bitvector.h
        ${SOURCE_DIR}/mapper_info.h ${SOURCE_DIR}/mapper_info.c
        ${SOURCE_DIR}/log.h ${SOURCE_DIR}/log.c
        )

if (DEBUG_BUILD)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

    add_library(M6502 OBJECT
            ${SOURCE_DIR}/M6502/M6502.h ${SOURCE_DIR}/M6502/M6502.c
            ${SOURCE_DIR}/M6502/Tables.h
            ${SOURCE_DIR}/M6502/Codes.h
            )

    find_package(Freetype REQUIRED)
    find_package(SDL REQUIRED)

    include_directories(${FREETYPE_INCLUDE_DIRS})

    add_executable(nesemu_debug ${SOURCE_DIR}/nesemu.c
            ${SOURCE_FILES}
            $<TARGET_OBJECTS:M6502>
            )
    TARGET_LINK_LIBRARIES(nesemu_debug m SDL SDL_image ${FREETYPE_LIBRARIES})
else ()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfloat-abi=softfp -mfpu=neon")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/app)

    set(CMAKE_C_COMPILER arm-linux-gnueabi-gcc)
    include_directories(BEFORE ${INCLUDE_DIR})
    add_library(loader SHARED
            ${SOURCE_DIR}/loader.c
            ${SOURCE_DIR}/log.h ${SOURCE_DIR}/log.c)
    set_target_properties(loader PROPERTIES PREFIX "")

    add_library(M6502 OBJECT
            ${SOURCE_DIR}/M6502/M6502.h ${SOURCE_DIR}/M6502/M6502.c
            ${SOURCE_DIR}/M6502/Tables.h
            ${SOURCE_DIR}/M6502/Codes.h
            )

    add_library(nesemu SHARED
            ${SOURCE_DIR}/nesemu.c
            ${SOURCE_FILES}
            $<TARGET_OBJECTS:M6502>
            )
    set_target_properties(nesemu PROPERTIES PREFIX "")
    target_link_libraries(nesemu
            ${LIBRARY_DIR}/libSDL.a
            ${LIBRARY_DIR}/libSDL_image.a
            ${LIBRARY_DIR}/libSDL_ttf.a
            ${LIBRARY_DIR}/libjpeg.a        # SDL_image dependency
            ${LIBRARY_DIR}/libpng.a         # SDL_image dependency
            ${LIBRARY_DIR}/libtiff.a        # SDL_image dependency
            ${LIBRARY_DIR}/libz.a           # SDL_image dependency
            ${LIBRARY_DIR}/libfreetype.a    # SDL_ttf dependency
            )
endif ()

