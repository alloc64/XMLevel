/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#ifndef _SKYBOX_H
#define _SKYBOX_H

struct SkyBoxVertex {
    SkyBoxVertex() {

    };

    SkyBoxVertex(float x, float y, float z) {
        this->x = x;
        this->y = y;
        this->z = z;
    };

    float x, y, z;
};

struct SkyBoxTexCoord {
    SkyBoxTexCoord() {

    };

    SkyBoxTexCoord(float x, float y, float z) {
        this->x = x;
        this->y = y;
        this->z = z;
    };

    float x, y, z;
};

class Skybox {
public:
    bool Create(char **paths, float size);

    void RenderSun(void);

    void Render(void);

    SkyBoxVertex vertex[24];
    SkyBoxTexCoord tcoord[24];
    GLuint skyID[6];
    GLuint skyCubeMap;

    GLuint sunGlowTexID;
    GLuint sunHaloTexID;

private:

};

#endif
