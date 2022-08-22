#ifndef _COLLSET_H_
#define _COLLSET_H_

#include "set.h"
#include "math/vector.h"

struct Triangle {
    Triangle(vec3 v0, vec3 v1, vec3 v2, void *aData) {
        v[0] = v0;
        v[1] = v1;
        v[2] = v2;
        auxData = aData;
        //next = NULL;
    }

    vec3 v[3];
    Triangle *next;
    void *auxData;
};


class CollNode {
public:
    CollNode();

    ~CollNode();

    void
    build(Set<Triangle *> &triangles, unsigned int cutWeight, unsigned int diffWeight, unsigned int coplanarWeight);

    bool intersect(vec3 v0, vec3 v1, vec3 *point = NULL, bool solid = true, void **auxData = NULL);

    bool pushSphere(vec3 &pos, float radius);

    Triangle *aboveTriangle(vec3 &pos);

protected:
    bool isect(vec3 v0, vec3 v1, vec3 *point, void **auxData);

    vec3 normal;
    float offset;

    CollNode *front;
    CollNode *back;

    Triangle *triangle;
};


class CollisionSet {
public:
    CollisionSet();

    ~CollisionSet();

    void addTriangle(vec3 v0, vec3 v1, vec3 v2, void *auxData = NULL) {
        triangles.add(new Triangle(v0, v1, v2, auxData));
    }

    void build(unsigned int cutWeight = 3, unsigned int diffWeight = 1, unsigned int coplanarWeight = 2);

    bool intersect(vec3 v0, vec3 v1, vec3 *point = NULL, bool solid = true, void **auxData = NULL);

    bool pushSphere(vec3 &pos, float radius);

protected:
    Set<Triangle *> triangles;
    CollNode *top;
};


#endif // _COLLISIONSET_H_
