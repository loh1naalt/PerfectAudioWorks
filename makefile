EXEC = PAW

$(EXEC): main.cpp
	g++ -o $@ $^
wavpharser: AudioPharser/wavpharser.cpp
	g++ -o $@ $^
portaudiohandler: AudioPharser/Portaudiohandler.cpp
	g++ -o $@ $^ -lportaudio -lasound -pthread

clean:
	rm -f $(EXEC)
	rm -f wavpharser
	rm -f portaudiohandler
