#include "cko_types.h"
#include <string.h>
#include <sqlite3.h>
#include <stardate.h>

int cko_multidigest_count(cko_multidigest_ptr x);
void cko_multidigest_file(cko_multidigest_ptr ctx);
void cko_multidigest_find(cko_multidigest_ptr ctx);
void cko_multidigest_query(cko_multidigest_ptr x);
void cko_multidigest_delete(cko_multidigest_ptr x);

int cko_arg_match(char* x, char* s, char* l) {
  return (!strcmp(x,s)||!strcmp(x,l)) ? 1 : 0;
}

char* cko_get_db_file() {
  static char buf[4096];
  FILE *sysfp = popen("git config --get ckoei.multidigest-db", "r");
  fgets(buf, 4096, sysfp);
  pclose(sysfp);
  int l = strlen(buf);
  if (l>0) {
    buf[l-1] = '\0';
    return buf;
  }
  return getenv("CKOEI_MULTIDIGEST_DB");
}

void cko_multidigest_print(cko_multidigest_ptr x) {
  char* dbfile = cko_get_db_file();
  printf("DB: %s\n", dbfile);
  printf("Adler32: %s",x->hex_adler32);
  printf("\nCRC32: %s",x->hex_crc32);
  printf("\nMD5: %s",x->hex_md5);
  printf("\nSHA1: %s",x->hex_sha1);
  printf("\nSHA256: %s",x->hex_sha256);
  printf("\nSHA512: %s",x->hex_sha512);
  printf("\nRIPEMD160: %s",x->hex_ripemd160);
  printf("\nSize: %llu",x->size);
  printf("\nVersion: %s\n",CKOEI_MULTIDIGEST_VERSION);
}

void cko_multidigest_comment(cko_multidigest_ptr x,char* cmt) {
  int rc;
  sqlite3 *dbh;
  sqlite3_stmt* stmt;
  static const char* ins = "INSERT INTO annotation(md5,sha1,sha256,comment) VALUES(?,?,?,?);";
  char* dbfile = cko_get_db_file();
  if (dbfile!=NULL) {
    if (strlen(dbfile)<1) return;
    rc = sqlite3_open(dbfile,&dbh);
    if (rc) {
      fprintf(stderr,"Unable to open db.\n");
      sqlite3_close(dbh);
      exit(1);
    }
    rc = sqlite3_prepare(dbh,ins,256,&stmt,NULL);
    if (rc) {
      fprintf(stderr,"Unable to prepare insert annotation statement.\n");
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
    rc = sqlite3_bind_text(stmt,3,x->hex_sha256,-1,SQLITE_STATIC);
    if (rc) {
      fprintf(stderr,"Unable to bind sha256.\n");
      sqlite3_close(dbh);
      exit(1);
    }
    rc = sqlite3_bind_text(stmt,4,cmt,-1,SQLITE_STATIC);
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
  } else {
      fprintf(stderr,"DB file not set.\n");
      exit(1);
  }
}

void cko_multidigest_insert(cko_multidigest_ptr x) {
  int rc;
  sqlite3 *dbh;
  sqlite3_stmt* stmt;
  static const char* ins = "INSERT INTO checksum(filename,adler32,crc32,md5,sha1,sha256,sha512,ripemd160,size) VALUES(?,?,?,?,?,?,?,?,?);";
  char* dbfile = cko_get_db_file();

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
      fprintf(stderr,"Unable to prepare insert checksum statement.\n");
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
    rc = sqlite3_bind_text(stmt,6,x->hex_sha256,-1,SQLITE_STATIC);
    if (rc) {
      fprintf(stderr,"Unable to bind sha256.\n");
      sqlite3_close(dbh);
      exit(1);
    }
    rc = sqlite3_bind_text(stmt,7,x->hex_sha512,-1,SQLITE_STATIC);
    if (rc) {
      fprintf(stderr,"Unable to bind sha512.\n");
      sqlite3_close(dbh);
      exit(1);
    }
    rc = sqlite3_bind_text(stmt,8,x->hex_ripemd160,-1,SQLITE_STATIC);
    if (rc) {
      fprintf(stderr,"Unable to bind ripemd160.\n");
      sqlite3_close(dbh);
      exit(1);
    }
    rc = sqlite3_bind_int64(stmt,9,x->size);
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
  }
}

void cko_multidigest_file(cko_multidigest_ptr ctx) {
  FILE* fp;
  if (ctx->filename!=NULL) {
    fp=(FILE*)fopen(ctx->filename,"r");
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
  char* dbfile = cko_get_db_file();
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
      fprintf(stderr,"Unable to prepare count statement.\n");
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
  char* dbfile = cko_get_db_file();
  static const char* query = "DELETE from checksum where filename=?;";

  rc = sqlite3_open(dbfile,&dbh);
  if (rc) {
    fprintf(stderr,"Unable to open db.\n");
    sqlite3_close(dbh);
    exit(1);
  }
  rc = sqlite3_prepare(dbh,query,256,&stmt,NULL);
  if (rc) {
    fprintf(stderr,"Unable to prepare delete statement.\n");
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
  char* dbfile = cko_get_db_file();
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
  char* dbfile = cko_get_db_file();
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
  char* dbfile = cko_get_db_file();
  static const char* query = "SELECT sha512,dts from checksum where filename=?;";

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
        printf("CKOEI_ERROR: Mismatch %0.12lf",getStarDateFromTimestamp(sqlite3_column_text(stmt,1)));
      } else {
        printf("CKOEI_OK: Matches %0.12lf",getStarDateFromTimestamp(sqlite3_column_text(stmt,1)));
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
  printf("\nUsage: ckoei-multidigest -a|--add <filename>\n");
  printf("       ckoei-multidigest -c|--comment <comment> <filename>\n");
  printf("       ckoei-multidigest    --delete <filename>\n");
  printf("       ckoei-multidigest -f|--find <filename>\n");
  printf("       ckoei-multidigest -h|--help\n");
  printf("       ckoei-multidigest -s|--string <string>\n");
  printf("       ckoei-multidigest -S|--string-comment <string> <comment>\n");
  printf("       ckoei-multidigest -q|--query <filename>\n");
  printf("       ckoei-multidigest -x|--checksum <filename>\n");
  printf("       ckoei-multidigest\n");
  printf("\ngit config <scope> ckoei.multidigest-db <database filename>\n");
  printf("export CKOEI_MULTIDIGEST_DB=<database filename>\n");
  printf("\nVersion: %s\n",CKOEI_MULTIDIGEST_VERSION);
  printf("DB: %s\n\n",cko_get_db_file());
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
    if (cko_arg_match(argv[1],"-c","--comment")) {
      m.filename = argv[3];
      cko_multidigest_file(&m);
      cko_multidigest_comment(&m,argv[2]);
      return 0;
    } else if (cko_arg_match(argv[1],"-S","--string-comment")) {
      cko_multidigest_string(&m,argv[2]);
      cko_multidigest_comment(&m,argv[3]);
      return 0;
    } else {
      cko_multidigest_help();
      return 0;
    }
  }
  cko_multidigest_help();
  return 0;
}
