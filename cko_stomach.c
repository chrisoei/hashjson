#define CKO_MULTIDIGEST_VERSION	"1.2"
#include <stdio.h>
#include <global.h>
#include <md5.h>
#include <sha1.h>
#include <sha2.h>
#include <string.h>
#include <rmd160.h>

typedef struct {
  int chunksize;
  MD5_CTX md5_ctx;
  sha1_context sha1_ctx;
  sha512_ctx sha512_ctx;
  ripemd160_ctx_ptr ripemd160_ctx;
  unsigned long adler32;
  unsigned long crc32;
  unsigned long size;
} cko_multidigest_t,*cko_multidigest_ptr;


void cko_multidigest_init(cko_multidigest_ptr x) {
  void crcFastInit();

  x->chunksize=1024*1024;
  x->size=0;
  MD5Init(&(x->md5_ctx));
  sha1_starts(&(x->sha1_ctx));
  sha512_init(&(x->sha512_ctx));
  x->adler32 = 1L;
  crcFastInit(&(x->crc32));
  x->ripemd160_ctx = (ripemd160_ctx_ptr) malloc(sizeof(ripemd160_ctx_t));
  MDinit(x->ripemd160_ctx);
}

// CKODEBUG FIXIT: is unsigned int big enough?
void cko_multidigest_update(cko_multidigest_ptr x,unsigned char* s,unsigned int l) {
  unsigned long update_adler32();
  void crcFastUpdate();

  x->size += l;
  MD5Update(&(x->md5_ctx),s,l);
  sha1_update(&(x->sha1_ctx),s,l);
  sha512_update(&(x->sha512_ctx),s,l);
  x->adler32 = update_adler32(x->adler32,s,l); // note that this take int, not uint
  crcFastUpdate(&(x->crc32),s,l);
}

void cko_multidigest_final(cko_multidigest_ptr x) {
  unsigned char d_md5[16];
  unsigned char d_sha1[20];
  unsigned char d_sha512[64];
  int i;
  int len;
  static const char cb64[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

  MD5Final(d_md5,&(x->md5_ctx));
  sha1_finish(&(x->sha1_ctx),d_sha1);
  sha512_final(&(x->sha512_ctx),d_sha512);
  crcFastFinal(&(x->crc32));

  printf("Adler32: %08x",x->adler32);
  printf("\nCRC32: %08x",x->crc32);
  printf("\nMD5: ");
  for (i=0;i<16;i++) {
    printf("%02x",(int)d_md5[i]);
  }
  printf("\nMD5 base 64: ");
  for (i=0;i<16;i+=3) {
    len = 16 - i;
    if (len > 3) len = 3;
    printf("%c",cb64[d_md5[i] >> 2]);
    printf("%c",cb64[((d_md5[i] & 0x03) << 4) | ((d_md5[i+1] & 0xf0) >> 4) ]);
    printf("%c",(unsigned char) (len > 1 ? cb64[ ((d_md5[i+1] & 0x0f) << 2) | ((d_md5[i+2] & 0xc0) >> 6) ] : '='));
    printf("%c",(unsigned char) (len > 2 ? cb64[ d_md5[i+2] & 0x3f ] : '='));
  }
  printf("\nSHA1: ");
  for (i=0;i<20;i++) {
    printf("%02x",(int)d_sha1[i]);
  }
  printf("\nSHA512: ");
  for (i=0;i<64;i++) {
    printf("%02x",(int)d_sha512[i]);
  }
  printf("\nSize: %lu",x->size);
  printf("\nVersion: %s\n",CKO_MULTIDIGEST_VERSION);
  free(x->ripemd160_ctx->MDbuf);
}

void cko_multidigest_file(char* f) {
  FILE* fp;
  if (f!=NULL) {
    fp=fopen(f,"r");
  } else {
    fp=stdin;
  }
  if (!fp) {
    printf("Unable to open %s!\n",f);
    exit(1);
  }
  cko_multidigest_t m;
  cko_multidigest_init(&m);
  int nbytes;
  char* dat;
  dat=(char*) malloc(m.chunksize);
  if (dat==NULL) {
    printf("Unable to malloc data buffer with size=%d\n",m.chunksize);
    exit(2);
  }
  while((nbytes=fread(dat,1,m.chunksize,fp))>0) {
    cko_multidigest_update(&m,dat,nbytes);
  }

  cko_multidigest_final(&m);
  fclose(fp);
}

void cko_multidigest_string(char* s) {
  cko_multidigest_t m;
  cko_multidigest_init(&m);
  cko_multidigest_update(&m,s,(unsigned int)strlen(s));
  cko_multidigest_final(&m);
}

int main(int argc,char* argv[]) {
  if (argc==1) {
    cko_multidigest_file(NULL);
  }
  if (argc==2) {
    cko_multidigest_file(argv[1]);
  }
  if (argc==3) {
    if (!strcmp(argv[1],"-s")) {
      cko_multidigest_string(argv[2]);
    } else {
      printf("Usage: cko-multidigest -s <string>\n");
      printf("Usage: cko-multidigest <filename>\n");
      printf("Usage: cko-multidigest\n");
    }
  }
}
