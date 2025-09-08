cmake_minimum_required(VERSION 3.10)
project(PAW LANGUAGES CXX C)

# Find Qt6 components
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
qt_standard_project_setup()

# External libraries
# mpg123
find_path(MPG123_INCLUDE_DIR mpg123.h)
find_library(MPG123_LIB NAMES mpg123)

# libsndfile
find_path(SNDFILE_INCLUDE_DIR sndfile.h)
find_library(SNDFILE_LIB NAMES sndfile)

# PortAudio
find_path(PORTAUDIO_INCLUDE_DIR portaudio.h)
find_library(PORTAUDIO_LIB NAMES portaudio)

# FFmpeg
find_package(FFmpeg COMPONENTS avformat avcodec avutil swresample REQUIRED)

# Include directories
include_directories(
    src/AudioPharser
    ${MPG123_INCLUDE_DIR}
    ${SNDFILE_INCLUDE_DIR}
    ${PORTAUDIO_INCLUDE_DIR}
    ${FFmpeg_INCLUDE_DIRS}
)

# Ensure C files are treated as C
set_source_files_properties(
    src/AudioPharser/libsndfiledecoder.c
    src/AudioPharser/mpg123decoder.c
    src/AudioPharser/CodecHandler.c
    src/AudioPharser/ffmpegdecoder.c
    src/AudioPharser/portaudio_backend.c
    src/miscellaneous/file.c
    PROPERTIES LANGUAGE C
)

# Add executable
qt_add_executable(PAW
    src/main.cpp
    src/AudioPharser/PortAudioHandler.cpp
    src/AudioPharser/libsndfiledecoder.c
    src/AudioPharser/mpg123decoder.c
    src/AudioPharser/CodecHandler.c
    src/AudioPharser/ffmpegdecoder.c
    src/AudioPharser/portaudio_backend.c
    src/miscellaneous/file.c
    src/PAW_GUI/about_paw_gui.ui
    src/PAW_GUI/about_paw_gui.cpp
    src/PAW_GUI/settings_paw_gui.ui
    src/PAW_GUI/settings_paw_gui.cpp
    src/PAW_GUI/main_paw_widget.ui
    src/PAW_GUI/main_paw_widget.cpp
)

# Add conditional compilation based on FFmpeg version directly to the target.
# This ensures the FFMPEG_NEW_API macro is always available during compilation.
if(FFmpeg_VERSION VERSION_GREATER_EQUAL "5.0")
    target_compile_definitions(PAW PRIVATE FFMPEG_NEW_API)
endif()

# Link libraries
target_link_libraries(PAW PRIVATE
    FFmpeg::avformat
    FFmpeg::avcodec
    FFmpeg::avutil
    FFmpeg::swresample
    ${MPG123_LIB}
    ${SNDFILE_LIB}
    ${PORTAUDIO_LIB}
    Qt6::Core
    Qt6::Widgets
)

# Optional: Set compiler warnings
target_compile_options(PAW PRIVATE -Wall -Wextra -O2)

