#include "collset.h"
#include <stdlib.h>
#include <math.h>

CollNode::CollNode() {
    triangle = NULL;
    front = NULL;
    back = NULL;
}

CollNode::~CollNode() {
    if (front != NULL) delete front;
    if (back != NULL) delete back;

    Triangle *curr = triangle;
    while (curr != NULL) {
        Triangle *next = curr->next;
        delete curr;
        curr = next;
    }
}

#define DIFF_IN_PLANE 0.05f

void CollNode::build(Set<Triangle *> &triangles, unsigned int cutWeight, unsigned int diffWeight,
                     unsigned int coplanarWeight) {

    // Try to find the "best" triangle plane to split along
    unsigned int best = 0xFFFFFFFF;
    unsigned int bestIndex = 0;

    for (unsigned int i = 0; i < triangles.getCount(); i++) {
        vec3 norm = Normalize(Cross(triangles[i]->v[1] - triangles[i]->v[0], triangles[i]->v[2] - triangles[i]->v[0]));
        float off = -Dot(norm, triangles[i]->v[0]);

        unsigned int value = 0;
        int diff = 0;
        for (unsigned int j = 0; j < triangles.getCount(); j++) {
            unsigned int pos = 0;
            unsigned int neg = 0;

            for (unsigned int k = 0; k < 3; k++) {
                float d = Dot(triangles[j]->v[k], norm) + off;
                if (fabsf(d) > DIFF_IN_PLANE) {
                    if (d > 0) pos++; else neg++;
                }
            }

            if (pos) {
                if (neg) value += cutWeight; else diff += diffWeight;
            } else {
                if (neg) diff -= diffWeight; else value += coplanarWeight;
            }
        }
        value += abs(diff);

        if (value < best) {
            best = value;
            bestIndex = i;
        }
    }

    // The "best" plane
    normal = Normalize(Cross(triangles[bestIndex]->v[1] - triangles[bestIndex]->v[0],
                             triangles[bestIndex]->v[2] - triangles[bestIndex]->v[0]));
    offset = -Dot(normal, triangles[bestIndex]->v[0]);

    Set<Triangle *> frontTris;
    Set<Triangle *> backTris;

    // Split into back and front list, and a list of triangles that are in the plane
    for (unsigned int j = 0; j < triangles.getCount(); j++) {
        float d[3];
        unsigned int pos = 0;
        unsigned int neg = 0;
        for (unsigned int k = 0; k < 3; k++) {
            d[k] = Dot(triangles[j]->v[k], normal) + offset;
            if (fabsf(d[k]) > DIFF_IN_PLANE) {
                if (d[k] > 0) pos++; else neg++;
            }
        }

        if (pos && neg) { // Plane cuts the triangle
            frontTris.add(triangles[j]);
            backTris.add(
                    new Triangle(triangles[j]->v[0], triangles[j]->v[1], triangles[j]->v[2], triangles[j]->auxData));

        } else if (pos) { // Front triangle
            frontTris.add(triangles[j]);
        } else if (neg) { // Back triangle
            backTris.add(triangles[j]);
        } else { // Triangle in the plane
            triangles[j]->next = triangle;
            triangle = triangles[j];
        }

    }

    if (frontTris.getCount() > 0) {
        front = new CollNode();
        front->build(frontTris, cutWeight, diffWeight, coplanarWeight);
    }

    if (backTris.getCount() > 0) {
        back = new CollNode();
        back->build(backTris, cutWeight, diffWeight, coplanarWeight);
    }
}

bool CollNode::intersect(vec3 v0, vec3 v1, vec3 *point, bool solid, void **auxData) {
    float d0 = Dot(normal, v0) + offset;
    float d1 = Dot(normal, v1) + offset;

    if (d0 > 0) {
        if (front != NULL && front->intersect(v0, v1, point, solid, auxData)) return true;
        if (d1 < 0) {
            if (isect(v0, v1, point, auxData)) return true;
            if (back != NULL && back->intersect(v0, v1, point, solid, auxData)) return true;
        }
    } else {
        if (back != NULL && back->intersect(v0, v1, point, solid, auxData)) return true;
        if (d1 > 0) {
            if (!solid && isect(v0, v1, point, auxData)) return true;
            if (front != NULL && front->intersect(v0, v1, point, solid, auxData)) return true;
        }
    }

    return false;
}

bool CollNode::isect(vec3 v0, vec3 v1, vec3 *point, void **auxData) {
    Triangle *curr = triangle;

    float k = (Dot(normal, v0) + offset) / Dot(normal, v0 - v1);
    vec3 p = v0 + k * (v1 - v0);

    while (curr != NULL) {
        // Find interpolation constant
        unsigned int prev = 2;
        bool result = true;
        for (unsigned int i = 0; i < 3; i++) {
            vec3 edgeNormal = Cross(normal, curr->v[i] - curr->v[prev]);
            if (Dot(edgeNormal, p) - Dot(edgeNormal, curr->v[i]) < 0) {
                result = false;
                break;
            }
            prev = i;
        }

        if (result) {
            if (point != NULL) *point = p;
            if (auxData != NULL) *auxData = curr->auxData;
            return true;
        }

        curr = curr->next;
    }
    return false;
}

bool CollNode::pushSphere(vec3 &pos, float radius) {
    float d = Dot(pos, normal) + offset;

    bool pushed = false;
    if (fabsf(d) < radius) {
        Triangle *above = aboveTriangle(pos);

        if (above != NULL) {
            pos += (radius - d) * normal;
            pushed = true;
        }
        /*Triangle *curr = triangle;

        while (curr != NULL){



            curr = curr->next;
        }*/
    }

    if (front != NULL && d > -radius) pushed |= front->pushSphere(pos, radius);
    if (back != NULL && d < radius) pushed |= back->pushSphere(pos, radius);

    return pushed;
}

Triangle *CollNode::aboveTriangle(vec3 &pos) {
    Triangle *curr = triangle;

    while (curr != NULL) {

        unsigned int prev = 2;
        bool result = true;
        for (unsigned int i = 0; i < 3; i++) {
            vec3 edgeNormal = Cross(normal, curr->v[i] - curr->v[prev]);
            if (Dot(edgeNormal, pos) - Dot(edgeNormal, curr->v[i]) < 0) {
                result = false;
                break;
            }
            prev = i;
        }

        if (result) return curr;

        curr = curr->next;
    }

    return NULL;
}


CollisionSet::CollisionSet() {
    top = NULL;
}


CollisionSet::~CollisionSet() {
    if (top != NULL) delete top;
}

void CollisionSet::build(unsigned int cutWeight, unsigned int diffWeight, unsigned int coplanarWeight) {
    if (top != NULL) delete top;

    if (triangles.getCount() > 0) {
        top = new CollNode();
        top->build(triangles, cutWeight, diffWeight, coplanarWeight);

        triangles.clear();
    }
}

bool CollisionSet::intersect(vec3 v0, vec3 v1, vec3 *point, bool solid, void **auxData) {
    if (top != NULL) return top->intersect(v0, v1, point, solid, auxData);
    return false;
}

bool CollisionSet::pushSphere(vec3 &pos, float radius) {
    if (top != NULL) return top->pushSphere(pos, radius);

    return false;
}
