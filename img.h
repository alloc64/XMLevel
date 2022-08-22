#ifndef _IMAGE3D_H_
#define _IMAGE3D_H_

enum FORMAT {
    FORMAT_NONE = 0,
    // Plain formats
    FORMAT_I8 = 1,
    FORMAT_IA8 = 2,
    FORMAT_RGB8 = 3,
    FORMAT_RGBA8 = 4,

    FORMAT_I16 = 5,
    FORMAT_RG16 = 6,
    FORMAT_RGB16 = 7,
    FORMAT_RGBA16 = 8,

    FORMAT_I16F = 9,
    FORMAT_RG16F = 10,
    FORMAT_RGB16F = 11,
    FORMAT_RGBA16F = 12,

    FORMAT_I32F = 13,
    FORMAT_RG32F = 14,
    FORMAT_RGB32F = 15,
    FORMAT_RGBA32F = 16,
    // Packed formats
    FORMAT_RGB565 = 17,
    // Compressed formats
    FORMAT_DXT1 = 18,
    FORMAT_DXT3 = 19,
    FORMAT_DXT5 = 20,
    FORMAT_3DC = 21,


    // TODO: Fix these
    FORMAT_D16 = 22,
    FORMAT_D24 = 23,
};


class Image3D {
protected:
    unsigned char *pixels;
    int width, height, depth;
    int nMipMaps;
    FORMAT format;
    bool ownsMemory;

public:
    Image3D();

    Image3D(const Image3D &img);

    ~Image3D();

    void free();

    void clear();

    unsigned char *getImage(const int mipMapLevel = 0) const;

    int getNumberOfMipMaps() const;

    int getNumberOfMipMapsFromDimesions() const;

    int getMipMappedSize(const int firstMipMapLevel = 0, const int nMipMapLevels = 0x7fffffff,
                         FORMAT srcFormat = FORMAT_NONE) const;

    int getWidth(const int mipMapLevel = 0) const;

    int getHeight(const int mipMapLevel = 0) const;

    int getDepth(const int mipMapLevel = 0) const;

    FORMAT getFormat() const { return format; }

protected:
    void buildMipMap(unsigned char *dest, unsigned char *src, int width, int height, int depth, int channels);

public:
    bool loadFromFile(const char *fileName, const bool useRGBA = true);

    void loadFromMemory(void *mem, const int w, const int h, const int d, const FORMAT frmt, bool ownsmemory,
                        bool mipMapped = false);

    bool saveToFile(const char *fileName);

    bool createMipMaps();
};


inline int getBytesPerBlock(const FORMAT format) {
    return (format == FORMAT_DXT1) ? 8 : 16;
}


inline bool isPlainFormat(const FORMAT format) {
    return (format <= FORMAT_RGBA32F);
}

inline bool isPackedFormat(const FORMAT format) {
    return (format == FORMAT_RGB565);
}

inline bool isCompressedFormat(const FORMAT format) {
    return (format >= FORMAT_DXT1) && (format <= FORMAT_3DC);
}

inline bool isFloatFormat(const FORMAT format) {
    return (format >= FORMAT_I16F && format <= FORMAT_RGBA32F);
}

inline bool isDepthFormat(const FORMAT format) {
    return (format >= FORMAT_D16 && format <= FORMAT_D24);
}

inline int getBytesPerPixel(const FORMAT format) {
    switch (format) {
        case FORMAT_I8:
            return 1;
        case FORMAT_IA8:
        case FORMAT_I16:
        case FORMAT_I16F:
        case FORMAT_RGB565:
        case FORMAT_D16:
            return 2;
        case FORMAT_RGB8:
        case FORMAT_D24:
            return 3;
        case FORMAT_RGBA8:
        case FORMAT_RG16:
        case FORMAT_RG16F:
        case FORMAT_I32F:
            return 4;
        case FORMAT_RGB16:
        case FORMAT_RGB16F:
            return 6;
        case FORMAT_RG32F:
            return 8;
        case FORMAT_RGBA16:
        case FORMAT_RGBA16F:
        case FORMAT_RGB32F:
            return 12;
        case FORMAT_RGBA32F:
            return 16;
        default:
            return 0;
    }
}


inline int getChannels(const FORMAT format) {
    switch (format) {
        case FORMAT_I8:
        case FORMAT_I16:
        case FORMAT_I16F:
        case FORMAT_I32F:
        case FORMAT_D16:
        case FORMAT_D24:
            return 1;
        case FORMAT_IA8:
        case FORMAT_RG16:
        case FORMAT_RG16F:
        case FORMAT_RG32F:
        case FORMAT_3DC:
            return 2;
        case FORMAT_RGB8:
        case FORMAT_RGB16:
        case FORMAT_RGB16F:
        case FORMAT_RGB32F:
        case FORMAT_RGB565:
        case FORMAT_DXT1:
            return 3;
        case FORMAT_RGBA8:
        case FORMAT_RGBA16:
        case FORMAT_RGBA16F:
        case FORMAT_RGBA32F:
        case FORMAT_DXT3:
        case FORMAT_DXT5:
            return 4;
        default:
            return 0;
    }
}


#endif // _IMAGE3D_H_
