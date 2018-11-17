#include "lz77-konami.h"

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef HAVE_FMEMOPEN
#define HAVE_FMEMOPEN
#include "fmemopen.h"
#endif // HAVE_FMEMOPEN

#ifndef HAVE_OPEN_MEMSTREAM
#define HAVE_OPEN_MEMSTREAM
#include "open_memstream.h"
#endif // HAVE_OPEN_MEMSTREAM

void *memrmem(const void *v, size_t size, const void *pat, size_t patsize) {
  const char *p = "";

  assert(v != NULL);
  assert(pat != NULL);

  /* Special cases */
  if (size < patsize)
    return NULL;
  if (patsize == 0)
    return (void *)v;

  /* xxx - this will make p < v if pattern not found; potential
     portability problem */
  for (p = v, p += size - patsize; size >= patsize; --p, --size)
    if (memcmp(p, pat, patsize) == 0)
      return (void *)p;
  return NULL;
}

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define N 4096 /* size of ring buffer */
#define F 18   /* upper limit for match_length */

const size_t WINDOW_SIZE = 0x1000;
const size_t WINDOW_MASK = WINDOW_SIZE - 1;
const size_t THRESHOLD = 3;
const size_t INPLACE_THRESHOLD = 0xA;
const size_t LOOK_RANGE = 0x200;
const size_t MAX_LEN = 0xF + THRESHOLD;

// -1が入ってたら無かったと思うこと
typedef struct Window {
  int offset;
  int length;
} Window;

#define _get(c)                                                                \
  if (!ilen) {                                                                 \
    c = EOF;                                                                   \
    break;                                                                     \
  }                                                                            \
  c = *istr;                                                                   \
  ++istr;                                                                      \
  --ilen

#define _put(c)                                                                \
  *ostr = c;                                                                   \
  ++ostr;                                                                      \
  ++olen

// #define _put_buf(c)                                                            \
//   *buf = c;                                                                    \
//   ++buf;                                                                       \
//   ++buflen

#define _put_buf(c)                                                            \
  if (buflen == buf_limit) {                                                   \
    buf_limit *= 2;                                                            \
    ostr = realloc(buf, buf_limit);                                            \
  }                                                                            \
  *buf = c;                                                                    \
  ++buf;                                                                       \
  ++buflen

// Find the longest match for the string starting at offset
//   in the preceeding data
Window match_window(char *istr, size_t ilen, size_t offset) {
  size_t window_start = MAX(offset - WINDOW_MASK, 0);
  size_t window_end, idx, code_offset, code_len, str_to_find_len, in_data_len;
  char *ret, *str_to_find, *in_data;

  Window window = {-1, -1};

  for (size_t i = MAX_LEN; i > THRESHOLD - 1; i--) {
    window_end = MIN(offset + i, ilen);
    // we've not got enough data left for a meaningful result
    if (window_end - offset < THRESHOLD) {
      return window;
    }

    str_to_find_len = window_end - offset;
    str_to_find = malloc(str_to_find_len + 1);
    for (size_t j = 0; j < str_to_find_len; j++) {
      *(str_to_find + j) = *(istr + j + offset);
    }
    *(str_to_find + str_to_find_len) = 0; // for debug
    in_data_len = window_end - window_start - i;
    in_data = malloc(in_data_len);
    for (size_t j = 0; j < in_data_len; j++) {
      *(in_data + j) = *(istr + j + window_start);
    }
    ret = memrmem(in_data, in_data_len, str_to_find, str_to_find_len);
    free(str_to_find);
    free(in_data);

    if (ret != NULL) {
      idx = ret - in_data + window_start;
      code_offset = offset - idx;
      code_len = window_end - offset;
      window.offset = offset - idx;
      window.length = window_end - offset;
      return window;
    }
  }
  return window;
}

size_t Encode(size_t ilen_, char *istr_, size_t olen, char *ostr) {
  char *istr = malloc(ilen_ + WINDOW_SIZE);
  for (size_t i = 0; i < WINDOW_SIZE; i++)
    *(istr + i) = 0;
  for (size_t i = 0; i < ilen_; i++)
    *(istr + i + WINDOW_SIZE) = *(istr_ + i);
  size_t ilen = ilen_ + WINDOW_SIZE;
  size_t current_pos = WINDOW_SIZE;
  char *istr_start = istr;
  Window match;
  size_t win_pos, win_length;
  unsigned short win_info;

  olen = 0;

  size_t bit = 0;

  size_t flag_byte;
  size_t buf_size = 33;
  size_t buf_limit = buf_size;
  size_t buflen = 0;
  char *buf = malloc(buf_size);
  for (size_t i = 0; i < buf_size; i++)
    *(buf + i) = 0;
  char *buf_start = buf;

  while (current_pos < ilen) {
    flag_byte = 0;
    for (size_t i = 0; i < 8; i++) {
      if (current_pos >= ilen) {
        bit = 0;
      } else {
        match = match_window(istr, ilen, current_pos);
        if (match.offset != -1) {
          win_pos = match.offset;
          win_length = match.length;
          win_info = (win_pos << 4) | ((win_length - THRESHOLD) & 0x0F);
          _put_buf((win_info >> 8) & 0xFF);
          _put_buf((char)win_info);
          bit = 0;
          current_pos += win_length;
        } else {
          _put_buf(*(istr_start + current_pos));
          current_pos += 1;
          bit = 1;
        }
      }
      flag_byte = (flag_byte >> 1) | ((bit & 1) << 7);
    }
    _put(flag_byte);
    for (size_t i = 0; i < buflen; i++) {
      _put(*(buf_start + i));
    }
    buf = buf_start;
    buflen = 0;
  }
  _put(0);
  _put(0);
  _put(0);

  free(istr_start);
  free(buf_start);
  return olen;
}

#undef _get
#undef _put
#undef _put_buf

#define _get(c)                                                                \
  c = fgetc(input_str);                                                        \
  if (c == EOF) {                                                              \
    break;                                                                     \
  }

#define _put(c)                                                                \
  fputc(c, output_str);                                                        \
  *olen = ftell(output_str);

char *Decode(size_t ilen, char *istr, size_t *olen) {
  int c, h, l, flag, position, length, diff;
  unsigned short w;

  char *ostr = malloc(ilen);

  *olen = 0;

  FILE *input_str = fmemopen(istr, ilen, "rb");
  FILE *output_str = open_memstream(&ostr, olen);

  for (;;) {
    _get(flag);
    for (size_t i = 0; i < 8; i++) {
      if (((flag >> i) & 1) == 1) {
        _get(c);
        _put(c);
      } else {
        _get(h);
        _get(l);
        w = (((short)h) << 8) | (0xff & l);
        position = (w >> 4);
        length = (w & 0x0F) + THRESHOLD;
        if (position == 0) {
          fclose(input_str);
          fclose(output_str);
          return (char *)ostr;
        }
        if (position > *olen) {
          diff = position - *olen;
          diff = MIN(diff, length);
          for (size_t j = 0; j < diff; j++)
            _put(0);
          length -= diff;
        }
        if (-1 * position + length < 0) {
          fwrite(ostr + *olen - position, sizeof(char), length, output_str);
        } else {
          for (size_t j = 0; j < length; j++) {
            _put(*(ostr + *olen - position));
          }
        }
      }
    }
  }
  fclose(input_str);
  fclose(output_str);
  return (char *)ostr;
}

#undef _get
#undef _put
