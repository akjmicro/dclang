CC = gcc
CFLAGS = -c -march=native -O3
LDFLAGS = -lm
SOURCES = main.c
OBJECTS = main.o
EXECUTABLE = dclang
LIBOBJ = libdclang.o
LIBSOOBJ = libdclang.so

.PHONY: clean lib

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

libdclang.o:
	$(CC) -c -fPIC -o $@ libdclang.c $(CFLAGS)

lib: libdclang.o
	$(CC) -shared $(LIBOBJ) -o $(LIBSOOBJ) $(LDFLAGS)

dclang: $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

clean:
	rm -rf examples/*~ *~ \
               dclang *.o *.so
