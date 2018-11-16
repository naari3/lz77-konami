#ifndef LZ77_KONAMI_H
#define LZ77_KONAMI_H

#include <ctype.h>
unsigned char *Decode(size_t ilen, unsigned char *istr, size_t *olen);
size_t Encode(size_t ilen, unsigned char *istr, size_t olen,
              unsigned char *ostr);

#endif
