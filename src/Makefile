# use -DCYGWIN for cygwin
CFLAGS=-O3 -I. -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -DCKOEI_MULTIDIGEST_VERSION=\"2.5.2\"

all: ckoei-multidigest

clean:
	rm -f *.o ckoei-multidigest

ckoei-multidigest: sha1.o sha2.o md5c.o adler32.o crc.o rmd160.o cko_stomach.c cko_types.o
	gcc $(CFLAGS) -o ckoei-multidigest cko_stomach.c md5c.o sha1.o sha2.o adler32.o crc.o rmd160.o cko_types.o -lsqlite3 -lpthread -ldl

sha1.o: sha1.c
	gcc $(CFLAGS) -c sha1.c

sha2.o: sha2.c
	gcc $(CFLAGS) -c sha2.c

md5c.o: md5c.c
	gcc $(CFLAGS) -c md5c.c

adler32.o: adler32.c
	gcc $(CFLAGS) -c adler32.c

crc.o: crc.c
	gcc $(CFLAGS) -c crc.c

rmd160.o: rmd160.h rmd160.c
	gcc $(CFLAGS) -c rmd160.c

cko_types.o: cko_types.c
	gcc $(CFLAGS) -c cko_types.c

test: ckoei-multidigest
	./ckoei-multidigest -x test_vectors/vector1 |grep -v Version > test_vectors/vector1.test
	./ckoei-multidigest --checksum test_vectors/vector2 |grep -v Version > test_vectors/vector2.test
	./ckoei-multidigest -x test_vectors/vector2b |grep -v Version > test_vectors/vector2b.test
	./ckoei-multidigest --checksum test_vectors/vector3 |grep -v Version > test_vectors/vector3.test
	diff test_vectors/vector1.test test_vectors/vector1.reference
	diff test_vectors/vector2.test test_vectors/vector2.reference
	diff test_vectors/vector2b.test test_vectors/vector2b.reference
	diff test_vectors/vector3.test test_vectors/vector3.reference

