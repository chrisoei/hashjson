#include <cko_types.h>

void cko_multidigest_file(cko_multidigest_ptr ctx);

void cko_multidigest_print_yaml(cko_multidigest_ptr x) {
  printf("adler32: '%s'\n",x->hex_adler32);
  printf("crc32: '%s'\n",x->hex_crc32);
  printf("md5: '%s'\n",x->hex_md5);
  printf("sha1: '%s'\n",x->hex_sha1);
  printf("sha2-256: '%s'\n",x->hex_sha256);
  printf("sha2-512: '%s'\n",x->hex_sha512);
  printf("ripemd160: '%s'\n",x->hex_ripemd160);
  printf("sha3-256: '%s'\n",x->hex_sha3_256);
  printf("size: '%llu'\n",x->size);
  printf("version: 'hashyaml-%s'\n", HASHJSON_VERSION);
}

int main(int argc,char* argv[]) {
  cko_types_test();
  cko_multidigest_t m;

  cko_multidigest_init(&m);
  if (argc == 2) {
    m.filename = argv[1];
  }
  cko_multidigest_file(&m);
  cko_multidigest_print_yaml(&m);
  return 0;
}
