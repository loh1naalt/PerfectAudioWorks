EXEC = PAW

$(EXEC): main.cpp
	g++ -o $@ $^
wavpharser: AudioPharser/wavpharser.cpp
	g++ -o $@ $^

clean:
	rm -f $(EXEC)
	rm -f wavpharser
