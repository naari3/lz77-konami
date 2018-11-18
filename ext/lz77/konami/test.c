#include "lz77-konami.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_hex(char *istr, size_t ilen) {
  for (int i = 0; i < ilen; i++) {
    printf("0x%02X ", *(istr + i) & 0x000000FF);
  }
  printf("\n");
}

void print_invalid(char *expstr, size_t explen, char *gotstr, size_t gotlen,
                   bool view_hex) {
  printf("invalid!\n");
  if (view_hex) {
    printf("expected: ");
    print_hex(expstr, explen);
    printf("got:      ");
    print_hex(gotstr, gotlen);
    printf("\n");
  } else {
    printf("expected size: %lu\n", explen);
    printf("got size:      %lu\n", gotlen);
  }
}

int encode_test(size_t ilen, char *istr, size_t validlen, char *validstr,
                bool view_hex) {
  int result;
  char *encoded = malloc(ilen * 2);
  size_t olen = Encode(ilen, istr, ilen * 2, encoded);
  if (memcmp(encoded, validstr, olen) == 0 && olen == validlen) {
    printf(".\n");
    result = 0;
  } else {
    print_invalid(validstr, validlen, encoded, olen, view_hex);
    result = 1;
  }
  free(encoded);
  return result;
}
int decode_test(size_t ilen, char *istr, size_t validlen, char *validstr,
                bool view_hex) {
  size_t olen = 0;

  char *decoded = Decode(ilen, istr, &olen);
  if (memcmp(decoded, validstr, olen) == 0 && olen == validlen) {
    printf(".\n");
    return 0;
  } else {
    print_invalid(validstr, validlen, decoded, olen, view_hex);
    return 1;
  }
}

int main() {
  printf("=== decode ===\n");
  printf("just put test\n");
  decode_test(8, "\x0f\x61\x61\x61\x61\x00\x00\x00", //
              4, "aaaa", true);
  printf("add from ref test\n");
  decode_test(12, "\x0f\x74\x65\x73\x74\x00\x41\x00\x85\x00\x00\x00", //
              16, "testtesttesttest", true);
  printf("add from ref opted test\n");
  decode_test(15,
              "\x07\x61\x61\x61\x00\x30\x00\x63\x00\xc9\x01\x2d\x00\x00\x00",
              40, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", true);
  printf("invalid value test\n");
  decode_test(1, "a", //
              0, "", true);
  // printf("0 padding test\n"); // TODO

  printf("=== encode ===\n");
  printf("just put test\n");
  encode_test(4, "aaaa", //
              8, "\x0f\x61\x61\x61\x61\x00\x00\x00", true);
  printf("match window test\n");
  encode_test(16, "testtesttesttest", //
              12, "\x0f\x74\x65\x73\x74\x00\x41\x00\x85\x00\x00\x00", true);
  printf("match window memrmem test\n");
  encode_test(
      40, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", //
      15, "\x07\x61\x61\x61\x00\x30\x00\x63\x00\xc9\x01\x2d\x00\x00\x00", true);
  encode_test(
      20, "12345678901234567890", //
      17,
      "\xff\x31\x32\x33\x34\x35\x36\x37\x38\x03\x39\x30\x00\xa7\x00\x00\x00",
      true);

  printf("=== file test ===\n");
  FILE *cmpr = fopen("./test_data/aaa.bin", "rb"); // compressed kbin
  fseek(cmpr, 0L, SEEK_END);
  size_t clen = ftell(cmpr);
  fseek(cmpr, 0L, SEEK_SET);
  char *cstr = malloc(clen);
  fread(cstr, sizeof(char), clen, cmpr);

  FILE *decm = fopen("./test_data/bbb.kbin", "rb"); // decompressed kbin
  fseek(decm, 0L, SEEK_END);
  size_t dlen = ftell(decm);
  fseek(decm, 0L, SEEK_SET);
  char *dstr = malloc(dlen);
  fread(dstr, sizeof(char), dlen, decm);

  decode_test(clen, cstr, dlen, dstr, false);
  encode_test(dlen, dstr, clen, cstr, false);

  fclose(cmpr);
  fclose(decm);
  free(cstr);
  free(dstr);
  return 0;
}
