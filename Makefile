CC = gcc
CFLAGS = -c -march=native -O3
LDFLAGS = -lm
LDDCLANG = -ldclang
SOURCES = main.c
OBJECTS = main.o
EXECUTABLE = dclang
LIBOBJ = libdclang.o
LIBSOOBJ = libdclang.so
LIBTESTC = libtest.c
HEADER = dclang.h
PREFIX ?= /usr/local

.PHONY: clean lib libtest

default: dclang

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

dclang: $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

libdclang.o:
	$(CC) -c -fPIC -o $@ libdclang.c $(CFLAGS)

lib: libdclang.o
	$(CC) -shared $(LIBOBJ) -o $(LIBSOOBJ) $(LDFLAGS)

libtest: lib install
	$(CC) $(LIBTESTC) -o $@ $(LDDCLANG)

install: lib dclang
	sudo cp -a $(HEADER) $(PREFIX)/include
	sudo cp -a $(LIBSOOBJ) $(PREFIX)/lib
	sudo ldconfig
	sudo cp -a $(EXECUTABLE) $(PREFIX)/bin

clean:
	rm -rf examples/*~ *~ dclang libtest *.o *.so
