CC = gcc
# Raspberry Pi users: you might want to research the best compile flags
# below. This works for RPi3. Check the docs for your compiliation setting
# otherwise.
#### EDIT THIS LINE FOR YOUR ARCHITECTURE
CFLAGS = -c -march=native -O3 -funsafe-math-optimizations
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
