#include "format_conversion.h"

#define CLIP(x) ((x) > 255 ? 255 : ((x) < 0 ? 0 : x))
#define AVERAGE(a, b) (((a) / 2) + ((b) / 2) + ((a) & (b) & 1))

#define RGB2Y(R, G, B) CLIP(( (  66 * (R) + 129 * (G) +  25 * (B) + 128) >> 8) +  16)
#define RGB2U(R, G, B) CLIP(( ( -38 * (R) -  74 * (G) + 112 * (B) + 128) >> 8) + 128)
#define RGB2V(R, G, B) CLIP(( ( 112 * (R) -  94 * (G) -  18 * (B) + 128) >> 8) + 128)

void r8g8b8a8_to_yuy2(const unsigned char *rgba, unsigned char *yuy2, int in_stride, int width, int height)
{
	int i, j;

	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j+=2) {
			const unsigned char *rgbap = &rgba[4 * (j + i * in_stride)];
			unsigned char *yuy2p = &yuy2[2 * (j + i * width)];

			unsigned char p0_r = rgbap[0 + 0],
			              p0_g = rgbap[0 + 1],
			              p0_b = rgbap[0 + 2];

			unsigned char p1_r = rgbap[4 + 0],
			              p1_g = rgbap[4 + 1],
			              p1_b = rgbap[4 + 2];

			unsigned char sub_r = AVERAGE(p0_r, p1_r);
			unsigned char sub_g = AVERAGE(p0_g, p1_g);
			unsigned char sub_b = AVERAGE(p0_b, p1_b);
			unsigned char y0 = RGB2Y(p0_r, p0_g, p0_b);
			unsigned char y1 = RGB2Y(p1_r, p1_g, p1_b);
			unsigned char u = RGB2U(sub_r, sub_g, sub_b);
			unsigned char v = RGB2V(sub_r, sub_g, sub_b);

			yuy2p[0] = y0;
			yuy2p[1] = u;
			yuy2p[2] = y1;
			yuy2p[3] = v;
		}
	}
}

void r5g6b5_to_yuy2(const unsigned char *rgb, unsigned char *yuy2, int in_stride, int width, int height)
{
	int i, j;

	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j+=2) {
			const unsigned short *rgbp = (unsigned short *)&rgb[2 * (j + i * in_stride)];
			unsigned char *yuy2p = &yuy2[2 * (j + i * width)];

			unsigned short p0 = rgbp[0];
			unsigned short p1 = rgbp[1];

			unsigned char p0_r = p0 & 0x1F,
			              p0_g = (p0 >> 5) & 0x3F,
			              p0_b = (p0 >> 11) & 0x1F;

			unsigned char p1_r = p1 & 0x1F,
			              p1_g = (p1 >> 5) & 0x3F,
			              p1_b = (p1 >> 11) & 0x1F;

			p0_r = (p0_r * 527 + 23) >> 6;
			p0_g = (p0_g * 259 + 33) >> 6;
			p0_b = (p0_b * 527 + 23) >> 6;

			p1_r = (p1_r * 527 + 23) >> 6;
			p1_g = (p1_g * 259 + 33) >> 6;
			p1_b = (p1_b * 527 + 23) >> 6;

			unsigned char sub_r = AVERAGE(p0_r, p1_r);
			unsigned char sub_g = AVERAGE(p0_g, p1_g);
			unsigned char sub_b = AVERAGE(p0_b, p1_b);
			unsigned char y0 = RGB2Y(p0_r, p0_g, p0_b);
			unsigned char y1 = RGB2Y(p1_r, p1_g, p1_b);
			unsigned char u = RGB2U(sub_r, sub_g, sub_b);
			unsigned char v = RGB2V(sub_r, sub_g, sub_b);

			yuy2p[0] = y0;
			yuy2p[1] = u;
			yuy2p[2] = y1;
			yuy2p[3] = v;
		}
	}
}

void r5g5b5a1_to_yuy2(const unsigned char *rgba, unsigned char *yuy2, int in_stride, int width, int height)
{
	int i, j;

	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j+=2) {
			const unsigned short *rgbap = (unsigned short *)&rgba[2 * (j + i * in_stride)];
			unsigned char *yuy2p = &yuy2[2 * (j + i * width)];

			unsigned short p0 = rgbap[0];
			unsigned short p1 = rgbap[1];

			unsigned char p0_r = p0 & 0x1F,
			              p0_g = (p0 >> 5) & 0x1F,
			              p0_b = (p0 >> 10) & 0x1F;

			unsigned char p1_r = p1 & 0x1F,
			              p1_g = (p1 >> 5) & 0x1F,
			              p1_b = (p1 >> 10) & 0x1F;

			p0_r = (p0_r * 527 + 23) >> 6;
			p0_g = (p0_g * 527 + 23) >> 6;
			p0_b = (p0_b * 527 + 23) >> 6;

			p1_r = (p1_r * 527 + 23) >> 6;
			p1_g = (p1_g * 527 + 23) >> 6;
			p1_b = (p1_b * 527 + 23) >> 6;

			unsigned char sub_r = AVERAGE(p0_r, p1_r);
			unsigned char sub_g = AVERAGE(p0_g, p1_g);
			unsigned char sub_b = AVERAGE(p0_b, p1_b);
			unsigned char y0 = RGB2Y(p0_r, p0_g, p0_b);
			unsigned char y1 = RGB2Y(p1_r, p1_g, p1_b);
			unsigned char u = RGB2U(sub_r, sub_g, sub_b);
			unsigned char v = RGB2V(sub_r, sub_g, sub_b);

			yuy2p[0] = y0;
			yuy2p[1] = u;
			yuy2p[2] = y1;
			yuy2p[3] = v;
		}
	}
}
