#!/usr/bin/perl -spi.bak

s/^INSERT INTO "checksum"/INSERT INTO "checksum"(filename,adler32,crc32,md5,sha1,sha512,ripemd160,size,note,dts)/g;

