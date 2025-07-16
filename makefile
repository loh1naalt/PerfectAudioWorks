EXEC = PAW
CPPFLAGS = -lportaudio -lasound -pthread -lsndfile

$(EXEC): main.cpp AudioPharser/Portaudiohandler.cpp
	g++ -o $@ $^ ${CPPFLAGS}
wavpharser: AudioPharser/wavpharser.cpp
	g++ -o $@ $^
portaudiohandler: AudioPharser/Portaudiohandler.cpp
	g++ -o $@ $^ 

clean:
	rm -f $(EXEC)
	rm -f wavpharser
	rm -f portaudiohandler
