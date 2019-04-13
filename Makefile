CC = gcc
# Raspberry Pi users: you might want to research the best compile flags
# below. This works for RPi3. Check the docs for your compiliation setting
# otherwise. The point ti sot get good hardward floating point support
# if you have it.
#### EDIT THIS LINE FOR YOUR ARCHITECTURE
CFLAGS = -c -mcpu=cortex-a53 -mfpu=neon-fp-armv8 -mtune=cortex-a53 -ftree-vectorize \
         -mfloat-abi=hard -O3 -funsafe-math-optimizations
##### DONE EDITING
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
