#include <cko_types.h>

void cko_multidigest_file(cko_multidigest_ptr ctx);

void cko_multidigest_print_json(cko_multidigest_ptr x) {
  printf("{");
  printf("\n  \"adler32\": \"%s\",",x->hex_adler32);
  printf("\n  \"crc32\": \"%s\",",x->hex_crc32);
  printf("\n  \"md5\": \"%s\",",x->hex_md5);
  printf("\n  \"sha1\": \"%s\",",x->hex_sha1);
  printf("\n  \"sha256\": \"%s\",",x->hex_sha256);
  printf("\n  \"sha512\": \"%s\",",x->hex_sha512);
  printf("\n  \"ripemd160\": \"%s\",",x->hex_ripemd160);
  printf("\n  \"size\": \"%llu\",",x->size);
  printf("\n  \"version\": \"hashjson-%s\"\n",CKOEI_MULTIDIGEST_VERSION);
  printf("}\n");
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

int main(int argc,char* argv[]) {
  cko_types_test();
  cko_multidigest_t m;

  cko_multidigest_init(&m);
  if (argc == 2) {
    m.filename = argv[1];
  }
  cko_multidigest_file(&m);
  cko_multidigest_print_json(&m);
  return 0;
}
