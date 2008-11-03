#define CKO_MULTIDIGEST_VERSION	"2.0"
#include <stdio.h>
#include <stdlib.h>
#include <global.h>
#include <md5.h>
#include <sha1.h>
#include <sha2.h>
#include <string.h>
#include <rmd160.h>
#include <sqlite3.h>

typedef struct {
  char* filename;
  cko_u32 chunksize;
  MD5_CTX md5_ctx;
  sha1_context sha1_ctx;
  sha512_ctx sha512_ctx;
  ripemd160_ctx_t ripemd160_ctx;
  cko_u32 adler32;
  cko_u32 crc32;
  cko_u32 size;
} cko_multidigest_t,*cko_multidigest_ptr;


void cko_multidigest_init(cko_multidigest_ptr x) {
  void crcFastInit();
  x->filename=NULL;
  x->chunksize=1024*1024;
  x->size=0;
  MD5Init(&(x->md5_ctx));
  sha1_starts(&(x->sha1_ctx));
  sha512_init(&(x->sha512_ctx));
  x->adler32 = 1L;
  crcFastInit(&(x->crc32));
  MDinit(&(x->ripemd160_ctx));
}

// CKODEBUG FIXIT: is unsigned int big enough?
void cko_multidigest_update(cko_multidigest_ptr x,unsigned char* s,cko_u32 l) {
  cko_u32 update_adler32();
  void crcFastUpdate();

  x->size += l;
  MD5Update(&(x->md5_ctx),s,l);
  sha1_update(&(x->sha1_ctx),s,l);
  sha512_update(&(x->sha512_ctx),s,l);
  x->adler32 = update_adler32(x->adler32,s,l); // note that this take int, not uint
  crcFastUpdate(&(x->crc32),s,l);
  ripemd160_update(&(x->ripemd160_ctx),s,l);
}

void cko_multidigest_final(cko_multidigest_ptr x) {
  cko_u8 d_md5[18];
  cko_u8 d_sha1[20];
  cko_u8 d_sha512[64];
  cko_u8 d_ripemd160[160/8];
  int i;
  int len;
  static const char cb64[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  char* dbfile = getenv("CKO_MULTIDIGEST_DB");
  int rc;
  sqlite3 *dbh;
  sqlite3_stmt* stmt;
  static const char* ins = "INSERT INTO checksum(filename,adler32,crc32,md5,sha1,sha512,ripemd160,size,note) VALUES(?,?,?,?,?,?,?,?,?);";
  char hex_adler32[8+1];
  char hex_crc32[8+1];
  char hex_md5[32+1];
  char hex_sha1[40+1];
  char hex_sha512[128+1];
  char hex_ripemd160[40+1];

  MD5Final(d_md5,&(x->md5_ctx));
  sha1_finish(&(x->sha1_ctx),d_sha1);
  sha512_final(&(x->sha512_ctx),d_sha512);
  crcFastFinal(&(x->crc32));

  sprintf(hex_adler32,"%08x",x->adler32);
  printf("Adler32: %s",hex_adler32);
  sprintf(hex_crc32,"%08x",x->crc32);
  printf("\nCRC32: %s",hex_crc32);
  for (i=0;i<16;i++) {
    sprintf(hex_md5+2*i,"%02x",(cko_s16)d_md5[i]);
  }
  printf("\nMD5: %s",hex_md5);
  printf("\nMD5 base 64: ");
  d_md5[16] = 0;
  d_md5[17] = 0;
  for (i=0;i<16;i+=3) {
    len = 16 - i;
    if (len > 3) len = 3;
    printf("%c",cb64[d_md5[i] >> 2]);
    printf("%c",cb64[((d_md5[i] & 0x03) << 4) | ((d_md5[i+1] & 0xf0) >> 4) ]);
    printf("%c",(unsigned char) (len > 1 ? cb64[ ((d_md5[i+1] & 0x0f) << 2) | ((d_md5[i+2] & 0xc0) >> 6) ] : '='));
    printf("%c",(unsigned char) (len > 2 ? cb64[ d_md5[i+2] & 0x3f ] : '='));
  }
  for (i=0;i<20;i++) {
    sprintf(hex_sha1+i*2,"%02x",(cko_s16)d_sha1[i]);
  }
  printf("\nSHA1: %s",hex_sha1);
  for (i=0;i<64;i++) {
    sprintf(hex_sha512+i*2,"%02x",(cko_s16)d_sha512[i]);
  }
  printf("\nSHA512: %s",hex_sha512);
  MDfinish(&(x->ripemd160_ctx),"");

  for (i=0;i<20;i+=4 ) {
    d_ripemd160[i] = x->ripemd160_ctx.MDbuf[i>>2];
    d_ripemd160[i+1] = (x->ripemd160_ctx.MDbuf[i>>2]>>8);
    d_ripemd160[i+2] = (x->ripemd160_ctx.MDbuf[i>>2]>>16);
    d_ripemd160[i+3] = (x->ripemd160_ctx.MDbuf[i>>2]>>24);
  }
  for (i=0;i<20;i++) {
    sprintf(hex_ripemd160+i*2,"%02x",d_ripemd160[i]);
  }
  printf("\nRIPEMD160: %s",hex_ripemd160);
  printf("\nSize: %lu",(unsigned long)x->size);
  printf("\nVersion: %s\n",CKO_MULTIDIGEST_VERSION);

  if ((x->filename)&&(dbfile!=NULL)) {
    if (strlen(dbfile)<1) return;
    printf("Opening %s...\n",dbfile);
    rc = sqlite3_open(dbfile,&dbh);
    if (rc) {
      fprintf(stderr,"Unable to open db.\n");
      sqlite3_close(dbh);
      exit(1);
    }
    rc = sqlite3_prepare(dbh,ins,256,&stmt,NULL);
    if (rc) {
      fprintf(stderr,"Unable to prepare statement.\n");
      sqlite3_close(dbh);
      exit(1);
    }
    rc = sqlite3_bind_text(stmt,1,x->filename,-1,SQLITE_STATIC);
    if (rc) {
      fprintf(stderr,"Unable to bind filename.\n");
      sqlite3_close(dbh);
      exit(1);
    }
    rc = sqlite3_bind_text(stmt,2,hex_adler32,-1,SQLITE_STATIC);
    if (rc) {
      fprintf(stderr,"Unable to bind adler32.\n");
      sqlite3_close(dbh);
      exit(1);
    }
    rc = sqlite3_bind_text(stmt,3,hex_crc32,-1,SQLITE_STATIC);
    if (rc) {
      fprintf(stderr,"Unable to bind crc32.\n");
      sqlite3_close(dbh);
      exit(1);
    }
    rc = sqlite3_bind_text(stmt,4,hex_md5,-1,SQLITE_STATIC);
    if (rc) {
      fprintf(stderr,"Unable to bind md5.\n");
      sqlite3_close(dbh);
      exit(1);
    }
    rc = sqlite3_bind_text(stmt,5,hex_sha1,-1,SQLITE_STATIC);
    if (rc) {
      fprintf(stderr,"Unable to bind sha1.\n");
      sqlite3_close(dbh);
      exit(1);
    }
    rc = sqlite3_bind_text(stmt,6,hex_sha512,-1,SQLITE_STATIC);
    if (rc) {
      fprintf(stderr,"Unable to bind sha512.\n");
      sqlite3_close(dbh);
      exit(1);
    }
    rc = sqlite3_bind_text(stmt,7,hex_ripemd160,-1,SQLITE_STATIC);
    if (rc) {
      fprintf(stderr,"Unable to bind ripemd160.\n");
      sqlite3_close(dbh);
      exit(1);
    }
    rc = sqlite3_bind_int(stmt,8,x->size);
    if (rc) {
      fprintf(stderr,"Unable to bind size.\n");
      sqlite3_close(dbh);
      exit(1);
    }
    rc = sqlite3_step(stmt);
    if (rc!=SQLITE_DONE) {
      fprintf(stderr,"Unable to execute db step.\n");
      sqlite3_close(dbh);
      exit(1);
    }
    rc = sqlite3_finalize(stmt);
    if (rc!=SQLITE_OK) {
      fprintf(stderr,"Unable to finalize statement.\n");
      exit(1);
    }
    rc = sqlite3_close(dbh);
    if (rc!=SQLITE_OK) {
      fprintf(stderr,"Unable to close db.\n");
      exit(1);
    }
    printf("Database closed.\n");
  }
}

void cko_multidigest_file(char* f) {
  FILE* fp;
  if (f!=NULL) {
    fp=fopen(f,"r");
    printf("Filename: %s\n",f);
  } else {
    fp=stdin;
  }
  if (!fp) {
    printf("Unable to open %s!\n",f);
    exit(1);
  }
  cko_multidigest_t m;
  cko_multidigest_init(&m);
  m.filename = f;
  cko_u32 nbytes;
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
  free(dat);
}

void cko_multidigest_string(char* s) {
  cko_multidigest_t m;
  cko_multidigest_init(&m);
  cko_multidigest_update(&m,s,(cko_u32)strlen(s));
  cko_multidigest_final(&m);
}

int main(int argc,char* argv[]) {
  cko_types_test();
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
