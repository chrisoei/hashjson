# use -DCYGWIN for cygwin

CKOEI_MULTIDIGEST_VERSION=$(shell git describe --always --tags)

CFLAGS=-O3 -I. -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -DCKOEI_MULTIDIGEST_VERSION=\"$(CKOEI_MULTIDIGEST_VERSION)\"

all: hashjson

clean:
	rm -f *.o hashjson

hashjson: cko_types.o hashjson.o cko_stomach.o
	gcc $(CFLAGS) -o hashjson hashjson.c cko_types.o cko_stomach.o -lssl -lcrypto -lz -lpthread -ldl

cko_types.o: cko_types.c
	gcc $(CFLAGS) -c cko_types.c

