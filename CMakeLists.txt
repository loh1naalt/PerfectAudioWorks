cmake_minimum_required(VERSION 3.10)
project(PAW LANGUAGES CXX C)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
qt_standard_project_setup()

# Explicitly find headers and library for mpg123
find_path(MPG123_INCLUDE_DIR mpg123.h)
find_library(MPG123_LIB NAMES mpg123)

# Explicitly find headers and library for Sndfile
find_path(SNDFILE_INCLUDE_DIR sndfile.h)
find_library(SNDFILE_LIB NAMES sndfile)

# Explicitly find headers and library for PortAudio
find_path(PORTAUDIO_INCLUDE_DIR portaudio.h)
find_library(PORTAUDIO_LIB NAMES portaudio)

# Add include directories for your headers and external libraries
include_directories(
    src/AudioPharser
    ${MPG123_INCLUDE_DIR}
    ${SNDFILE_INCLUDE_DIR}
    ${PORTAUDIO_INCLUDE_DIR}
)

set_source_files_properties(
    src/AudioPharser/libsndfiledecoder.c
    src/AudioPharser/mpg123decoder.c
    src/AudioPharser/CodecHandler.c
    PROPERTIES LANGUAGE C
)

qt_add_executable(PAW 
    src/main.cpp
    src/AudioPharser/PortAudioHandler.cpp
    src/AudioPharser/libsndfiledecoder.c
    src/AudioPharser/mpg123decoder.c
    src/AudioPharser/CodecHandler.c
    src/miscellaneous/file.c
    src/PAW_GUI/about_paw_gui.ui
    src/PAW_GUI/about_paw_gui.cpp
    src/PAW_GUI/settings_paw_gui.ui
    src/PAW_GUI/settings_paw_gui.cpp
    src/PAW_GUI/main_paw_widget.ui  
    src/PAW_GUI/main_paw_widget.cpp
)

# Link against the explicitly found libraries
target_link_libraries(PAW PRIVATE
    ${MPG123_LIB}
    ${SNDFILE_LIB}
    ${PORTAUDIO_LIB}
    Qt6::Core
    Qt6::Widgets
)