#include <stdio.h>
#include <stdlib.h>
#include <global.h>
#include <md5.h>
#include <sha1.h>
#include <sha2.h>
#include <string.h>
#include <rmd160.h>
#include <sqlite3.h>
#include <stardate.h>

typedef struct {
  char* filename;
  cko_u32 chunksize;
  MD5_CTX md5_ctx;
  sha1_context sha1_ctx;
  sha512_ctx sha512_ctx;
  ripemd160_ctx_t ripemd160_ctx;
  cko_u32 adler32;
  cko_u32 crc32;
  cko_u64 size;
  char hex_adler32[8+1];
  char hex_crc32[8+1];
  char hex_md5[32+1];
  char hex_sha1[40+1];
  char hex_sha512[128+1];
  char hex_ripemd160[40+1];
  char* note;
} cko_multidigest_t,*cko_multidigest_ptr;

int cko_multidigest_count(cko_multidigest_ptr x);
void cko_multidigest_file(cko_multidigest_ptr ctx);
void cko_multidigest_find(cko_multidigest_ptr ctx);
void cko_multidigest_query(cko_multidigest_ptr x);
void cko_multidigest_delete(cko_multidigest_ptr x);

int cko_arg_match(char* x, char* s, char* l) {
  return (!strcmp(x,s)||!strcmp(x,l)) ? 1 : 0;
}


void cko_multidigest_init(cko_multidigest_ptr x) {
  x->note = "";
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


void cko_multidigest_print(cko_multidigest_ptr x) {
  printf("Adler32: %s",x->hex_adler32);
  printf("\nCRC32: %s",x->hex_crc32);
  printf("\nMD5: %s",x->hex_md5);
  printf("\nSHA1: %s",x->hex_sha1);
  printf("\nSHA512: %s",x->hex_sha512);
  printf("\nRIPEMD160: %s",x->hex_ripemd160);
  printf("\nSize: %lu",(unsigned long)x->size);
  printf("\nVersion: %s\n",CKOEI_MULTIDIGEST_VERSION);
  if (strlen(x->note)>0)
    printf("Note: %s\n",x->note);
}

void cko_multidigest_final(cko_multidigest_ptr x) {
  cko_u8 d_md5[18];
  cko_u8 d_sha1[20];
  cko_u8 d_sha512[64];
  cko_u8 d_ripemd160[160/8];
  int i;
  int len;
  static const char cb64[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

  MD5Final(d_md5,&(x->md5_ctx));
  sha1_finish(&(x->sha1_ctx),d_sha1);
  sha512_final(&(x->sha512_ctx),d_sha512);
  crcFastFinal(&(x->crc32));

  sprintf(x->hex_adler32,"%08x",x->adler32);
  sprintf(x->hex_crc32,"%08x",x->crc32);
  for (i=0;i<16;i++) {
    sprintf(x->hex_md5+2*i,"%02x",(cko_s16)d_md5[i]);
  }
  for (i=0;i<20;i++) {
    sprintf(x->hex_sha1+i*2,"%02x",(cko_s16)d_sha1[i]);
  }
  for (i=0;i<64;i++) {
    sprintf(x->hex_sha512+i*2,"%02x",(cko_s16)d_sha512[i]);
  }
  MDfinish(&(x->ripemd160_ctx),"");

  for (i=0;i<20;i+=4 ) {
    d_ripemd160[i] = x->ripemd160_ctx.MDbuf[i>>2];
    d_ripemd160[i+1] = (x->ripemd160_ctx.MDbuf[i>>2]>>8);
    d_ripemd160[i+2] = (x->ripemd160_ctx.MDbuf[i>>2]>>16);
    d_ripemd160[i+3] = (x->ripemd160_ctx.MDbuf[i>>2]>>24);
  }
  for (i=0;i<20;i++) {
    sprintf(x->hex_ripemd160+i*2,"%02x",d_ripemd160[i]);
  }

}

void cko_multidigest_comment(cko_multidigest_ptr x,char* cmt) {
  int rc;
  sqlite3 *dbh;
  sqlite3_stmt* stmt;
  static const char* ins = "INSERT INTO annotation(md5,sha1,comment) VALUES(?,?,?);";
  char* dbfile = getenv("CKOEI_MULTIDIGEST_DB");
  if ((x->filename)&&(dbfile!=NULL)) {
    if (strlen(dbfile)<1) return;
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
    rc = sqlite3_bind_text(stmt,1,x->hex_md5,-1,SQLITE_STATIC);
    if (rc) {
      fprintf(stderr,"Unable to bind md5.\n");
      sqlite3_close(dbh);
      exit(1);
    }
    rc = sqlite3_bind_text(stmt,2,x->hex_sha1,-1,SQLITE_STATIC);
    if (rc) {
      fprintf(stderr,"Unable to bind sha1.\n");
      sqlite3_close(dbh);
      exit(1);
    }
    rc = sqlite3_bind_text(stmt,3,cmt,-1,SQLITE_STATIC);
    if (rc) {
      fprintf(stderr,"Unable to bind comment.\n");
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
  }
}

void cko_multidigest_insert(cko_multidigest_ptr x) {
  int rc;
  sqlite3 *dbh;
  sqlite3_stmt* stmt;
  static const char* ins = "INSERT INTO checksum(filename,adler32,crc32,md5,sha1,sha512,ripemd160,size,note) VALUES(?,?,?,?,?,?,?,?,?);";
  char* dbfile = getenv("CKOEI_MULTIDIGEST_DB");

  if ((x->filename)&&(dbfile!=NULL)) {
    if (strlen(dbfile)<1) return;
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
    rc = sqlite3_bind_text(stmt,2,x->hex_adler32,-1,SQLITE_STATIC);
    if (rc) {
      fprintf(stderr,"Unable to bind adler32.\n");
      sqlite3_close(dbh);
      exit(1);
    }
    rc = sqlite3_bind_text(stmt,3,x->hex_crc32,-1,SQLITE_STATIC);
    if (rc) {
      fprintf(stderr,"Unable to bind crc32.\n");
      sqlite3_close(dbh);
      exit(1);
    }
    rc = sqlite3_bind_text(stmt,4,x->hex_md5,-1,SQLITE_STATIC);
    if (rc) {
      fprintf(stderr,"Unable to bind md5.\n");
      sqlite3_close(dbh);
      exit(1);
    }
    rc = sqlite3_bind_text(stmt,5,x->hex_sha1,-1,SQLITE_STATIC);
    if (rc) {
      fprintf(stderr,"Unable to bind sha1.\n");
      sqlite3_close(dbh);
      exit(1);
    }
    rc = sqlite3_bind_text(stmt,6,x->hex_sha512,-1,SQLITE_STATIC);
    if (rc) {
      fprintf(stderr,"Unable to bind sha512.\n");
      sqlite3_close(dbh);
      exit(1);
    }
    rc = sqlite3_bind_text(stmt,7,x->hex_ripemd160,-1,SQLITE_STATIC);
    if (rc) {
      fprintf(stderr,"Unable to bind ripemd160.\n");
      sqlite3_close(dbh);
      exit(1);
    }
    rc = sqlite3_bind_int64(stmt,8,x->size);
    if (rc) {
      fprintf(stderr,"Unable to bind size.\n");
      sqlite3_close(dbh);
      exit(1);
    }
    rc = sqlite3_bind_text(stmt,9,x->note,-1,SQLITE_STATIC);
    if (rc) {
      fprintf(stderr,"Unable to bind note.\n");
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
  char* dat;
  dat=(char*) malloc(ctx->chunksize);
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

int cko_multidigest_count(cko_multidigest_ptr x) {
  int ans = 0;
  sqlite3 *dbh;
  sqlite3_stmt* stmt;
  int rc;
  char* dbfile = getenv("CKOEI_MULTIDIGEST_DB");
  if ((x->filename)&&(dbfile!=NULL)) {
    static const char* query = "SELECT count(*) from checksum where filename=?;";
    rc = sqlite3_open(dbfile,&dbh);
    if (rc) {
      fprintf(stderr,"Unable to open db.\n");
      sqlite3_close(dbh);
      exit(1);
    }
    rc = sqlite3_prepare(dbh,query,256,&stmt,NULL);
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
    rc = sqlite3_step(stmt);
    switch(rc) {
      case SQLITE_DONE:
        printf("Weird error: Query turned up no results.\n");
        break;
      case SQLITE_ROW:
        ans = sqlite3_column_int(stmt,0);
        break;
    }
    rc = sqlite3_finalize(stmt);
  }
  return ans;
}

void cko_multidigest_delete(cko_multidigest_ptr x) {
  int rc;
  sqlite3 *dbh;
  sqlite3_stmt* stmt;
  char* dbfile = getenv("CKOEI_MULTIDIGEST_DB");
  static const char* query = "DELETE from checksum where filename=?;";

  rc = sqlite3_open(dbfile,&dbh);
  if (rc) {
    fprintf(stderr,"Unable to open db.\n");
    sqlite3_close(dbh);
    exit(1);
  }
  rc = sqlite3_prepare(dbh,query,256,&stmt,NULL);
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
  rc = sqlite3_step(stmt);
  
  if (rc!=SQLITE_DONE) {
    printf("ERROR in deletion.\n");
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
}

void cko_multidigest_find(cko_multidigest_ptr x) {
  int rc;
  sqlite3 *dbh;
  sqlite3_stmt* stmt;
  char* dbfile = getenv("CKOEI_MULTIDIGEST_DB");
  cko_multidigest_file(x);
  static const char* query = "SELECT filename from checksum where sha512=?;";
  rc = sqlite3_open(dbfile,&dbh);
  if (rc) {
    fprintf(stderr,"Unable to open db.\n");
    sqlite3_close(dbh);
    exit(1);
  }
  rc = sqlite3_prepare(dbh,query,512,&stmt,NULL);
  if (rc) {
    fprintf(stderr,"Unable to prepare statement.\n");
    sqlite3_close(dbh);
    exit(1);
  }
  rc = sqlite3_bind_text(stmt,1,x->hex_sha512,-1,SQLITE_STATIC);
  if (rc) {
    fprintf(stderr,"Unable to bind filename.\n");
    sqlite3_close(dbh);
    exit(1);
  }

  while ((rc = sqlite3_step(stmt))==SQLITE_ROW) {
      printf("%s\n",sqlite3_column_text(stmt,0));
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
}

void cko_multidigest_lookup(cko_multidigest_ptr x) {
  int rc;
  sqlite3 *dbh;
  sqlite3_stmt* stmt;
  char* dbfile = getenv("CKOEI_MULTIDIGEST_DB");
  static const char* query = "SELECT dts,comment from annotation where sha1=? ORDER BY DTS;";
  rc = sqlite3_open(dbfile,&dbh);
  if (rc) {
    fprintf(stderr,"Unable to open db.\n");
    sqlite3_close(dbh);
    exit(1);
  }
  rc = sqlite3_prepare(dbh,query,256,&stmt,NULL);
  if (rc) {
    fprintf(stderr,"Unable to prepare statement.\n");
    sqlite3_close(dbh);
    exit(1);
  }
  rc = sqlite3_bind_text(stmt,1,x->hex_sha1,-1,SQLITE_STATIC);
  if (rc) {
    fprintf(stderr,"Unable to bind sha1.\n");
    sqlite3_close(dbh);
    exit(1);
  }
  while ((rc = sqlite3_step(stmt))==SQLITE_ROW) {
      printf("Comment %0.12lf: %s\n",getStarDateFromTimestamp(sqlite3_column_text(stmt,0)),sqlite3_column_text(stmt,1));
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

}

void cko_multidigest_query(cko_multidigest_ptr x) {
  int rc;
  sqlite3 *dbh;
  sqlite3_stmt* stmt;
  char* dbfile = getenv("CKOEI_MULTIDIGEST_DB");
  static const char* query = "SELECT sha512,note,dts from checksum where filename=?;";

  rc = sqlite3_open(dbfile,&dbh);
  if (rc) {
    fprintf(stderr,"Unable to open db.\n");
    sqlite3_close(dbh);
    exit(1);
  }
  rc = sqlite3_prepare(dbh,query,256,&stmt,NULL);
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
  rc = sqlite3_step(stmt);
  
  switch(rc) {
    case SQLITE_DONE:
      printf("Query turned up no results.\n");
      break;
    case SQLITE_ROW:
      //printf("SHA512: %s\n",sqlite3_column_text(stmt,0));

      cko_multidigest_file(x);
      if (strcmp(x->hex_sha512,sqlite3_column_text(stmt,0))) {
        printf("CKOEI_ERROR: Mismatch %0.12lf",getStarDateFromTimestamp(sqlite3_column_text(stmt,2)));
      } else {
        printf("CKOEI_OK: Matches %0.12lf",getStarDateFromTimestamp(sqlite3_column_text(stmt,2)));
      }
      if (strlen(sqlite3_column_text(stmt,1))>0) {
        printf(" Note: %s", sqlite3_column_text(stmt,1));
      }
      printf("\n");
      break;
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
}

void cko_multidigest_help() {
  printf("Usage: ckoei-multidigest -a|--add <filename>\n");
  printf("       ckoei-multidigest -c|--comment <comment> <filename>\n");
  printf("       ckoei-multidigest    --delete <filename>\n");
  printf("       ckoei-multidigest -f|--find <filename>\n");
  printf("       ckoei-multidigest -h|--help\n");
  printf("       ckoei-multidigest -n|--note <note> <filename>\n");
  printf("       ckoei-multidigest -s|--string <string>\n");
  printf("       ckoei-multidigest -q|--query <filename>\n");
  printf("       ckoei-multidigest -x|--checksum <filename>\n");
  printf("       ckoei-multidigest\n");
  printf("export CKOEI_MULTIDIGEST_DB=<database filename>\n");
}

void cko_multidigest_string(cko_multidigest_ptr ctx, char* s) {
  cko_multidigest_update(ctx,s,(cko_u32)strlen(s));
  cko_multidigest_final(ctx);
}

int main(int argc,char* argv[]) {
  cko_types_test();
  cko_multidigest_t m;
  cko_multidigest_init(&m);

  if (argc==1) {
    cko_multidigest_file(&m);
    cko_multidigest_print(&m);
    return 0;
  } else if (argc==2) {
    if (cko_arg_match(argv[1],"-h","--help")) {
      cko_multidigest_help();
      return 0;
    } else {
      cko_multidigest_help();
      return 0;
    }
  } else if (argc==3) {
    if (cko_arg_match(argv[1],"-a","--add")) {
      m.filename = argv[2];
      printf("Filename: %s\n",m.filename);
      int cnt = cko_multidigest_count(&m);
      if (cnt > 0) {
        printf("Already %d entries in database.\n",cnt);
        return 0;
      }
      cko_multidigest_file(&m);
      cko_multidigest_print(&m);
      cko_multidigest_insert(&m);
      return 0;
    } else if (cko_arg_match(argv[1],"-x","--checksum")) {
      m.filename = argv[2];
      printf("Filename: %s\n",m.filename);
      cko_multidigest_file(&m);
      cko_multidigest_print(&m);
      return 0;
    } else if (!strcmp(argv[1],"--delete")) {
      m.filename = argv[2];
      cko_multidigest_delete(&m);
      return 0;
    } else if (cko_arg_match(argv[1],"-f","--find")) {
      m.filename = argv[2];
      cko_multidigest_find(&m);
      cko_multidigest_lookup(&m);
      return 0;
    } else if (cko_arg_match(argv[1],"-q","--query")) {
      m.filename = argv[2];
      printf("%s: ",m.filename);
      cko_multidigest_query(&m);
      cko_multidigest_lookup(&m);
      return 0;
    } else if (cko_arg_match(argv[1],"-s","--string")) {
      cko_multidigest_string(&m,argv[2]);
      cko_multidigest_print(&m);
      cko_multidigest_lookup(&m);
      return 0;
    } else {
      cko_multidigest_help();
      return 0;
    }
  } else if (argc==4) {
    if (cko_arg_match(argv[1],"-n","--note")) {
      m.filename = argv[3];
      int cnt = cko_multidigest_count(&m);
      if (cnt > 0) {
        printf("Already %d entries in database.\n",cnt);
        return 0;
      }
      m.note = argv[2];
      cko_multidigest_file(&m);
      cko_multidigest_print(&m);
      cko_multidigest_insert(&m);
      return 0;
    } else if (cko_arg_match(argv[1],"-c","--comment")) {
      m.filename = argv[3];
      cko_multidigest_file(&m);
      cko_multidigest_comment(&m,argv[2]);
      return 0;
    } else {
      cko_multidigest_help();
      return 0;
    }
  }
  cko_multidigest_help();
  return 0;
}
