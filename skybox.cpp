/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#include "./main.h"

extern Texture texture;
extern xmLevel file;

bool Skybox::Create(char **paths, float size) {
    tcoord[0] = SkyBoxTexCoord(-1, 1, -1);
    vertex[0] = SkyBoxVertex(-size, -size, -size);
    tcoord[1] = SkyBoxTexCoord(-1, -1, -1);
    vertex[1] = SkyBoxVertex(-size, size, -size);
    tcoord[2] = SkyBoxTexCoord(-1, 1, 1);
    vertex[2] = SkyBoxVertex(-size, -size, size);
    tcoord[3] = SkyBoxTexCoord(-1, -1, 1);
    vertex[3] = SkyBoxVertex(-size, size, size);

    tcoord[4] = SkyBoxTexCoord(1, 1, -1);
    vertex[4] = SkyBoxVertex(size, -size, -size);
    tcoord[5] = SkyBoxTexCoord(1, 1, 1);
    vertex[5] = SkyBoxVertex(size, -size, size);
    tcoord[6] = SkyBoxTexCoord(1, -1, -1);
    vertex[6] = SkyBoxVertex(size, size, -size);
    tcoord[7] = SkyBoxTexCoord(1, -1, 1);
    vertex[7] = SkyBoxVertex(size, size, size);

    tcoord[8] = SkyBoxTexCoord(-1, 1, -1);
    vertex[8] = SkyBoxVertex(-size, -size, -size);
    tcoord[9] = SkyBoxTexCoord(-1, 1, 1);
    vertex[9] = SkyBoxVertex(-size, -size, size);
    tcoord[10] = SkyBoxTexCoord(1, 1, -1);
    vertex[10] = SkyBoxVertex(size, -size, -size);
    tcoord[11] = SkyBoxTexCoord(1, 1, 1);
    vertex[11] = SkyBoxVertex(size, -size, size);

    tcoord[12] = SkyBoxTexCoord(-1, -1, -1);
    vertex[12] = SkyBoxVertex(-size, size, -size);
    tcoord[13] = SkyBoxTexCoord(1, -1, -1);
    vertex[13] = SkyBoxVertex(size, size, -size);
    tcoord[14] = SkyBoxTexCoord(-1, -1, 1);
    vertex[14] = SkyBoxVertex(-size, size, size);
    tcoord[15] = SkyBoxTexCoord(1, -1, 1);
    vertex[15] = SkyBoxVertex(size, size, size);

    tcoord[16] = SkyBoxTexCoord(-1, 1, -1);
    vertex[16] = SkyBoxVertex(-size, -size, -size);
    tcoord[17] = SkyBoxTexCoord(1, 1, -1);
    vertex[17] = SkyBoxVertex(size, -size, -size);
    tcoord[18] = SkyBoxTexCoord(-1, -1, -1);
    vertex[18] = SkyBoxVertex(-size, size, -size);
    tcoord[19] = SkyBoxTexCoord(1, -1, -1);
    vertex[19] = SkyBoxVertex(size, size, -size);

    tcoord[20] = SkyBoxTexCoord(-1, 1, 1);
    vertex[20] = SkyBoxVertex(-size, -size, size);
    tcoord[21] = SkyBoxTexCoord(-1, -1, 1);
    vertex[21] = SkyBoxVertex(-size, size, size);
    tcoord[22] = SkyBoxTexCoord(1, 1, 1);
    vertex[22] = SkyBoxVertex(size, -size, size);
    tcoord[23] = SkyBoxTexCoord(1, -1, 1);
    vertex[23] = SkyBoxVertex(size, size, size);

    skyCubeMap = texture.LoadCubemap(paths);
    sunGlowTexID = texture.LoadTexture("textures/skybox/sun/glow.dds", 0, 1, true);

    if (sunGlowTexID == -1 || skyCubeMap == -1) {
        printf("error: cannot load textures ::SkyBox!\n");
        return false;
    }

    return true;
}

void Skybox::RenderSun(void) {
    glPushMatrix();
    {
        glEnable(GL_BLEND);
        {
            glBlendFunc(GL_ONE, GL_ONE);
            vec3 pos = vec3(file.mainLight.center.x,
                            file.mainLight.center.y - 800,
                            file.mainLight.center.z);

            int width = 500, height = 500;
            float mat[16];
            glGetFloatv(GL_MODELVIEW_MATRIX, mat);

            vec3 right = Normalize(vec3(mat[0], mat[4], mat[8])) * width;
            vec3 up = Normalize(vec3(mat[1], mat[5], mat[9])) * height;

            float quad[] =
                    {
                            0.0f, 0.0f, pos.x + (-right.x - up.x), pos.y + (-right.y - up.y), pos.z + (-right.z - up.z),
                            1.0f, 0.0f, pos.x + (right.x - up.x), pos.y + (right.y - up.y), pos.z + (right.z - up.z),
                            1.0f, 1.0f, pos.x + (right.x + up.x), pos.y + (right.y + up.y), pos.z + (right.z + up.z),
                            0.0f, 1.0f, pos.x + (-right.x + up.x), pos.y + (-right.y + up.y), pos.z + (-right.z + up.z)
                    };

            glBindTexture(GL_TEXTURE_2D, sunGlowTexID);
            glInterleavedArrays(GL_T2F_V3F, 0, quad);
            glDrawArrays(GL_QUADS, 0, 4);
        }
        glDisable(GL_BLEND);
    }
    glPopMatrix();
}

void Skybox::Render(void) {
    glPushMatrix();
    {
        glPushMatrix();
        {
            glEnable(GL_TEXTURE_CUBE_MAP_ARB);
            glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, skyCubeMap);

            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glTexCoordPointer(3, GL_FLOAT, sizeof(SkyBoxTexCoord), tcoord);

            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(3, GL_FLOAT, sizeof(SkyBoxVertex), vertex);

            for (int i = 0; i < 6; i++) glDrawArrays(GL_TRIANGLE_STRIP, i * 4, 4);

            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            glDisableClientState(GL_VERTEX_ARRAY);

            glDisable(GL_TEXTURE_CUBE_MAP_ARB);
        }
        glPopMatrix();
    }
    glPopMatrix();
}
