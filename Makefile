CC = gcc
CFLAGS = -c -mcpu=cortex-a53 -mfloat-abi=hard -mfpu=neon-vfpv4 -mtune=cortex-a53 -O3
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
