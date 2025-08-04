cmake_minimum_required(VERSION 3.10)
project(PAW LANGUAGES CXX)  

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
qt_standard_project_setup()


qt_add_executable(PAW 
            src/main.cpp
            src/AudioPharser/PortAudioHandler.cpp
            src/AudioPharser/libsndfiledecoder.cpp
            src/PAW_GUI/settings_paw_gui.ui
            src/PAW_GUI/settings_paw_gui.cpp
            src/PAW_GUI/main_paw_widget.ui  
            src/PAW_GUI/main_paw_widget.cpp
            
    
) 
target_link_libraries(PAW PRIVATE portaudio)
target_link_libraries(PAW PRIVATE sndfile)
target_link_libraries(PAW PRIVATE Qt6::Core Qt6::Widgets) 