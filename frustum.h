#ifndef _FRUSTUM_H
#define _FRUSTUM_H

#include "./main.h"

class Frustum {
public:
    void Update();

    bool SphereInFrustum(vec3 Center, float Radius);

private:
    float FrustumMatrix[6][4];
    float ClipMatrix[16];
    float ProjectionMatrix[16];
    float ModelviewMatrix[16];
    float t;
};

#endif
