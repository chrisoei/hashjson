# use -DCYGWIN for cygwin

CKOEI_MULTIDIGEST_VERSION=$(shell git describe --always --tags)

CFLAGS=-O3 -I. -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -DCKOEI_MULTIDIGEST_VERSION=\"$(CKOEI_MULTIDIGEST_VERSION)\"

all: ckoei-multidigest hashjson

clean:
	rm -f *.o ckoei-multidigest hashjson

ckoei-multidigest: ckoei_multidigest.o cko_stomach.o cko_types.o stardate.o
	gcc $(CFLAGS) -o ckoei-multidigest ckoei_multidigest.o cko_stomach.c stardate.o cko_types.o -lssl -lcrypto -lz -lsqlite3 -lpthread -ldl

hashjson: cko_types.o hashjson.o cko_stomach.o
	gcc $(CFLAGS) -o hashjson hashjson.c cko_types.o cko_stomach.o -lssl -lcrypto -lz -lpthread -ldl

stardate.o: stardate.c
	gcc $(CFLAGS) -c stardate.c

cko_types.o: cko_types.c
	gcc $(CFLAGS) -c cko_types.c

test: ckoei-multidigest
	./ckoei-multidigest -x test_vectors/vector1 |grep -v Version|grep -v DB: > test_vectors/vector1.test
	./ckoei-multidigest --checksum test_vectors/vector2 |grep -v Version|grep -v DB: > test_vectors/vector2.test
	./ckoei-multidigest -x test_vectors/vector2b |grep -v Version|grep -v DB: > test_vectors/vector2b.test
	./ckoei-multidigest --checksum test_vectors/vector3 |grep -v Version|grep -v DB: > test_vectors/vector3.test
	diff test_vectors/vector1.test test_vectors/vector1.reference
	diff test_vectors/vector2.test test_vectors/vector2.reference
	diff test_vectors/vector2b.test test_vectors/vector2b.reference
	diff test_vectors/vector3.test test_vectors/vector3.reference

register:
	export CKOEI_MULTIDIGEST_DB=../ckoei-multidigest.sqlite3; echo '\$\CKOEI_MULTIDIGEST_DB'

