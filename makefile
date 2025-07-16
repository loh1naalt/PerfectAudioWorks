EXEC = PAW
CPPFLAGS = -lportaudio -lasound -pthread -lsndfile
FILES_TO_COMPILE = main.cpp AudioPharser/Portaudiohandler.cpp AudioPharser/wavpharser.cpp

$(EXEC): ${FILES_TO_COMPILE}
	g++ -o $@ $^ ${CPPFLAGS}
# wavpharser: AudioPharser/wavpharser.cpp
# 	g++ -o $@ $^
# portaudiohandler: AudioPharser/Portaudiohandler.cpp
# 	g++ -o $@ $^ 

clean:
	rm -f $(EXEC)

