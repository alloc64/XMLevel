/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#include "main.h"

extern Texture texture;
extern xmLevel file;
Skybox skybox;
Frustum frustum;
int windowID;

bool isReflectableOrMatt(int u) {
    return (strstr(file.object[u].name, "reflectable") || strstr(file.object[u].name, "matt_material"));
}

bool isReflectableOrRefractive(int u) {
    return (strstr(file.object[u].name, "reflectable") || strstr(file.object[u].name, "refractive"));
}

bool isRefractive(int u) {
    return (strstr(file.object[u].name, "refractive"));
}

void CubemapRenderer::render() {
    GLfloat lp[] = {file.mainLight.center.x, file.mainLight.center.y, file.mainLight.center.z, 1.0f};
    char *arrTextures[6] =
            {
                    "textures/skybox/sky2/highres_px.dds",
                    "textures/skybox/sky2/highres_nx.dds",
                    "textures/skybox/sky2/highres_py.dds",
                    "textures/skybox/sky2/highres_ny.dds",
                    "textures/skybox/sky2/highres_pz.dds",
                    "textures/skybox/sky2/highres_nz.dds"
            };

    bool bFoundReflectiveObject = false;
    for (int o = 0; o < file.iNumObjects; o++) if (isReflectableOrRefractive(o)) bFoundReflectiveObject = true;

    if (!bFoundReflectiveObject) {
        printf("ok: no reflectable objects found!\nok: skipping render to cubemap!\n");
        goto _end;
    }
    if (!texture.LoadExtensions()) goto _end;

    if (!skybox.Create(arrTextures, 3000.0)) {
        printf("error: fatal error ::Skybox!\n");
        goto _end;
    }

    for (int o = 0; o < file.iNumObjects; o++)
        file.object[o].uDiffuseMap = texture.LoadTexture(makeOptionalTexturePath((char *) &file.object[o].texturePath),
                                                         o, (int) file.object[o].textureFilter);
    glLightfv(GL_LIGHT0, GL_POSITION, lp);

    GLuint CubeMapID;
    glGenTextures(1, &CubeMapID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, CubeMapID);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    for (int face = 0; face < 6; face++)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_RGB, TEXTURE_SIZE, TEXTURE_SIZE, 0, GL_RGB, GL_FLOAT,
                     NULL);

    for (int u = 0; u < file.iNumObjects; u++) {
        if (isReflectableOrRefractive(u)) {
            glPushMatrix();
            {
                unsigned int pos = 0, px = 0, py = 0;

                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                gluPerspective(90.0, 1.0, 1.0, 10000.0);
                glMatrixMode(GL_MODELVIEW);
                if (4 * TEXTURE_SIZE <= window_w || 3 * TEXTURE_SIZE <= window_h)
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                for (int i = 0; i < 6; i++) {
                    switch (i) {
                        case 0:
                            px = 3 * TEXTURE_SIZE;
                            py = TEXTURE_SIZE;
                            break;
                        case 1:
                            px = 2 * TEXTURE_SIZE;
                            py = TEXTURE_SIZE;
                            break;
                        case 2:
                            px = TEXTURE_SIZE;
                            py = TEXTURE_SIZE;
                            break;
                        case 3:
                            px = 0;
                            py = TEXTURE_SIZE;
                            break;
                        case 4:
                            px = TEXTURE_SIZE;
                            py = 0;
                            break;
                        case 5:
                            px = TEXTURE_SIZE;
                            py = 2 * TEXTURE_SIZE;
                            break;
                    }

                    if (4 * TEXTURE_SIZE > window_w || 3 * TEXTURE_SIZE > window_h) {
                        px = 0;
                        py = 0;
                    }

                    glViewport(px, py, TEXTURE_SIZE, TEXTURE_SIZE);
                    if (4 * TEXTURE_SIZE > window_w || 3 * TEXTURE_SIZE > window_h)
                        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                    glPushMatrix();
                    {
                        if (i < 4) glRotatef(180, 0, 0, 1);

                        switch (i) {
                            case 0:
                                break;                            // pozerame na -z
                            case 1:
                                glRotatef(90, 0, 1, 0);
                                break;        // pozerame na +x
                            case 2:
                                glRotatef(180, 0, 1, 0);
                                break;    // pozerame na +z
                            case 3:
                                glRotatef(270, 0, 1, 0);
                                break;    // pozerame na -x
                            case 4:
                                glRotatef(-90, 1, 0, 0);
                                break;    // pozerame na +y
                            case 5:
                                glRotatef(90, 1, 0, 0);
                                break;        // pozerame na -y
                        }

                        //render

                        glTranslatef(-file.object[u].center.x, -file.object[u].center.y, -file.object[u].center.z);
                        frustum.Update();

                        glDisable(GL_LIGHTING);
                        skybox.Render();
                        skybox.RenderSun();
                        glEnable(GL_LIGHTING);
                        glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

                        glDisable(GL_CULL_FACE);
                        //glCullFace(GL_FRONT);
                        glEnable(GL_TEXTURE_2D);
                        for (int o = 0; o < file.iNumObjects; o++) {
                            if (o == u) continue;
                            if (!isRefractive(u) && isReflectableOrMatt(o)) continue;
                            if (isRefractive(u) && isRefractive(o)) continue;
                            if (isReflectableOrMatt(u) && isRefractive(o)) continue;

                            glBindTexture(GL_TEXTURE_2D, file.object[o].uDiffuseMap);
                            glEnableClientState(GL_VERTEX_ARRAY);
                            glVertexPointer(3, GL_FLOAT, 0, &file.object[o].vertex[0]);

                            glEnableClientState(GL_NORMAL_ARRAY);
                            glNormalPointer(GL_FLOAT, sizeof(vec3), &file.object[o].normal[0]);

                            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                            glTexCoordPointer(2, GL_FLOAT, sizeof(vec2), &file.object[o].texcoord[0]);

                            glDrawElements(GL_TRIANGLES, file.object[o].iNumFaces * 3, GL_UNSIGNED_INT,
                                           &file.object[o].face[0]);

                            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
                            glDisableClientState(GL_NORMAL_ARRAY);
                            glDisableClientState(GL_VERTEX_ARRAY);
                        }
                        glDisable(GL_CULL_FACE);

                        switch (i) {
                            case 0:
                                pos = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB;
                                break;
                            case 1:
                                pos = GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB;
                                break;
                            case 2:
                                pos = GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB;
                                break;
                            case 3:
                                pos = GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB;
                                break;
                            case 4:
                                pos = GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB;
                                break;
                            case 5:
                                pos = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB;
                                break;
                        }

                        glEnable(GL_TEXTURE_CUBE_MAP_ARB);
                        glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, CubeMapID);

                        glCopyTexSubImage2D(pos, 0, 0, 0, 0, 0, TEXTURE_SIZE, TEXTURE_SIZE);

                        glDisable(GL_TEXTURE_CUBE_MAP_ARB);
                    }
                    glPopMatrix();
                }
                resize(window_w, window_h);
            }
            glPopMatrix();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            char *finalName = new char[1024];
            finalName = strtok(file.filepath, ".");
            if (!finalName) finalName = "default";

            mkdir(finalName);
            char finalPath[1024];
            sprintf((char *) &finalPath, "%s/%s_cubemap.xmc", finalName, file.object[u].name);
            printf("ok: rendered cubemap saved as %s!\n", finalPath);

            FILE *pFile = fopen(finalPath, "wb");
            if (!pFile) {
                printf("error: cannot create cubemap %s!", finalPath);
                return;
            }
            char *header = "#XMC FileFormat By Jatro 8D\n#XMC is an fileformat for XMEngine (c) Jatro!\0LXMC21\0";

            fwrite(header, 1, 81, pFile);

            int finalImgSize = TEXTURE_SIZE * TEXTURE_SIZE * 3 * sizeof(unsigned char) * 6;

            fwrite(&finalImgSize, 1, sizeof(int), pFile);

            int imgSize = TEXTURE_SIZE * TEXTURE_SIZE * 3 * sizeof(unsigned char);
            unsigned char *buffer = new unsigned char[imgSize];

            for (int nFace = 0; nFace < 6; nFace++) {
                glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X + nFace, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer);
                fwrite(buffer, imgSize, 1, pFile);
            }

            delete[] buffer;

            fclose(pFile);
            glutSwapBuffers();
        }
    }
    _end:

    glutDestroyWindow(windowID);
    system("PAUSE");
    exit(0);
}
