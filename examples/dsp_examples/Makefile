CC = gcc
LDFLAGS = -lportaudio -lasound -lrt -pthread -ldclang

portaudio_example:
	$(CC) portaudio_example.c -o $@ $(LDFLAGS)

clean:
	rm _tmp_portaudio.dc portaudio_example.c portaudio_example
