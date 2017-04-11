#ifndef GL_BITMAP_H
#define GL_BITMAP_H

#include <android/bitmap.h>

typedef struct {
    uint8_t alpha, red, green, blue;
} ARGB;

class Bitmap {
public:
    uint32_t *pixels;
    AndroidBitmapInfo bitmapInfo;

    Bitmap() {
        pixels = NULL;
    }
};

#endif //GL_BITMAP_H
