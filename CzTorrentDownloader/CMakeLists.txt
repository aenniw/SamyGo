cmake_minimum_required(VERSION 3.2)
project(CzTorrentDownloader)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(SOURCE_FILES
    commons.h
    gui.h
    gui.c
    fonts.h
    fonts.c
    cztorrent.h
    cztorrent.c
    main.c)

add_executable(CzTorrentDownloader ${SOURCE_FILES})

include_directories(${CURL_INCLUDE_DIRS})

target_link_libraries(CzTorrentDownloader  curl)
target_link_libraries(CzTorrentDownloader  SDL)
target_link_libraries(CzTorrentDownloader  SDL_image)

