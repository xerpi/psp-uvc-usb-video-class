#ifndef FORMAT_CONVERSION_H
#define FORMAT_CONVERSION_H

#include <inttypes.h>

void r8g8b8a8_to_yuy2(const unsigned char *rgb, unsigned char *yuy2, int in_stride, int width, int height);

#endif
