#include "lz77-konami.h"
#include <stdio.h>
#include <string.h>

void decode_test(size_t ilen, unsigned char *istr, char *cstr) {
  size_t olen = 0;

  char *decoded = Decode(ilen, istr, &olen);
  if (memcmp(decoded, cstr, olen) == 0) {
    printf("OK\n\n");
  } else {
    printf("invalid! decoded: %s\n\n", decoded);
  }
}

int main() {
  printf("=== decode ===\n");
  printf("just put test\n");
  decode_test(8, (unsigned char *)"\x0f\x61\x61\x61\x61\x00\x00\x00", "aaaa");
  printf("add from ref test\n");
  decode_test(
      12, (unsigned char *)"\x0f\x74\x65\x73\x74\x00\x41\x00\x85\x00\x00\x00",
      "testtesttesttest");
  printf("add from ref opted test\n");
  decode_test(
      15,
      (unsigned char
           *)"\x07\x61\x61\x61\x00\x30\x00\x63\x00\xc9\x01\x2d\x00\x00\x00",
      "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
  // printf("0 padding test\n"); // TODO
  return 0;
}
