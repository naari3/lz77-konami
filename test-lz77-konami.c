#include "lz77-konami.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_hex(char *istr, size_t ilen) {
  for (int i = 0; i < ilen; i++) {
    printf("0x%02X ", *(istr + i) & 0x000000FF);
  }
  printf("\n");
}

void encode_test(size_t ilen, char *istr, size_t validlen, char *validstr) {
  char *encoded = (char *)malloc(ilen * 2);
  size_t olen = Encode(ilen, istr, ilen * 2, encoded);
  if (memcmp(encoded, validstr, olen) == 0) {
    printf("OK\n\n");
  } else {
    printf("invalid!\n");
    printf("expected: ");
    print_hex(validstr, validlen);
    printf("got:      ");
    print_hex(encoded, olen);
    printf("\n");
  }
  free(encoded);
}
void decode_test(size_t ilen, unsigned char *istr, size_t validlen,
                 char *validstr) {
  size_t olen = 0;

  char *decoded = Decode(ilen, istr, &olen);
  if (memcmp(decoded, validstr, olen) == 0) {
    printf("OK\n\n");
  } else {
    printf("invalid!\n");
    printf("expected: ");
    print_hex(validstr, validlen);
    printf("got:      ");
    print_hex(decoded, olen);
    printf("\n");
  }
}

int main() {
  printf("=== decode ===\n");
  printf("just put test\n");
  decode_test(8, (unsigned char *)"\x0f\x61\x61\x61\x61\x00\x00\x00", //
              4, "aaaa");
  printf("add from ref test\n");
  decode_test(
      12, (unsigned char *)"\x0f\x74\x65\x73\x74\x00\x41\x00\x85\x00\x00\x00",
      16, "testtesttesttest");
  printf("add from ref opted test\n");
  decode_test(
      15,
      (unsigned char
           *)"\x07\x61\x61\x61\x00\x30\x00\x63\x00\xc9\x01\x2d\x00\x00\x00",
      40, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
  // printf("0 padding test\n"); // TODO

  printf("=== encode ===\n");
  printf("just put test\n");
  encode_test(4, "aaaa", 8, "\x0f\x61\x61\x61\x61\x00\x00\x00");
  printf("match window test\n");
  encode_test(16, "testtesttesttest", //
              12, "\x0f\x74\x65\x73\x74\x00\x41\x00\x85\x00\x00\x00");
  printf("match window memrmem test\n");
  encode_test(40, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", //
              15,
              "\x07\x61\x61\x61\x00\x30\x00\x63\x00\xc9\x01\x2d\x00\x00\x00");

  return 0;
}
