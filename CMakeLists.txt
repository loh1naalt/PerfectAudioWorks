cmake_minimum_required(VERSION 3.10)
project(PAW LANGUAGES C CXX)


find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
qt_standard_project_setup()


option(ENABLE_SNDFILE "Enable libsndfile support" ON)
option(ENABLE_MPG123 "Enable mpg123 support" ON)
option(ENABLE_FFMPEG "Enable FFmpeg support" ON)


include_directories(src/AudioPharser)

if(ENABLE_SNDFILE)
    find_path(SNDFILE_INCLUDE_DIR sndfile.h)
    find_library(SNDFILE_LIB NAMES sndfile)
    if(SNDFILE_INCLUDE_DIR AND SNDFILE_LIB)
        message(STATUS "libsndfile enabled")
        add_definitions(-DENABLE_SNDFILE)
        include_directories(${SNDFILE_INCLUDE_DIR})
    else()
        message(WARNING "libsndfile requested but not found, disabling")
        set(ENABLE_SNDFILE OFF)
    endif()
endif()

if(ENABLE_MPG123)
    find_path(MPG123_INCLUDE_DIR mpg123.h)
    find_library(MPG123_LIB NAMES mpg123)
    if(MPG123_INCLUDE_DIR AND MPG123_LIB)
        message(STATUS "mpg123 enabled")
        add_definitions(-DENABLE_MPG123)
        include_directories(${MPG123_INCLUDE_DIR})
    else()
        message(WARNING "mpg123 requested but not found, disabling")
        set(ENABLE_MPG123 OFF)
    endif()
endif()

if(ENABLE_FFMPEG)
    find_package(FFmpeg COMPONENTS avformat avcodec avutil swresample REQUIRED)
    if(FFmpeg_FOUND)
        message(STATUS "FFmpeg enabled")
        add_definitions(-DENABLE_FFMPEG)
        include_directories(${FFmpeg_INCLUDE_DIRS})
    else()
        message(WARNING "FFmpeg requested but not found, disabling")
        set(ENABLE_FFMPEG OFF)
    endif()
endif()


find_path(PORTAUDIO_INCLUDE_DIR portaudio.h)
find_library(PORTAUDIO_LIB NAMES portaudio)
include_directories(${PORTAUDIO_INCLUDE_DIR})


set(SOURCES
    src/main.cpp
    src/AudioPharser/PortAudioHandler.cpp
    src/AudioPharser/CodecHandler.c
    src/AudioPharser/portaudio_backend.c
    src/miscellaneous/file.c
)

if(ENABLE_SNDFILE)
    list(APPEND SOURCES src/AudioPharser/libsndfiledecoder.c)
endif()

if(ENABLE_MPG123)
    list(APPEND SOURCES src/AudioPharser/mpg123decoder.c)
endif()

if(ENABLE_FFMPEG)
    list(APPEND SOURCES src/AudioPharser/ffmpegdecoder.c)
endif()

# GUI files
list(APPEND SOURCES
    src/PAW_GUI/about_paw_gui.ui
    src/PAW_GUI/about_paw_gui.cpp
    src/PAW_GUI/settings_paw_gui.ui
    src/PAW_GUI/settings_paw_gui.cpp
    src/PAW_GUI/main_paw_widget.ui
    src/PAW_GUI/main_paw_widget.cpp
)

# -----------------------------
# Build executable
# -----------------------------
qt_add_executable(PAW ${SOURCES})

target_link_libraries(PAW PRIVATE
    Qt6::Core
    Qt6::Widgets
    ${PORTAUDIO_LIB}
)

if(ENABLE_SNDFILE)
    target_link_libraries(PAW PRIVATE ${SNDFILE_LIB})
endif()
if(ENABLE_MPG123)
    target_link_libraries(PAW PRIVATE ${MPG123_LIB})
endif()
if(ENABLE_FFMPEG)
    target_link_libraries(PAW PRIVATE FFmpeg::avformat FFmpeg::avcodec FFmpeg::avutil FFmpeg::swresample)
endif()

target_compile_options(PAW PRIVATE -Wall -Wextra -O2)
