#ifndef OPEN_MEMSTREAM_H_
#define OPEN_MEMSTREAM_H_
#include <stdio.h>
FILE *open_memstream(char **bufp, size_t *sizep);
#endif
