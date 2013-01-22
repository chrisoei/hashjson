# use -DCYGWIN for cygwin

HASHJSON_VERSION=$(shell git describe --always --tags)

CFLAGS=-O3 -I. -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -DHASHJSON_VERSION=\"$(HASHJSON_VERSION)\" -Wno-deprecated-declarations

all: hashjson hashyaml

clean:
	rm -f *.o hashjson hashyaml

hashjson: cko_types.o hashjson.o cko_stomach.o keccak.o
	gcc $(CFLAGS) -o hashjson hashjson.c cko_types.o cko_stomach.o keccak.o -lssl -lcrypto -lz -lpthread -ldl

hashyaml: cko_types.o hashyaml.o cko_stomach.o keccak.o
	gcc $(CFLAGS) -o hashyaml hashyaml.c cko_types.o cko_stomach.o keccak.o -lssl -lcrypto -lz -lpthread -ldl
