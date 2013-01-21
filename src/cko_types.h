#ifndef CKO_TYPES
#define CKO_TYPES

typedef unsigned char cko_u8;
typedef unsigned short cko_u16;
typedef unsigned int cko_u32;
//typedef unsigned long cko_u64;
typedef unsigned long long cko_u64;

typedef char cko_s8;
typedef short cko_s16;
typedef int cko_s32;
//typedef long cko_s64;
typedef long long cko_s64;

void cko_types_test();

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/md5.h>
#include <openssl/ripemd.h>
#include <openssl/sha.h>
#include <zlib.h>
#include "keccak.h"

typedef struct {
  char* filename;
  cko_u32 chunksize;
  MD5_CTX md5_ctx;
  SHA_CTX sha1_ctx;
  SHA256_CTX sha256_ctx;
  SHA512_CTX sha512_ctx;
  RIPEMD160_CTX ripemd160_ctx;
  tKeccakLane sha3_256_ctx[5*5];
  cko_u32 adler32;
  cko_u32 crc32;
  cko_u64 size;
  char hex_adler32[8+1];
  char hex_crc32[8+1];
  char hex_md5[32+1];
  char hex_sha1[40+1];
  char hex_sha256[64+1];
  char hex_sha512[128+1];
  char hex_ripemd160[40+1];
  char hex_sha3_256[64+1];
} cko_multidigest_t,*cko_multidigest_ptr;

void cko_multidigest_init(cko_multidigest_ptr x);
void cko_multidigest_update(cko_multidigest_ptr x, unsigned char* s,cko_u32 l);
void cko_multidigest_final(cko_multidigest_ptr x);

#endif // CKO_TYPES
