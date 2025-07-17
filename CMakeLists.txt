cmake_minimum_required(VERSION 3.10)

project(PAW LANGUAGES CXX)  
add_executable(PAW main.cpp
            AudioPharser/Portaudiohandler.cpp 
			AudioPharser/wavpharser.cpp) 
target_link_libraries(PAW PRIVATE portaudio)
target_link_libraries(PAW PRIVATE sndfile)