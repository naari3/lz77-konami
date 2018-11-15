#include "lz77-konami.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define N 4096 /* size of ring buffer */
#define F 18   /* upper limit for match_length */

#define WINDOW_SIZE = 0x1000
#define WINDOW_MASK = WINDOW_SIZE - 1
const int THRESHOLD = 3;
#define INPLACE_THRESHOLD = 0xA
#define LOOK_RANGE = 0x200
#define MAX_LEN = 0xF + THRESHOLD

#define _put(c)                                                                \
  if (*olen == limit) {                                                        \
    limit *= 2;                                                                \
    ostr = realloc(ostr, limit);                                               \
  }                                                                            \
  ostr[*olen] = c;                                                             \
  *olen += 1

#define _get(c)                                                                \
  if (!ilen) {                                                                 \
    c = EOF;                                                                   \
    break;                                                                     \
  }                                                                            \
  c = *istr;                                                                   \
  ++istr;                                                                      \
  --ilen

char *Decode(size_t ilen, unsigned char *istr, size_t *olen) {
  int c, h, l, flag, position, length, diff;
  unsigned short w;

  char *ostr = malloc(ilen);
  char *ostr_start = ostr;

  int limit = ilen;

  *olen = 0;

  for (;;) {
    _get(flag);
    for (int i = 0; i < 8; i++) {
      if (((flag >> i) & 1) == 1) {
        _get(c);
        _put(c);
      } else {
        _get(h);
        _get(l);
        w = (((short)h) << 8) | (0xff & l);
        position = (w >> 4);
        length = (w & 0x0F) + THRESHOLD;
        // printf("high %u\n", h);
        // printf("low %u\n", l);
        // printf("%u\n", w);
        // printf("position, %u\n", position);
        // printf("length, %u\n\n", length);
        if (position == 0) {
          return ostr;
        }
        if (position > *olen) {
          diff = position - *olen;
          diff = MIN(diff, length);
          for (int j = 0; j < diff; j++)
            _put(0);
          length -= diff;
        }
        if (-1 * position + length < 0) {
          for (int j = 0; j < length; j++) {
            _put(*(ostr_start + *olen - length + j));
          }
        } else {
          for (int j = 0; j < length; j++) {
            _put(*(ostr_start + *olen - length));
          }
        }
      }
    }
  }
  return ostr;
}

#undef _get
#undef _put
