#include <stdio.h>
#include <stdlib.h>

#include <ruby.h>

#include "lz77-konami.h"

#ifndef HAVE_FMEMOPEN
#define HAVE_FMEMOPEN
#include "mem/fmemopen.h"
#endif // HAVE_FMEMOPEN

#ifndef HAVE_OPEN_MEMSTREAM
#define HAVE_OPEN_MEMSTREAM
FILE *open_memstream(char **buf, size_t *len);
#endif // HAVE_OPEN_MEMSTREAM

static VALUE encode(VALUE self, VALUE str) {
  size_t ilen = RSTRING_LEN(str);
  char *buff = (char *)malloc(ilen * 2);
  size_t olen = Encode(RSTRING_LEN(str), RSTRING_PTR(str), ilen * 2, buff);
  VALUE ret = rb_str_new(buff, olen);
  free(buff);
  return ret;
}

static VALUE decode(VALUE self, VALUE str) {
  size_t olen = 0;
  char *buff = Decode(RSTRING_LEN(str), RSTRING_PTR(str), &olen);
  VALUE ret = rb_str_new(buff, olen);
  free(buff);
  return ret;
}

void Init_lz77_konami() {
  VALUE LZ77 = rb_define_module("LZ77");
  VALUE Konami = rb_define_module_under(LZ77, "Konami");
  rb_define_module_function(Konami, "encode", RUBY_METHOD_FUNC(encode), 1);
  rb_define_module_function(Konami, "decode", RUBY_METHOD_FUNC(decode), 1);
}
