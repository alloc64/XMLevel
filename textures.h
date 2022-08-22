/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#ifndef _TEXTURES_H
#define _TEXTURES_H

#include "./main.h"

struct TexData {
    GLsizei width;
    GLsizei height;

    GLenum format;
    GLint internalFormat;
    GLuint id;
    GLint depth;
    GLint totalComponenets;
    GLint totalMipMaps;
    GLint totalSize;
    GLint mipFactor;
    GLubyte *texels;
};

struct LoadedTexture {
    LoadedTexture() {
        strcpy(name, "");
        id = -1;
    }

    char name[1024];
    int id;
};

inline int GetTextureType(char *FileName) {
    if (strstr(FileName, ".jpg")) return 0;
    if (strstr(FileName, ".xmt")) return 1;
    if (strstr(FileName, ".dds")) return 2;

    return -1;
}

class Texture {
public:
    Texture() {
        index = 0;
    }

    bool LoadExtensions();

    int LoadTexture(char *FileName, GLuint TexIndex, GLuint Filtering, bool Clamped = false);

    int LoadCubemap(char **FilePaths);

    TexData *LoadJPG(char *FileName);

    TexData *LoadXMT(char *FileName);

    TexData *LoadDDS(char *FileName);

    LoadedTexture usedTexture[8192];

private:
    GLuint MinFilter;
    GLuint MaxFilter;
    int index;
};


#endif
