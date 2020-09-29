#ifndef __PSPDMACPLUS_H__
#define __PSPDMACPLUS_H__

#ifdef __cplusplus
extern "C" {
#endif

#define SCE_DMACPLUS_LCDC_FORMAT_RGBA8888 0
#define SCE_DMACPLUS_LCDC_FORMAT_RGB565   1
#define SCE_DMACPLUS_LCDC_FORMAT_RGBA5551 2
#define SCE_DMACPLUS_LCDC_FORMAT_RGBA4444 3

extern void *sceDmacplusLcdcGetBaseAddr(void);
extern int sceDmacplusLcdcGetFormat(int *width, int *stride, int *pixelformat);

#ifdef __cplusplus
}
#endif

#endif
