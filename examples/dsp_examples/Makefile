CC = gcc
#LDFLAGS = -lportaudio -lasound -ljack -lrt -pthread -ldclang
LDFLAGS = -lportaudio -lasound -lrt -pthread -ldclang

portaudio_example:
	$(CC) portaudio_example.c -o $@ $(LDFLAGS)

clean:
	rm portaudio_example portaudio_example.c
