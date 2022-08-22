#include "img.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MCHAR4(a, b, c, d) (a | (b << 8) | (c << 16) | (d << 24))
#pragma pack (push, 1)

struct DdsHeader {
    unsigned int ddsIdentifier;
    unsigned char junk[8];
    unsigned int height;
    unsigned int width;

    unsigned int pitch;
    unsigned int depth;
    unsigned int nMipMaps;

    unsigned char junk3[52];
    unsigned int fourCC;
    unsigned int bpp;
    unsigned char junk4[36];
};

#pragma pack (pop)


Image3D::Image3D() {
    pixels = NULL;
    width = 0;
    height = 0;
    depth = 0;
    nMipMaps = 0;
    format = FORMAT_NONE;
    ownsMemory = true;
}

Image3D::Image3D(const Image3D &img) {
    width = img.width;
    height = img.height;
    depth = img.depth;
    nMipMaps = img.nMipMaps;
    format = img.format;
    ownsMemory = true;

    int size = getMipMappedSize(0, nMipMaps);
    pixels = new unsigned char[size];
    memcpy(pixels, img.pixels, size);
}

Image3D::~Image3D() {
    if (ownsMemory && (pixels != NULL)) delete pixels;
}

void Image3D::free() {
    if (pixels != NULL) {
        if (ownsMemory) delete pixels;
        pixels = NULL;
    }
    ownsMemory = true;
}

void Image3D::clear() {
    free();
    width = 0;
    height = 0;
    depth = 0;
    nMipMaps = 0;
    format = FORMAT_NONE;
}

unsigned char *Image3D::getImage(const int mipMapLevel) const {
    return (mipMapLevel < nMipMaps) ? pixels + getMipMappedSize(0, mipMapLevel) : NULL;
}

int Image3D::getNumberOfMipMaps() const {
    return nMipMaps;
}

int Image3D::getNumberOfMipMapsFromDimesions() const {
    int max = (width > height) ? ((width > depth) ? width : depth) : (height > depth) ? height : depth;
    int i = 0;

    while (max > 0) {
        max >>= 1;
        i++;
    }

    return i;
}

int Image3D::getMipMappedSize(const int firstMipMapLevel, const int nMipMapLevels, FORMAT srcFormat) const {
    int w = getWidth(firstMipMapLevel) << 1,
            h = getHeight(firstMipMapLevel) << 1,
            d = getDepth(firstMipMapLevel) << 1;
    int level = 0, size = 0;

    if (srcFormat == FORMAT_NONE) srcFormat = format;

    while (level < nMipMapLevels && (w != 1 || h != 1 || d != 1)) {
        if (w > 1) w >>= 1;
        if (h > 1) h >>= 1;
        if (d > 1) d >>= 1;
        if (isCompressedFormat(srcFormat)) {
            size += ((w + 3) >> 2) * ((h + 3) >> 2) * d;
        } else {
            size += w * h * d;
        }
        level++;
    }

    if (isCompressedFormat(srcFormat)) {
        size *= getBytesPerBlock(srcFormat);
    } else {
        size *= getBytesPerPixel(srcFormat);
    }
    return size;
}

int Image3D::getWidth(const int mipMapLevel) const {
    int a = width >> mipMapLevel;
    return (a == 0) ? 1 : a;
}

int Image3D::getHeight(const int mipMapLevel) const {
    int a = height >> mipMapLevel;
    return (a == 0) ? 1 : a;
}

int Image3D::getDepth(const int mipMapLevel) const {
    int a = depth >> mipMapLevel;
    return (a == 0) ? 1 : a;
}

void Image3D::loadFromMemory(void *mem, const int w, const int h, const int d, const FORMAT frmt, bool ownsmemory,
                             bool mipMapped) {
    free();

    pixels = (unsigned char *) mem;
    width = w;
    height = h;
    depth = d;
    format = frmt;
    ownsMemory = ownsmemory;

    nMipMaps = mipMapped ? getNumberOfMipMapsFromDimesions() : 1;
}

bool Image3D::saveToFile(const char *fileName) {
    if (!isPlainFormat(format) || isFloatFormat(format)) return false;

    DdsHeader header = {
            MCHAR4('D', 'D', 'S', ' '),
            {0x44, 0x44, 0x53, 0x20, 0x7c, 0x00, 0x00, 0x00},
            height,
            width,
            0,
            depth,
            (nMipMaps == 1) ? 0 : nMipMaps,
            {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00,
             0x00, 0x00, 0x02, 0x00},
            0,
            8 * getChannels(format),
            {0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
             0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
             0x00, 0x00, 0x00, 0x00},
    };

    FILE *file = fopen(fileName, "wb");
    if (file == NULL) return false;

    fwrite(&header, sizeof(header), 1, file);
    fwrite(pixels, getMipMappedSize(0, nMipMaps), 1, file);
    fclose(file);

    return true;
}

void Image3D::buildMipMap(unsigned char *dest, unsigned char *src, int width, int height, int depth, int channels) {
    int xOff = (width < 2) ? 0 : channels;
    int yOff = (height < 2) ? 0 : width * channels;
    int zOff = (depth < 2) ? 0 : width * height * channels;

    for (int z = 0; z < depth; z += 2) {
        for (int y = 0; y < height; y += 2) {
            for (int x = 0; x < width; x += 2) {
                for (int i = 0; i < channels; i++) {
                    *dest++ = ((src[0] + src[xOff] + src[yOff] + src[yOff + xOff] +
                                src[zOff] + src[zOff + xOff] + src[zOff + yOff] + src[zOff + yOff + xOff]) + 4) >> 3;
                    src++;
                }
                src += xOff;
            }
            src += yOff;
        }
        src += zOff;
    }
}

bool Image3D::createMipMaps() {
    if (format > FORMAT_RGBA8) return false;

    int w = width, h = height, d = depth;
    unsigned char *src, *dest;

    if (nMipMaps <= 1) {
        pixels = (unsigned char *) realloc(pixels, getMipMappedSize());
        nMipMaps = getNumberOfMipMapsFromDimesions();
    }
    dest = pixels;
    int nChannels = getChannels(format);

    while (w > 1 || h > 1 || d > 1) {
        src = dest;
        dest += w * h * d * nChannels;
        buildMipMap(dest, src, w, h, d, nChannels);

        if (w > 1) w >>= 1;
        if (h > 1) h >>= 1;
        if (d > 1) d >>= 1;
    }

    return true;
}
