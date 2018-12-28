CC = gcc
CFLAGS = -c -m64 -march=native -mtune=native -Ofast
# -fcall-saved-r15 -fcall-saved-r14
LDFLAGS = -lm
SOURCES = main.c
OBJECTS = main.o
EXECUTABLE = dclang

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

clean:
	rm -rf examples/*~ dclang *.o *~
