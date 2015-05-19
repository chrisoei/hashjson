#include <cko_types.h>

void cko_multidigest_file(cko_multidigest_ptr ctx);

void cko_multidigest_print_json(cko_multidigest_ptr x) {
  fprintf(stderr, "{");
  fprintf(stderr, "\n  \"adler32\": \"%s\",",x->hex_adler32);
  fprintf(stderr, "\n  \"crc32\": \"%s\",",x->hex_crc32);
  fprintf(stderr, "\n  \"md5\": \"%s\",",x->hex_md5);
  fprintf(stderr, "\n  \"sha1\": \"%s\",",x->hex_sha1);
  fprintf(stderr, "\n  \"sha2-256\": \"%s\",",x->hex_sha256);
  fprintf(stderr, "\n  \"sha2-512\": \"%s\",",x->hex_sha512);
  fprintf(stderr, "\n  \"ripemd160\": \"%s\",",x->hex_ripemd160);
  fprintf(stderr, "\n  \"sha3-256\": \"%s\",",x->hex_sha3_256);
  fprintf(stderr, "\n  \"size\": \"%llu\",",x->size);
  fprintf(stderr, "\n  \"version\": \"hashjson-%s\"\n", HASHJSON_VERSION);
  fprintf(stderr, "}\n");
}

int main(int argc,char* argv[]) {
  cko_types_test();
  cko_multidigest_t m;

  cko_multidigest_init(&m);
  cko_multidigest_file(&m);
  cko_multidigest_print_json(&m);
  return 0;
}
