# use -DCYGWIN for cygwin

HASHJSON_VERSION=$(shell git describe --always --tags)

CFLAGS=-O3 -I. -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -DHASHJSON_VERSION=\"$(HASHJSON_VERSION)\"

all: hashjson

clean:
	rm -f *.o hashjson

hashjson: cko_types.o hashjson.o cko_stomach.o
	gcc $(CFLAGS) -o hashjson hashjson.c cko_types.o cko_stomach.o -lssl -lcrypto -lz -lpthread -ldl

