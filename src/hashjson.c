#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cko_types.h>
#include <openssl/md5.h>
#include <openssl/ripemd.h>
#include <openssl/sha.h>
#include <zlib.h>

typedef struct {
  char* filename;
  cko_u32 chunksize;
  MD5_CTX md5_ctx;
  SHA_CTX sha1_ctx;
  SHA256_CTX sha256_ctx;
  SHA512_CTX sha512_ctx;
  RIPEMD160_CTX ripemd160_ctx;
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
} cko_multidigest_t,*cko_multidigest_ptr;

int cko_multidigest_count(cko_multidigest_ptr x);
void cko_multidigest_file(cko_multidigest_ptr ctx);

int cko_arg_match(char* x, char* s, char* l) {
  return (!strcmp(x,s)||!strcmp(x,l)) ? 1 : 0;
}

void cko_multidigest_init(cko_multidigest_ptr x) {
  void crcFastInit();
  x->filename=NULL;
  x->chunksize=1024*1024;
  x->size=0;
  MD5_Init(&(x->md5_ctx));
  SHA1_Init(&(x->sha1_ctx));
  SHA256_Init(&(x->sha256_ctx));
  SHA512_Init(&(x->sha512_ctx));
  x->adler32 = adler32(0L, Z_NULL, 0);
  x->crc32 = crc32(0L, Z_NULL, 0);
  RIPEMD160_Init(&(x->ripemd160_ctx));
}

// CKODEBUG FIXIT: is unsigned int big enough?
void cko_multidigest_update(cko_multidigest_ptr x, unsigned char* s,cko_u32 l) {
  cko_u32 update_adler32();
  void crcFastUpdate();

  x->size += l;
  MD5_Update(&(x->md5_ctx),s,l);
  SHA1_Update(&(x->sha1_ctx),s,l);
  SHA256_Update(&(x->sha256_ctx),s,l);
  SHA512_Update(&(x->sha512_ctx),s,l);
  x->adler32 = adler32(x->adler32,s,l); // note that this take int, not uint
  x->crc32 = crc32(x->crc32,s,l);
  RIPEMD160_Update(&(x->ripemd160_ctx),s,l);
}

void cko_multidigest_print_json(cko_multidigest_ptr x) {
  printf("{");
  printf("\n  \"adler32\": \"%s\",",x->hex_adler32);
  printf("\n  \"crc32\": \"%s\",",x->hex_crc32);
  printf("\n  \"md5\": \"%s\",",x->hex_md5);
  printf("\n  \"sha1\": \"%s\",",x->hex_sha1);
  printf("\n  \"sha256\": \"%s\",",x->hex_sha256);
  printf("\n  \"sha512\": \"%s\",",x->hex_sha512);
  printf("\n  \"ripemd160\": \"%s\",",x->hex_ripemd160);
  printf("\n  \"size\": \"%lu\",",(unsigned long)x->size);
  printf("\n  \"version\": \"hashjson-%s\"\n",CKOEI_MULTIDIGEST_VERSION);
  printf("}\n");
}

void cko_multidigest_final(cko_multidigest_ptr x) {
  cko_u8 d_md5[18];
  cko_u8 d_sha1[20];
  cko_u8 d_sha256[32];
  cko_u8 d_sha512[64];
  cko_u8 d_ripemd160[160/8];
  int i;
  int len;
  static const char cb64[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

  MD5_Final(d_md5,&(x->md5_ctx));
  SHA1_Final(d_sha1,&(x->sha1_ctx));
  SHA256_Final(d_sha256, &(x->sha256_ctx));
  SHA512_Final(d_sha512, &(x->sha512_ctx));
  RIPEMD160_Final(d_ripemd160, &(x->ripemd160_ctx));

  sprintf(x->hex_adler32,"%08x",x->adler32);
  sprintf(x->hex_crc32,"%08x",x->crc32);
  for (i=0;i<16;i++) {
    sprintf(x->hex_md5+2*i,"%02x",(cko_s16)d_md5[i]);
  }
  for (i=0;i<20;i++) {
    sprintf(x->hex_sha1+i*2,"%02x",(cko_s16)d_sha1[i]);
  }
  for (i=0;i<32;i++) {
    sprintf(x->hex_sha256+i*2,"%02x",(cko_s16)d_sha256[i]);
  }
  for (i=0;i<64;i++) {
    sprintf(x->hex_sha512+i*2,"%02x",(cko_s16)d_sha512[i]);
  }

  for (i=0;i<20;i++ ) {
    sprintf(x->hex_ripemd160+i*2,"%02x",(cko_s16)d_ripemd160[i]);
  }

}

void cko_multidigest_file(cko_multidigest_ptr ctx) {
  FILE* fp;
  if (ctx->filename!=NULL) {
#ifdef CYGWIN
    fp=(FILE*)fopen(ctx->filename,"r");
#else
    fp=(FILE*)fopen64(ctx->filename,"r");
#endif
  } else {
    fp=stdin;
  }
  if (!fp) {
    printf("Unable to open %s!\n",ctx->filename);
    exit(1);
  }
  cko_u64 nbytes;
  unsigned char* dat;
  dat=(unsigned char*) malloc(ctx->chunksize);
  if (dat==NULL) {
    printf("Unable to malloc data buffer with size=%d\n",ctx->chunksize);
    exit(2);
  }
  while((nbytes=fread(dat,1,ctx->chunksize,fp))>0) {
    cko_multidigest_update(ctx,dat,nbytes);
  }
  cko_multidigest_final(ctx);
  fclose(fp);
  free(dat);
}

void cko_multidigest_string(cko_multidigest_ptr ctx, unsigned char* s) {
  cko_multidigest_update(ctx,s,(cko_u32)strlen(s));
  cko_multidigest_final(ctx);
}

int main(int argc,char* argv[]) {
  cko_types_test();
  cko_multidigest_t m;
  cko_multidigest_init(&m);

  cko_multidigest_file(&m);
  cko_multidigest_print_json(&m);
  return 0;
}
