cmake_minimum_required(VERSION 2.5)
project(VNC-Client)

#set(DEBUG_BUILD true)
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(SOURCE_FILES ${SOURCE_FILES}
        ${SOURCE_DIR}/scrap.h ${SOURCE_DIR}/scrap.c
        ${SOURCE_DIR}/rfb/rfbproto.h ${SOURCE_DIR}/rfb/rfbproto.c
        ${SOURCE_DIR}/rfb/tls.h ${SOURCE_DIR}/rfb/tls_none.c
        ${SOURCE_DIR}/rfb/d3des.h ${SOURCE_DIR}/rfb/d3des.c
        ${SOURCE_DIR}/rfb/minilzo.h ${SOURCE_DIR}/rfb/minilzo.c
        ${SOURCE_DIR}/rfb/default8x16.h
        ${SOURCE_DIR}/rfb/rfbregion.h
        ${SOURCE_DIR}/rfb/rfbconfig.h
        ${SOURCE_DIR}/rfb/rfbclient.h
        ${SOURCE_DIR}/rfb/keysym.h
        ${SOURCE_DIR}/rfb/rfb.h

        ${SOURCE_DIR}/rfb/lzoconf.h
        ${SOURCE_DIR}/rfb/lzodefs.h
        ${SOURCE_DIR}/rfb/zywrletemplate.c
        ${SOURCE_DIR}/rfb/vncauth.c
        ${SOURCE_DIR}/rfb/sockets.c
        ${SOURCE_DIR}/rfb/listen.c
        ${SOURCE_DIR}/rfb/cursor.c
        ${SOURCE_DIR}/rfb/vncviewer.c
        )

if (DEBUG_BUILD)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

    find_package(ZLIB)
    find_package(JPEG)
    find_package(SDL)

    add_executable(VNCclient_debug ${SOURCE_DIR}/viewer.c
            ${SOURCE_FILES}
            )
    TARGET_LINK_LIBRARIES(VNCclient_debug SDL
            ${ZLIB_LIBRARIES}
            ${JPEG_LIBRARIES}
            )
else ()
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/app)

    set(CMAKE_C_COMPILER arm-linux-gnueabi-gcc)
    include_directories(BEFORE ${INCLUDE_DIR})

    add_library(loader SHARED
            ${SOURCE_DIR}/loader.c
            ${SOURCE_DIR}/log.h ${SOURCE_DIR}/log.c)
    set_target_properties(loader PROPERTIES PREFIX "")

    add_library(VNCviewer SHARED
            ${SOURCE_DIR}/viewer.c
            ${SOURCE_FILES}
            )
    set_target_properties(VNCviewer PROPERTIES PREFIX "")
    target_link_libraries(VNCviewer
            ${LIBRARY_DIR}/libSDL.a
            ${LIBRARY_DIR}/libz.a
            ${LIBRARY_DIR}/libjpeg.a
            )
endif ()

