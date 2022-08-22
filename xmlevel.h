/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#ifndef _XMLEVEL_H
#define _XMLEVEL_H

#include "math\vector.h"
#include <gl\gl.h>

struct xmlEntity {
    char name[256];
    vec3 rotation;
    vec3 center;
    vec3 min;
    vec3 max;
    float radius;
};

struct xmlDynEntFrame {
    short int iCurrFrame;
    vec3 *vertex;
};

struct xmlDynamicEntity {
    char name[256];
    vec3 center;
    vec3 min;
    vec3 max;
    float radius;
    short int iNumFrames;
    short int iFramerate;
    short int iNumVerts;
    short int iNumFaces;
    xmFace *face;
    xmlDynEntFrame *frame;
    vec2 *texcoord;
    vec3 *tangent;
    vec3 *bitangent;
    vec3 *normal;
    char texturePath[1024];
    char normalMapPath[1024];
    char specularMapPath[1024];
    short int textureFilter;
    float opacity;

};

struct PolyData {
    vec3 v0, vDir0, vDir1;
    vec2 t0, tDir0, tDir1;
};

struct xmlLight {
    //point light specific
    float radius;

    //spotlight specific
    float falloff;
    float hotspot;
    vec3 target;

    short int type;
    color3f color;
    vec3 center;
};

struct xmlObject {
    vec3 *vertex;
    vec3 *normal;
    vec3 *tangent;
    vec3 *bitangent;
    vec2 *texcoord;
    vec2 *texcoord2;
    xmFace *face;
    vec3 min;
    vec3 max;
    vec3 center;
    float radius;
    long int iNumFaces;
    long int iNumVertices;
    char name[256];
    char texturePath[1024];
    char heightMapPath[1024];
    char specularMapPath[1024];
    short int textureFilter;
    float opacity;
    float shininess;
    GLuint uDiffuseMap;
    GLuint uNormalMap;
    GLuint uHeightMap;

    GLuint uVertexVBO;
    GLuint uTexCoordVBO;
    GLuint uNormalVBO;
    GLuint uFaceVBO;

    bool bUseBumpMap;
    vec3 BoxVertices[36];
    vec3 BoxNormals[12];
};

class xmLevel {
public:
    xmLevel();

    ~xmLevel() {};

    bool xmLoadBinaryXMLevel(char *xmLevelPath);

    bool xmSaveBinaryXMLevel(char *xmLevelPath);

    xmlObject *object;
    xmlEntity *entity;
    xmlEntity *spawn;
    xmlDynamicEntity *dynamicEntity;
    xmlLight *areaLight;
    xmlLight mainLight;
    short int iNumObjects;
    short int iNumEntities;
    short int iNumSpawns;
    short int iNumDynamicEntities;
    short int iNumLights;
    short int iSkyNum;
    short int iShadowCasterID;
    vec3 worldMin;
    vec3 worldMax;
    vec3 worldCenter;
    float worldRadius;
    int iNVObjects;
    int iNVEntities;
    int iNumEmptyObjects;
    char filepath[1024];
private:
    char headerTitle[255];
    char header[8];

};

inline char *makeOptionalTexturePath(char *fullPath) {
    char *texturePaths[3] =
            {
                    "low",
                    "medium",
                    "high"
            };

    char finalpath[1024];
    sprintf(finalpath, "textures/%s/%s", texturePaths[2], fullPath);
    return finalpath;
}


#endif
