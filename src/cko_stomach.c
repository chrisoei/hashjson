#include <cko_types.h>


void cko_multidigest_init(cko_multidigest_ptr x) {
  void crcFastInit();
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

void cko_multidigest_file(cko_multidigest_ptr ctx) {
  cko_u64 nbytes;
  unsigned char* dat;
  dat=(unsigned char*) malloc(ctx->chunksize);
  if (dat==NULL) {
    printf("Unable to malloc data buffer with size=%d\n",ctx->chunksize);
    exit(2);
  }
  while((nbytes=fread(dat,1,ctx->chunksize,stdin))>0) {
    cko_multidigest_update(ctx,dat,nbytes);
    fwrite(dat, 1, nbytes, stdout);
  }
  cko_multidigest_final(ctx);
  free(dat);
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

void cko_multidigest_final(cko_multidigest_ptr x) {
  cko_u8 d_md5[18];
  cko_u8 d_sha1[20];
  cko_u8 d_sha256[32];
  cko_u8 d_sha512[64];
  cko_u8 d_ripemd160[160/8];
  cko_u8 d_sha3_256[32];
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
  x->hex_adler32[8] = '\0';
  x->hex_crc32[8] = '\0';
  x->hex_md5[32] = '\0';
  x->hex_sha1[40] = '\0';
  x->hex_sha256[64] = '\0';
  x->hex_sha512[128] = '\0';
  x->hex_ripemd160[40] = '\0';
}

// vim: expandtab nocompatible shiftwidth=2 softtabstop=2 tabstop=2
