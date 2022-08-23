/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#include "./main.h"
#include "directx/ddraw.h"

//#define _texdebug

int Texture::LoadTexture(char *FileName, GLuint TexIndex, GLuint Filtering, bool Clamped) {
    GLuint TextureID = 0;
    TexData *pTexData = NULL;
    int MipMapOffset = 0;
    for (int i = 0; i < 8192; i++) {
        if (usedTexture[i].name && usedTexture[i].id != -1 && strstr(usedTexture[i].name, FileName)) {
            return usedTexture[i].id;
        }
    }

    switch (Filtering) {
        case 0: //LINEAR
            MinFilter = GL_NEAREST;
            MaxFilter = GL_LINEAR;
#ifdef _texdebug
            printf("warning: setting texture filter GL_NEAREST;\n");
#endif
            break;

        case 1: //BILINEAR
            MinFilter = GL_LINEAR;//GL_LINEAR_MIPMAP_NEAREST;
            MaxFilter = GL_LINEAR;
#ifdef _texdebug
            printf("warning: setting texture filter GL_LINEAR;\n");
#endif
            break;

        case 2: //TRILINEAR
            MinFilter = GL_LINEAR;//GL_LINEAR_MIPMAP_LINEAR;
            MaxFilter = GL_LINEAR;
#ifdef _texdebug
            printf("warning: setting texture filter GL_LINEAR_MIPMAP_LINEAR;\n");
#endif
            break;

        default:
            printf("error: bad filter type %d %s!\n", Filtering, FileName);
            return -1;
            break;
    }


    switch (GetTextureType(FileName)) {
        case 0: //JPG texture
            pTexData = LoadJPG(FileName);
            if (!pTexData) {
                printf("error: cannot load texture %s!\n", FileName);
                return -1;
            }
            glGenTextures(1, &TextureID);
            glBindTexture(GL_TEXTURE_2D, TextureID);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, MinFilter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, MaxFilter);
#if 0
            glTexImage2D (GL_TEXTURE_2D, 0, pTexData->internalFormat, pTexData->width, pTexData->height, 0, pTexData->format, GL_UNSIGNED_BYTE, pTexData->texels);
#else
            printf("warning: creating mipmaps is very slow!(JPEG) \n");
            gluBuild2DMipmaps(GL_TEXTURE_2D, pTexData->internalFormat, pTexData->width, pTexData->height,
                              pTexData->format, GL_UNSIGNED_BYTE, pTexData->texels);
#endif

            break;

        case 1:    //XMT texture
            pTexData = LoadXMT(FileName);
            if (!pTexData) {
                printf("error: cannot load texture %s!\n", FileName);
                return -1;
            }
            glGenTextures(1, &TextureID);
            glBindTexture(GL_TEXTURE_2D, TextureID);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, MinFilter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, MaxFilter);
            glTexImage2D(GL_TEXTURE_2D, 0, pTexData->internalFormat, pTexData->width, pTexData->height, 0,
                         pTexData->format, GL_UNSIGNED_BYTE, &pTexData->texels);
            break;

        case 2:    //DDS texture
            pTexData = LoadDDS(FileName);
            if (!pTexData) {
                printf("error: cannot load texture %s!\n", FileName);
                return -1;
            }
            glGenTextures(1, &TextureID);
            glBindTexture(GL_TEXTURE_2D, TextureID);

            if (pTexData->format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT)
                pTexData->mipFactor = 8;
            else pTexData->mipFactor = 16;
            for (int i = 0; i < pTexData->totalMipMaps; i++) {
                int MipMapSize = ((pTexData->width + 3) / 4) * ((pTexData->height + 3) / 4) * pTexData->mipFactor;

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, Clamped ? GL_CLAMP : GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, Clamped ? GL_CLAMP : GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, MinFilter);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, MaxFilter);

                glCompressedTexImage2DARB(GL_TEXTURE_2D, i, pTexData->format, pTexData->width, pTexData->height, 0,
                                          MipMapSize, pTexData->texels + MipMapOffset);

                pTexData->width = pTexData->width >> 1;
                pTexData->height = pTexData->height >> 1;

                MipMapOffset += MipMapSize;
            }
            break;

        default:
            printf("error: texture isn't supported! only JPG, DDS and XMT are supported!\n");
            return -1;
            break;
    }

    if (pTexData) {
        if (pTexData->texels) delete[] pTexData->texels;
        delete pTexData;
    }
    strcpy(usedTexture[index].name, FileName);
    usedTexture[index].id = TextureID;
    index++;
    printf("ok: texture %s loaded sucessfully!\n", FileName);

    return TextureID;
}

int Texture::LoadCubemap(char **FilePaths) {
    GLuint cubeMap;
    glEnable(GL_TEXTURE_CUBE_MAP_ARB);
    glGenTextures(1, &cubeMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, cubeMap);

    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP);

    for (int i = 0; i < 6; i++) {
        switch (GetTextureType(FilePaths[i])) {
            case 0: {
                TexData *t = LoadJPG(FilePaths[i]);
                if (!t) {
                    printf("error: %s not found\n", FilePaths[i]);
                    continue;
                }
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + i, 0, 3, t->width, t->height, 0, GL_RGB,
                             GL_UNSIGNED_BYTE, t->texels);
            }
                break;

            case 2: {
                TexData *t = LoadDDS(FilePaths[i]);
                if (!t) {
                    printf("error: %s not found\n", FilePaths[i]);
                    continue;
                }
                if (t->format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) t->mipFactor = 8; else t->mipFactor = 16;
                int MipMapSize = ((t->width + 3) / 4) * ((t->height + 3) / 4) * t->mipFactor;
                glCompressedTexImage2DARB(GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + i, 0, t->format, t->width, t->height, 0,
                                          MipMapSize, t->texels);
            }
                break;
        }
    }
    glDisable(GL_TEXTURE_CUBE_MAP_ARB);

    return cubeMap;
}

TexData *Texture::LoadJPG(char *FileName) {
    TexData *pData = NULL;

    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    JSAMPROW j;

    FILE *pFile = fopen(FileName, "rb");
    if (!pFile) {
        printf("error: couldn't open %s!\n", FileName);
        return NULL;
    }

    jpeg_create_decompress(&cinfo);
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_stdio_src(&cinfo, pFile);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);

    pData = (TexData *) malloc(sizeof(TexData));

    pData->width = cinfo.image_width;
    pData->height = cinfo.image_height;
    pData->internalFormat = cinfo.num_components;

    if (cinfo.num_components == 1) {
        pData->format = GL_LUMINANCE;
    } else {
        pData->format = GL_RGB;
    }

    pData->texels = (GLubyte *) malloc(sizeof(GLubyte) * pData->width * pData->height * pData->internalFormat);

    for (int i = 0; i < pData->height; ++i) {
        //j = (pData->texels +((pData->height - (i + 1)) * pData->width * pData->internalFormat)); // fliped image
        j = &pData->texels[pData->width * i * pData->internalFormat];
        jpeg_read_scanlines(&cinfo, &j, 1);
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

    fclose(pFile);

    return pData;
}

TexData *Texture::LoadXMT(char *FileName) {

    int width = 0;
    int height = 0;
    int channels = 0;
    int size = 0;
    char header[3];

    FILE *TexFile = fopen(FileName, "rb");
    if (!TexFile) printf("error: file %s not found!\n", FileName);

    fread(&header, 3, 1, TexFile);
    if (header[0] != 'X' || header[1] != 'M' || header[2] != 'T') {
        printf("error: header isn't XMT! file %s has bad format!\n", FileName);
        return nullptr;
    }

    fread(&width, sizeof(int), 1, TexFile);
    fread(&height, sizeof(int), 1, TexFile);
    fread(&channels, sizeof(int), 1, TexFile);

    size = width * height * channels;
    TexData *pTexData = new TexData[size];

    if (!pTexData) {
        printf("error: cannot allocate memory for T texture!\n");
        return nullptr;
    }

    fread(&pTexData->texels, size, 1, TexFile);

    pTexData->width = width;
    pTexData->height = height;
    pTexData->internalFormat = channels;
    pTexData->format = GL_RGB;

    return pTexData;
}

TexData *Texture::LoadDDS(char *FileName) {
    FILE *fp;
    TexData *pTexData = NULL;
    DDSURFACEDESC2 ddsd;
    char imageID[4];
    int imageSize = 0;

    fp = fopen(FileName, "rb");
    if (!fp) {
        //printf("error: couldn't open %s!\n", FileName);
        return NULL;
    }

    fread(imageID, 1, 4, fp);

    if (strncmp(imageID, "DDS ", 4) != 0) {
        fclose(fp);
        return NULL;
    }

    fread(&ddsd, sizeof(DDSURFACEDESC2), 1, fp);

    if (ddsd.dwLinearSize != 0) {
        imageSize = ddsd.dwLinearSize;
    } else {
        imageSize = ddsd.dwHeight * ddsd.dwWidth;
    }

    if (imageSize <= 0) {
        printf("error: %s is empty!\n", FileName);
        fclose(fp);
        return NULL;
    }

    pTexData = new TexData();
    pTexData->totalMipMaps = ddsd.dwMipMapCount;
    pTexData->width = ddsd.dwWidth;
    pTexData->height = ddsd.dwHeight;

    // DXT1 has a compression ratio of 8:1, DXT3 has 4:1, and DXT5 has 4:1.
    switch (ddsd.ddpfPixelFormat.dwFourCC) {
        case FOURCC_DXT1:
            pTexData->mipFactor = 2;
            pTexData->totalComponenets = 3;
            pTexData->format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
            break;

        case FOURCC_DXT3:
            pTexData->mipFactor = 4;
            pTexData->totalComponenets = 4;
            pTexData->format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
            break;

        case FOURCC_DXT5:
            pTexData->mipFactor = 4;
            pTexData->totalComponenets = 4;
            pTexData->format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
            break;

        default:
            fclose(fp);
            return NULL;
            break;
    }

    if (ddsd.dwMipMapCount > 1) pTexData->totalSize = imageSize * pTexData->mipFactor;
    else pTexData->totalSize = imageSize;

    pTexData->texels = new unsigned char[pTexData->totalSize];

    if (!pTexData->texels) {
        fclose(fp);
        return NULL;
    }

    fread(pTexData->texels, 1, pTexData->totalSize, fp);

    fclose(fp);
    return pTexData;
}

bool Texture::LoadExtensions() {
    glCompressedTexImage2DARB = (PFNGLCOMPRESSEDTEXIMAGE2DARBPROC) wglGetProcAddress("glCompressedTexImage2DARB");
    if (!glCompressedTexImage2DARB) {
        printf("error: cannot load ARB_texture_compression!\n");
        return false;
    }
    return true;
}

bool WriteTGA(char *file, short int width, short int height, unsigned char *outImage) {
    FILE *pFile;               // The file pointer.
    unsigned char uselessChar; // used for useless char.
    short int uselessInt;      // used for useless int.
    unsigned char imageType;   // Type of image we are saving.
    int index;                 // used with the for loop.
    unsigned char bits;    // Bit depth.
    long Size;                 // Size of the picture.
    int colorMode;
    unsigned char tempColors;
    pFile = fopen(file, "wb");
    if (!pFile) {
        fclose(pFile);
        return false;
    }
    imageType = 2;
    colorMode = 3;
    bits = 24;
    uselessChar = 0;
    uselessInt = 0;
    fwrite(&uselessChar, sizeof(unsigned char), 1, pFile);
    fwrite(&uselessChar, sizeof(unsigned char), 1, pFile);
    fwrite(&imageType, sizeof(unsigned char), 1, pFile);
    fwrite(&uselessInt, sizeof(short int), 1, pFile);
    fwrite(&uselessInt, sizeof(short int), 1, pFile);
    fwrite(&uselessChar, sizeof(unsigned char), 1, pFile);
    fwrite(&uselessInt, sizeof(short int), 1, pFile);
    fwrite(&uselessInt, sizeof(short int), 1, pFile);
    fwrite(&width, sizeof(short int), 1, pFile);
    fwrite(&height, sizeof(short int), 1, pFile);
    fwrite(&bits, sizeof(unsigned char), 1, pFile);
    fwrite(&uselessChar, sizeof(unsigned char), 1, pFile);
    Size = width * height * colorMode;
    for (index = 0; index < Size; index += colorMode) {
        tempColors = outImage[index];
        outImage[index] = outImage[index + 2];
        outImage[index + 2] = tempColors;
    }
    fwrite(outImage, sizeof(unsigned char), Size, pFile);
    fclose(pFile);
    return true;
}
