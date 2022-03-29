CC = gcc
LDFLAGS = -lportaudio -lrt -pthread -ldclang

portaudio_example:
	$(CC) portaudio_example.c -o $@ $(LDFLAGS)

clean:
	rm portaudio_example portaudio_example.c
