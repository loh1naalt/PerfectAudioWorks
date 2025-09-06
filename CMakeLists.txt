cmake_minimum_required(VERSION 3.10)
project(PAW LANGUAGES CXX C)  

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
qt_standard_project_setup()

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
    src/PAW_GUI/about_paw_gui.ui
    src/PAW_GUI/about_paw_gui.cpp
    src/PAW_GUI/settings_paw_gui.ui
    src/PAW_GUI/settings_paw_gui.cpp
    src/PAW_GUI/main_paw_widget.ui  
    src/PAW_GUI/main_paw_widget.cpp
)

find_library(MPG123_LIB mpg123)
target_link_libraries(PAW PRIVATE portaudio sndfile ${MPG123_LIB} Qt6::Core Qt6::Widgets)
