#include "format_conversion.h"

#define CLIP(x) ((x) > 255 ? 255 : ((x) < 0 ? 0 : x))
#define AVERAGE(a, b) (((a) / 2) + ((b) / 2) + ((a) & (b) & 1))

#define RGB2Y(R, G, B) CLIP(( (  66 * (R) + 129 * (G) +  25 * (B) + 128) >> 8) +  16)
#define RGB2U(R, G, B) CLIP(( ( -38 * (R) -  74 * (G) + 112 * (B) + 128) >> 8) + 128)
#define RGB2V(R, G, B) CLIP(( ( 112 * (R) -  94 * (G) -  18 * (B) + 128) >> 8) + 128)

void r8g8b8a8_to_yuy2(const unsigned char *rgb, unsigned char *yuy2, int in_stride, int width, int height)
{
	int i, j;

	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j+=2) {
			const unsigned char *rgbap = &rgb[4 * (j + i * in_stride)];
			unsigned char *yuy2p = &yuy2[2 * (j + i * width)];

			unsigned char sub_r = AVERAGE(rgbap[0 + 0], rgbap[4 + 0]);
			unsigned char sub_g = AVERAGE(rgbap[0 + 1], rgbap[4 + 1]);
			unsigned char sub_b = AVERAGE(rgbap[0 + 2], rgbap[4 + 2]);
			unsigned char y0 = RGB2Y(rgbap[0 + 0], rgbap[0 + 1], rgbap[0 + 2]);
			unsigned char y1 = RGB2Y(rgbap[4 + 0], rgbap[4 + 1], rgbap[4 + 2]);
			unsigned char u = RGB2U(sub_r, sub_g, sub_b);
			unsigned char v = RGB2V(sub_r, sub_g, sub_b);

			yuy2p[0] = y0;
			yuy2p[1] = u;
			yuy2p[2] = y1;
			yuy2p[3] = v;
		}
	}
}
