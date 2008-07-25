all: cko-multidigest test

cko-multidigest: sha1.o sha2.o md5c.o adler32.o crc.o rmd160.o cko_stomach.c
	gcc -O2 -I. -o cko-multidigest cko_stomach.c md5c.o sha1.o sha2.o adler32.o crc.o rmd160.o

sha1.o: sha1.c
	gcc -O2 -I. -c sha1.c

sha2.o: sha2.c
	gcc -O2 -I. -c sha2.c

md5c.o: md5c.c
	gcc -O2 -I. -c md5c.c

adler32.o: adler32.c
	gcc -O2 -I. -c adler32.c

crc.o: crc.c
	gcc -O2 -I. -c crc.c

rmd160.o: rmd160.h rmd160.c
	gcc -O2 -I. -c rmd160.c

test: cko-multidigest
	./cko-multidigest test_vectors/vector1 > test_vectors/vector1.test
	./cko-multidigest test_vectors/vector2 > test_vectors/vector2.test
	./cko-multidigest test_vectors/vector3 > test_vectors/vector3.test
	diff test_vectors/vector1.test test_vectors/vector1.reference
	diff test_vectors/vector2.test test_vectors/vector2.reference
	diff test_vectors/vector3.test test_vectors/vector3.reference
