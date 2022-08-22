/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#include "main.h"
//#define _info
#define _basic
#define SAMPLE_COUNT 1

CollisionSet coll;
CubemapRenderer cm;
Texture texture;

xmLevel::xmLevel() {
    iNumEmptyObjects = 0;
}

bool xmLevel::xmLoadBinaryXMLevel(char *xmLevelPath) {
    strcpy(filepath, xmLevelPath);

    FILE *pFile = fopen(xmLevelPath, "rb");
    if (!pFile) {
        printf("error: file %s not found! can't open xmlevel file!\n", xmLevelPath);
        return false;
    }
    printf("ok: loading header!\n");
    fread(headerTitle, 1, 74, pFile);
    fread(header, 1, 7, pFile);

    if (strcmp(header, "LXML19")) {
        printf("error: file %s is not valid xmLevel file!\n");
        return false;
    }
    fread(&iNumLights, 1, sizeof(short int), pFile);
    //printf("%d\n", iNumLights);
    printf("ok: loading mainlight!\n");
    fread(&mainLight.color, 1, sizeof(color3f), pFile);
    fread(&mainLight.center, 1, sizeof(vec3), pFile);
    fread(&mainLight.radius, 1, sizeof(float), pFile);
    //printf("%f %f %f %f %f %f %f", mainLight.color.r, mainLight.color.g, mainLight.color.b, mainLight.center.x,mainLight.center.y,mainLight.center.z, mainLight.radius);

    printf("ok: loading %d area lights!\n", iNumLights);
    areaLight = new xmlLight[iNumLights];
    if (!areaLight) return false;

    for (int l = 0; l < iNumLights; l++) {
        fread(&areaLight[l].type, 1, sizeof(short int), pFile);
        //printf("%d\n", areaLight[l].type);

        switch (areaLight[l].type) {
            case 0: {
                printf("ok: loading spotlight %d!\n", l);
                fread(&areaLight[l].falloff, 1, sizeof(float), pFile);
                fread(&areaLight[l].hotspot, 1, sizeof(float), pFile);
                fread(&areaLight[l].color, 1, sizeof(color3f), pFile);
                fread(&areaLight[l].center, 1, sizeof(vec3), pFile);
                fread(&areaLight[l].target, 1, sizeof(vec3), pFile);
            }
                break;

            case 1: {
                printf("ok: loading pointlight %d!\n", l);
                fread(&areaLight[l].color, 1, sizeof(color3f), pFile);
                fread(&areaLight[l].center, 1, sizeof(vec3), pFile);
                fread(&areaLight[l].radius, 1, sizeof(float), pFile);
            }
                break;

            default:
                printf("error: bad light type!\n");
                return false;
                break;
        }
    }

    //printf("ok: loading %d player spawns!\n", iNumSpawns);
    fread(&iNumSpawns, 1, sizeof(short int), pFile);
    spawn = new xmlEntity[iNumSpawns];
    if (!spawn) return false;

    for (int s = 0; s < iNumSpawns; s++) {
        short int size = 0;
        fread(&size, 1, sizeof(short int), pFile);
        fread(spawn[s].name, 1, size + 1, pFile);
#ifdef _info
        printf("%s %d\n",spawn[s].name, size);
#endif
        fread(&spawn[s].center, 1, sizeof(vec3), pFile);
#ifdef _info
        printf("%f %f %f\n",spawn[s].center.x, spawn[s].center.y, spawn[s].center.z);
#endif
    }

    printf("ok: loading entites!\n");
    fread(&iNumEntities, 1, sizeof(short int), pFile);
    entity = new xmlEntity[iNumEntities];
    if (!entity) return false;
    //printf("%d\n", iNumEntities);

    for (int e = 0; e < iNumEntities; e++) {
        short int lnght = 0;
        fread(&lnght, 1, sizeof(short int), pFile);
        fread(entity[e].name, 1, lnght + 1, pFile);
#ifdef _info
        printf("%s %d\n",entity[e].name,lnght);
#endif
        fread(&entity[e].rotation, 1, sizeof(vec3), pFile);
        fread(&entity[e].center, 1, sizeof(vec3), pFile);
#ifdef _info
        printf("%f %f %f\n",entity[e].center.x,entity[e].center.y,entity[e].center.z);
#endif
    }

    fread(&iNumObjects, 1, sizeof(short int), pFile);
    fread(&iSkyNum, 1, sizeof(short int), pFile);
#ifdef _info
    for(int l=0; l < iNumLights; l++) printf("%f %f %f, color %f %f %f\n", light[l].center.x, light[l].center.y, light[l].center.z, light[l].color.r, light[l].color.g, light[l].color.b);
    printf("%s\n", headerTitle);
    printf("iNumObjects: %d iSkyNum: %d\n", iNumObjects, iSkyNum);
#endif

    printf("ok: loading %d level objects!\n", iNumObjects);
    //printf("%d\n", iNumObjects);
    if (iNumObjects <= 0) return false;
    object = new xmlObject[iNumObjects];
    if (!object) return false;

    for (int o = 0; o < iNumObjects; o++) {
        short int lnght = 0;
        fread(&lnght, 1, sizeof(short int), pFile);
        fread(object[o].name, 1, lnght + 1, pFile);
#ifdef _info
        printf("%s %d %d\n",object[o].name, strlen(object[o].name), o);
#endif
        fread(&object[o].iNumVertices, 1, sizeof(long int), pFile);
        fread(&object[o].iNumFaces, 1, sizeof(long int), pFile);

        if (object[o].iNumFaces <= 0 || object[o].iNumVertices <= 0) {
            printf("warning: empty object \"%s\" on index %d\n", object[o].name, o);
            iNumEmptyObjects++;
            continue;
        }

#ifdef _info
        printf("%d %d\n",object[o].iNumVertices, object[o].iNumFaces);
#endif

        object[o].vertex = new vec3[object[o].iNumVertices];
        object[o].texcoord = new vec2[object[o].iNumVertices];
        object[o].normal = new vec3[object[o].iNumVertices];
        object[o].tangent = new vec3[object[o].iNumVertices];
        object[o].bitangent = new vec3[object[o].iNumVertices];
        object[o].face = new xmFace[object[o].iNumFaces];

        if (!object[o].vertex || !object[o].texcoord || !object[o].face || !object[o].normal || !object[o].tangent ||
            !object[o].bitangent) {
            printf("error: cannot alloc. memory for vertex, texcoords and things about it for xmLevel!\n");
            return false;
        }

        for (int v = 0; v < object[o].iNumVertices; v++) {
            fread(&object[o].vertex[v], 1, sizeof(vec3), pFile);
            fread(&object[o].texcoord[v], 1, sizeof(vec2), pFile);
#ifdef _info
            printf("%f %f %f  %f %f\n", object[o].vertex[v].x, object[o].vertex[v].y, object[o].vertex[v].z, object[o].texcoord[v].x, object[o].texcoord[v].y);
#endif
        }
        //readnu kompletne fsechny indice vertexu
        fread(object[o].face, object[o].iNumFaces, sizeof(xmFace), pFile);
#ifdef _info
        for(int f=0; f < object[o].iNumFaces; f++) printf("%d %d %d\n",object[o].face[f].a, object[o].face[f].b, object[o].face[f].c);
#endif
        fread(&lnght, 1, sizeof(short int), pFile);
        fread(object[o].texturePath, 1, lnght + 1, pFile);
        fread(&object[o].textureFilter, 1, sizeof(short int), pFile);
        fread(&object[o].opacity, 1, sizeof(float), pFile);
        fread(&object[o].shininess, 1, sizeof(float), pFile);
#ifdef _info
        printf("%s %d %f\n",object[o].texturePath, object[o].textureFilter, object[o].opacity);
#endif

        for (int i = 0; i < object[o].iNumFaces; i++) {

            vec3 v0 = object[o].vertex[object[o].face[i].a];
            vec3 v1 = object[o].vertex[object[o].face[i].b];
            vec3 v2 = object[o].vertex[object[o].face[i].c];
            //coll.addTriangle(v0, v1, v2);
            xmCalculateNTB(object[o].vertex[object[o].face[i].a], object[o].vertex[object[o].face[i].b],
                           object[o].vertex[object[o].face[i].c], object[o].texcoord[object[o].face[i].a],
                           object[o].texcoord[object[o].face[i].b], object[o].texcoord[object[o].face[i].c],
                           object[o].normal, object[o].bitangent, object[o].tangent, object[o].face[i]);

            if (object[o].min.x == 0) object[o].min.x = 999999.9f;
            if (object[o].max.x == 0) object[o].max.x = -999999.9f;
            if (object[o].min.z == 0) object[o].min.z = 999999.9f;
            if (object[o].max.z == 0) object[o].max.z = -999999.9f;
            if (object[o].min.y == 0) object[o].min.y = 999999.9f;
            if (object[o].max.y == 0) object[o].max.y = -999999.9f;

            if (worldMin.x == 0) worldMin.x = 999999.9f;
            if (worldMax.x == 0) worldMax.x = -999999.9f;
            if (worldMin.z == 0) worldMin.z = 999999.9f;
            if (worldMax.z == 0) worldMax.z = -999999.9f;
            if (worldMin.y == 0) worldMin.y = 999999.9f;
            if (worldMax.y == 0) worldMax.y = -999999.9f;

            int ind[3];
            ind[0] = object[o].face[i].a;
            ind[1] = object[o].face[i].b;
            ind[2] = object[o].face[i].c;

            for (int v = 0; v < 3; v++) {
                if (object[o].vertex[ind[v]].x < object[o].min.x) object[o].min.x = object[o].vertex[ind[v]].x - 2.0f;
                if (object[o].vertex[ind[v]].y < object[o].min.y) object[o].min.y = object[o].vertex[ind[v]].y - 2.0f;
                if (object[o].vertex[ind[v]].z < object[o].min.z) object[o].min.z = object[o].vertex[ind[v]].z - 2.0f;

                if (object[o].vertex[ind[v]].x > object[o].max.x) object[o].max.x = object[o].vertex[ind[v]].x + 2.0f;
                if (object[o].vertex[ind[v]].y > object[o].max.y) object[o].max.y = object[o].vertex[ind[v]].y + 2.0f;
                if (object[o].vertex[ind[v]].z > object[o].max.z) object[o].max.z = object[o].vertex[ind[v]].z + 2.0f;

                if (object[o].vertex[ind[v]].x < worldMin.x) worldMin.x = object[o].vertex[ind[v]].x - 2.0f;
                if (object[o].vertex[ind[v]].y < worldMin.y) worldMin.y = object[o].vertex[ind[v]].y - 2.0f;
                if (object[o].vertex[ind[v]].z < worldMin.z) worldMin.z = object[o].vertex[ind[v]].z - 2.0f;

                if (object[o].vertex[ind[v]].x > worldMax.x) worldMax.x = object[o].vertex[ind[v]].x + 2.0f;
                if (object[o].vertex[ind[v]].y > worldMax.y) worldMax.y = object[o].vertex[ind[v]].y + 2.0f;
                if (object[o].vertex[ind[v]].z > worldMax.z) worldMax.z = object[o].vertex[ind[v]].z + 2.0f;
            }
        }

        object[o].center = ((object[o].max + object[o].min) / 2);
        object[o].radius = Distance(object[o].max, object[o].center);

        worldCenter = ((worldMax + worldMin) / 2);
        worldRadius = Distance(worldMax, worldCenter);

    }
    fclose(pFile);

#ifdef _LIGHTMAP
    TexturePacker texPacker;
    for(int i = 0; i < iNumObjects; i++)
    {
        vec3 * src =  object[i].vertex;
        unsigned int *inds = (unsigned int*)object[i].face;

        for (int j = 0; j < object[i].iNumFaces; j += 6)
        {
            float w = VectorLength(src[inds[j + 1]] - src[inds[j]]);
            float h = VectorLength(src[inds[j + 5]] - src[inds[j]]);

            if (w < 8) w = 8;
            if (h < 8) h = 8;

            unsigned int tw = (int) w;
            unsigned int th = (int) h;

            tw = (tw + 4) & ~7;
            th = (th + 4) & ~7;

            texPacker.addRectangle(tw, th);
        }
    }

    unsigned int size = 2, tw, th;
    while(!texPacker.assignCoords(&tw, &th))
    {
        size *= 2;
        tw = th = size;
    }

    tw = getUpperPowerOfTwo(tw);
    th = getUpperPowerOfTwo(th);
    printf("ok: generating lightmap coordinates!\n");

    Set <PolyData *> polyData;

    unsigned int index = 0;
    for (int i = 0; i < iNumObjects; i++)
    {
        vec3 * src =  object[i].vertex;
        unsigned int *inds = (unsigned int *) object[i].face;
        object[i].texcoord2 = new vec2[object[i].iNumVertices];

        for (int j = 0; j < object[i].iNumFaces; j += 6)
        {
            TextureRectangle *rect = texPacker.getRectangle(index);

            float x0 = float(rect->x + 0.5f) / tw;
            float x1 = float(rect->x + rect->width - 0.5f) / tw;
            float y0 = float(rect->y + 0.5f) / th;
            float y1 = float(rect->y + rect->height - 0.5f) / th;

            object[i].texcoord2[inds[j    ]] = vec2(x0, y0);
            object[i].texcoord2[inds[j + 1]] = vec2(x1, y0);
            object[i].texcoord2[inds[j + 2]] = vec2(x1, y1);
            object[i].texcoord2[inds[j + 5]] = vec2(x0, y1);

            PolyData *pd = new PolyData;
            pd->v0 = src[inds[j]];
            pd->vDir0 = src[inds[j + 1]] - pd->v0;
            pd->vDir1 = src[inds[j + 5]] - pd->v0;
            pd->t0 = vec2(x0, y0);
            pd->tDir0 = vec2(x1 - x0, 0);
            pd->tDir1 = vec2(0, y1 - y0);
            polyData.add(pd);

            coll.addTriangle(src[inds[j]], src[inds[j + 1]], src[inds[j + 2]], pd);
            coll.addTriangle(src[inds[j + 3]], src[inds[j + 4]], src[inds[j + 5]], pd);

            index++;
        }
    }
    printf("ok: building tree!\n");
    coll.build(3, 1, 50);

    printf("ok: generating %d samples!\n", SAMPLE_COUNT);

    vec3 samples[SAMPLE_COUNT];
    for (unsigned int x = 0; x < SAMPLE_COUNT; x++){
        vec3 v;
        do {
            v = vec3(float(rand()), float(rand()), float(rand())) * (2.0f / RAND_MAX) - 1.0f;
        } while (Dot(v, v) > 1);
        samples[x] = v ;
    }

    printf("ok: generating lightmap - %dx%d!\n", tw, th);

    unsigned char *lMap = new unsigned char[tw * th * 3];
    memset(lMap, 0, tw * th * 3);
    // Pass 1, illuminate using the main light
    index = 0;
    for (int i = 0; i < iNumObjects; i++)
    {

        vec3 * src = object[i].vertex;
        unsigned int *inds = (unsigned int *) object[i].face;
        for (int j = 0; j < object[i].iNumFaces; j += 6)
        {
            TextureRectangle *rect = texPacker.getRectangle(index);

            vec3 dirS = (src[inds[j + 1]] - src[inds[j]]);
            vec3 dirT = (src[inds[j + 5]] - src[inds[j]]);

            vec3 pos = src[inds[j]];

            for (int t = 0; t < int(rect->height); t++)
            {
                for (int s = 0; s < int(rect->width); s++)
                {
                    vec3 samplePos = pos + saturate((float(s) / (rect->width - 1))) * dirS + saturate((float(t) / (rect->height - 1))) * dirT;

                    float shadow = 0.2;

                    coll.pushSphere(samplePos, 5);

                    for (unsigned int k = 0; k < SAMPLE_COUNT; k++)
                    {
                        if (!coll.intersect(samplePos, mainLight.center + samples[k])) shadow += 1.0f / SAMPLE_COUNT;
                    }

                    float light = saturate(shadow);

                    lMap[((rect->y + t) * tw + (rect->x + s)) * 3 +0] = (unsigned char) (255.0f * light);
                    lMap[((rect->y + t) * tw + (rect->x + s)) * 3 +1] = (unsigned char) (255.0f * light);
                    lMap[((rect->y + t) * tw + (rect->x + s)) * 3 +2] = (unsigned char) (255.0f * light);
                }
            }

            index++;
        }

        //Image image;
        //image.loadFromMemory(lMap, tw, th, FORMAT_RGB8, true, false);
        //image.saveImage("LightMap.dds");


        char * finalName = new char[1024];
        finalName = strtok(xmLevelPath,".");
        if(!finalName) finalName = "default";
        char finalPath[1024];
        sprintf((char*)&finalPath,"%s_lightmap.raw", finalName);

        FILE * fp = fopen(finalPath, "w");
        if(!fp)
        {
            printf("error: cannot save lightmap!\n");
            return false;
        }
        int size = tw *th * 3;

        fwrite(&tw, sizeof(unsigned int), 1, fp);
        fwrite(&th, sizeof(unsigned int), 1, fp);
        fwrite(lMap, sizeof(unsigned char), size, fp);

        fclose(fp);
    }
#endif

    if (!xmSaveBinaryXMLevel(xmLevelPath)) {
        printf("error: cannot save XML %s file!\n", xmLevelPath);
        return false;
    }


    cm.initWindow();

    return true;
}

bool xmLevel::xmSaveBinaryXMLevel(char *xmLevelPath) {
    char *finalName = new char[1024];
    finalName = strtok(xmLevelPath, ".");
    if (!finalName) finalName = "xma";
    char finalPath[1024];
    sprintf((char *) &finalPath, "%s_export.xml", finalName);

    FILE *pFile = fopen(finalPath, "wb");
    if (!pFile) {
        printf("error: can't create %s xmlevel file!\n", finalPath);
        return false;
    }
    printf("ok: writing XML header!\n");
    fwrite(headerTitle, 1, 74, pFile);
    fwrite(header, 1, 7, pFile);

    fwrite(&iNumLights, 1, sizeof(short int), pFile);
    printf("ok: writing mainlight!\n");
    fwrite(&mainLight, 1, sizeof(xmlLight), pFile);

    printf("ok: writing area lights!\n");
    fwrite(areaLight, iNumLights, sizeof(xmlLight), pFile);

    printf("ok: writing %d player spawns!\n", iNumSpawns);
    fwrite(&iNumSpawns, 1, sizeof(short int), pFile);
    for (int s = 0; s < iNumSpawns; s++) {
        short int size = strlen(spawn[s].name) + 1;
        fwrite(&size, 1, sizeof(short int), pFile);
        fwrite(spawn[s].name, 1, size, pFile);
        fwrite(&spawn[s].center, 1, sizeof(vec3), pFile);
    }

    printf("ok: writing %d entities!\n", iNumEntities);
    fwrite(&iNumEntities, 1, sizeof(short int), pFile);
    for (int e = 0; e < iNumEntities; e++) {
        short int size = strlen(entity[e].name) + 1;
        fwrite(&size, 1, sizeof(short int), pFile);
        fwrite(entity[e].name, 1, size, pFile);
        fwrite(&entity[e].rotation, 1, sizeof(vec3), pFile);
        fwrite(&entity[e].center, 1, sizeof(vec3), pFile);
        entity[e].min = vec3(-26.08f, -8.35f, -5.16f);
        entity[e].max = vec3(12.60f, 0.0f, 6.151f);
        entity[e].radius = 20.58f;

        fwrite(&entity[e].min, 1, sizeof(vec3), pFile);
        fwrite(&entity[e].max, 1, sizeof(vec3), pFile);
        fwrite(&entity[e].radius, 1, sizeof(float), pFile);
    }

    iNumObjects -= iNumEmptyObjects;
    printf("ok: writing %d objects of level!\n", iNumObjects);

    fwrite(&iNumObjects, 1, sizeof(short int), pFile);
    fwrite(&iSkyNum, 1, sizeof(short int), pFile);

    fwrite(&worldMin, 1, sizeof(vec3), pFile);
    fwrite(&worldMax, 1, sizeof(vec3), pFile);
    fwrite(&worldCenter, 1, sizeof(vec3), pFile);
    fwrite(&worldRadius, 1, sizeof(float), pFile);

    short int size = 0; //strlen(cMeshPath)+1;
    //fwrite(&size, 1, sizeof(short int), pFile);
    //fwrite(cMeshPath, 1, size, pFile);

    for (int o = 0; o < iNumObjects; o++) {
        short int size = strlen(object[o].name) + 1;
        fwrite(&size, 1, sizeof(short int), pFile);
        fwrite(object[o].name, 1, size, pFile);

        bool bAlpha = false;
        bool bCollBox = false;
        bool bReflectable = false;
        bool bRefractive = false;
        bool bCollidable = true;

        if (strstr(object[o].name, "alpha") || strstr(object[o].name, "_alfa")) bAlpha = true;
        if (strstr(object[o].name, "coll") || strstr(object[o].name, "Coll")) bCollBox = true;
        if (strstr(object[o].name, "_ign") || strstr(object[o].name, "_IGN")) bCollidable = false;
        if (strstr(object[o].name, "reflectable") || strstr(object[o].name, "_reflectable")) bReflectable = true;
        if (strstr(object[o].name, "refractive") || strstr(object[o].name, "_refractive")) bRefractive = true;

        fwrite(&bAlpha, 1, sizeof(bool), pFile);
        fwrite(&bCollBox, 1, sizeof(bool), pFile);
        fwrite(&bCollidable, 1, sizeof(bool), pFile);
        fwrite(&bReflectable, 1, sizeof(bool), pFile);
        fwrite(&bRefractive, 1, sizeof(bool), pFile);
        fwrite(&object[o].iNumVertices, 1, sizeof(long int), pFile);
        fwrite(&object[o].iNumFaces, 1, sizeof(long int), pFile);

        //printf("ok: writing vertices with tex coords!\n");
        for (int v = 0; v < object[o].iNumVertices; v++) {
            fwrite(&object[o].vertex[v], 1, sizeof(vec3), pFile);
            fwrite(&object[o].texcoord[v], 1, sizeof(vec2), pFile);
        }

        //printf("ok: writing vertex faces!\n");
        fwrite(object[o].face, object[o].iNumFaces, sizeof(xmFace), pFile);

        //printf("ok: writing normals, tagnents and bitangents!\n");
        fwrite(object[o].normal, object[o].iNumVertices, sizeof(vec3), pFile);
        fwrite(object[o].tangent, object[o].iNumVertices, sizeof(vec3), pFile);

        //printf("ok: writing mins, max, center and radius of every object!\n");
        fwrite(&object[o].min, 1, sizeof(vec3), pFile);
        fwrite(&object[o].max, 1, sizeof(vec3), pFile);
        fwrite(&object[o].center, 1, sizeof(vec3), pFile);
        fwrite(&object[o].radius, 1, sizeof(float), pFile);

        if (bCollBox) {
            object[o].BoxVertices[0] = vec3(object[o].min.x, object[o].min.y, object[o].min.z);
            object[o].BoxVertices[1] = vec3(object[o].min.x, object[o].max.y, object[o].min.z);
            object[o].BoxVertices[2] = vec3(object[o].max.x, object[o].max.y, object[o].min.z);

            object[o].BoxVertices[3] = vec3(object[o].min.x, object[o].min.y, object[o].min.z);
            object[o].BoxVertices[4] = vec3(object[o].max.x, object[o].max.y, object[o].min.z);
            object[o].BoxVertices[5] = vec3(object[o].max.x, object[o].min.y, object[o].min.z);

            object[o].BoxVertices[6] = vec3(object[o].min.x, object[o].min.y, object[o].max.z);
            object[o].BoxVertices[7] = vec3(object[o].max.x, object[o].max.y, object[o].max.z);
            object[o].BoxVertices[8] = vec3(object[o].min.x, object[o].max.y, object[o].max.z);

            object[o].BoxVertices[9] = vec3(object[o].min.x, object[o].min.y, object[o].max.z);
            object[o].BoxVertices[10] = vec3(object[o].max.x, object[o].min.y, object[o].max.z);
            object[o].BoxVertices[11] = vec3(object[o].max.x, object[o].max.y, object[o].max.z);

            object[o].BoxVertices[12] = vec3(object[o].min.x, object[o].min.y, object[o].min.z);
            object[o].BoxVertices[13] = vec3(object[o].max.x, object[o].min.y, object[o].max.z);
            object[o].BoxVertices[14] = vec3(object[o].min.x, object[o].min.y, object[o].max.z);

            object[o].BoxVertices[15] = vec3(object[o].min.x, object[o].min.y, object[o].min.z);
            object[o].BoxVertices[16] = vec3(object[o].max.x, object[o].min.y, object[o].min.z);
            object[o].BoxVertices[17] = vec3(object[o].max.x, object[o].min.y, object[o].max.z);

            object[o].BoxVertices[18] = vec3(object[o].min.x, object[o].max.y, object[o].min.z);
            object[o].BoxVertices[19] = vec3(object[o].min.x, object[o].max.y, object[o].max.z);
            object[o].BoxVertices[20] = vec3(object[o].max.x, object[o].max.y, object[o].max.z);

            object[o].BoxVertices[21] = vec3(object[o].min.x, object[o].max.y, object[o].min.z);
            object[o].BoxVertices[22] = vec3(object[o].max.x, object[o].max.y, object[o].max.z);
            object[o].BoxVertices[23] = vec3(object[o].max.x, object[o].max.y, object[o].min.z);

            object[o].BoxVertices[24] = vec3(object[o].min.x, object[o].min.y, object[o].min.z);
            object[o].BoxVertices[25] = vec3(object[o].min.x, object[o].min.y, object[o].max.z);
            object[o].BoxVertices[26] = vec3(object[o].min.x, object[o].max.y, object[o].max.z);

            object[o].BoxVertices[27] = vec3(object[o].min.x, object[o].min.y, object[o].min.z);
            object[o].BoxVertices[28] = vec3(object[o].min.x, object[o].max.y, object[o].max.z);
            object[o].BoxVertices[29] = vec3(object[o].min.x, object[o].max.y, object[o].min.z);

            object[o].BoxVertices[30] = vec3(object[o].max.x, object[o].min.y, object[o].min.z);
            object[o].BoxVertices[31] = vec3(object[o].max.x, object[o].max.y, object[o].max.z);
            object[o].BoxVertices[32] = vec3(object[o].max.x, object[o].min.y, object[o].max.z);

            object[o].BoxVertices[33] = vec3(object[o].max.x, object[o].min.y, object[o].min.z);
            object[o].BoxVertices[34] = vec3(object[o].max.x, object[o].max.y, object[o].min.z);
            object[o].BoxVertices[35] = vec3(object[o].max.x, object[o].max.y, object[o].max.z);
            //object[o].radius *= 2;             
            for (int i = 0; i < 12; i += 3) object[o].BoxNormals[i] = Normalize(
                        Cross(object[o].BoxVertices[i] - object[o].BoxVertices[i + 1],
                              object[o].BoxVertices[i + 1] - object[o].BoxVertices[i + 2]));

            size = 36;
            fwrite(&size, 1, sizeof(short int), pFile);
            fwrite(&object[o].BoxVertices, size, sizeof(vec3), pFile);
            fwrite(&object[o].BoxNormals, size / 3, sizeof(vec3), pFile);
        } else {
            size = 0;
            fwrite(&size, 1, sizeof(short int), pFile);
        }

        //printf("ok: texture paths\n");
        char *heightMap = strtok(object[o].texturePath, ".");
        if (heightMap) {
            char heightmappath[1024];
            sprintf((char *) &heightmappath, "%s_normal.dds", heightMap);
            strcpy(object[o].heightMapPath, heightmappath);
        }

        char *specularMap = strtok(object[o].texturePath, ".");
        if (specularMap) {
            char specularmappath[1024];
            sprintf((char *) &specularmappath, "%s_specular.dds", specularMap);
            strcpy(object[o].specularMapPath, specularmappath);
        }

        char diffusetexturepath[1024];
        sprintf((char *) &diffusetexturepath, "%s.dds", (char *) &(object[o].texturePath));
        strcpy(object[o].texturePath, diffusetexturepath);

        size = strlen(object[o].texturePath) + 1;
        fwrite(&size, 1, sizeof(short int), pFile);
        fwrite(object[o].texturePath, 1, size, pFile);

        size = strlen(object[o].heightMapPath) + 1;
        fwrite(&size, 1, sizeof(short int), pFile);
        fwrite(object[o].heightMapPath, 1, size, pFile);

        size = strlen(object[o].specularMapPath) + 1;
        fwrite(&size, 1, sizeof(short int), pFile);
        fwrite(object[o].specularMapPath, 1, size, pFile);

        fwrite(&object[o].textureFilter, 1, sizeof(short int), pFile);
        fwrite(&object[o].opacity, 1, sizeof(float), pFile);

    }
    fclose(pFile);
    printf("ok: file sucessfully writen as %s!\n", finalPath);
#ifdef _basic
    printf("Svetel: %d Objektu: %d Entit: %d\n", iNumLights, iNumObjects, iNumEntities);
#endif

    return true;
}
