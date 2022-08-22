/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#ifndef _VECTOR_H_INCLUDED
#define _VECTOR_H_INCLUDED

#include <math.h>

#define PI    3.1415926535897932384626433832795
#define DEG    PI/180


#ifndef min
#define min(x, y) ((x < y)? x : y)
#endif

#ifndef max
#define max(x, y) ((x > y)? x : y)
#endif

struct xmFace {
    unsigned int a, b, c;
};

struct color3f {
    float r, g, b;
};

class vec2;

class vec3 {
public:

    vec3() {}

    vec3(float X, float Y, float Z) {
        x = X;
        y = Y;
        z = Z;
    }

    vec3 operator+(vec3 vec) {
        return vec3(vec.x + x, vec.y + y, vec.z + z);
    }

    vec3 operator-(vec3 vec) {
        return vec3(x - vec.x, y - vec.y, z - vec.z);
    }

    vec3 operator-(float s) {
        return vec3(x - s, y - s, z - s);
    }

    vec3 operator*(float num) {
        return vec3(x * num, y * num, z * num);
    }

    vec3 operator*(vec3 vec) {
        return vec3(x * vec.x, y * vec.y, z * vec.z);
    }

    vec3 operator/(float num) {
        return vec3(x / num, y / num, z / num);
    }

    vec3 operator/(vec3 vec) {
        return vec3(x / vec.x, y / vec.y, z / vec.z);
    }

    vec3 operator-() {
        return vec3(-x, -y, -z);
    }

    friend vec3 operator*(float k, const vec3 V) {
        return vec3(V.x * k, V.y * k, V.z * k);
    }

    vec3 operator/=(float num) {
        return vec3(x /= num, y /= num, z /= num);
    }

    vec3 operator/=(vec3 vec) {
        return vec3(x /= vec.x, y /= vec.y, z /= vec.z);
    }

    vec3 operator*=(float num) {
        return vec3(x *= num, y *= num, z *= num);
    }

    vec3 operator+=(vec3 vec) {
        return vec3(x += vec.x, y += vec.y, z += vec.z);
    }

    vec3 operator-=(vec3 vec) {
        return vec3(x -= vec.x, y -= vec.y, z -= vec.z);
    }

    vec3 operator-=(float num) {
        return vec3(x -= num, y -= num, z -= num);
    }

    bool operator==(vec3 v1) {
        if (x != v1.x) return false;
        if (y != v1.y) return false;
        if (z != v1.z) return false;

        return true;
    };

    bool operator!=(vec3 v1) {
        if (x == v1.x) return false;
        if (y == v1.y) return false;
        if (z == v1.z) return false;

        return true;
    };


    float x, y, z;
};


class vec2 {
public:

    vec2() {}

    vec2(float X, float Y) {
        x = X;
        y = Y;
    }

    vec2 operator+(vec2 vec) {
        return vec2(vec.x + x, vec.y + y);
    }

    vec2 operator-(vec2 vec) {
        return vec2(x - vec.x, y - vec.y);
    }

    vec2 operator*(float num) {
        return vec2(x * num, y * num);
    }

    vec2 operator/(float num) {
        return vec2(x / num, y / num);
    }

    float x, y;
};

class vec2i {
public:

    vec2i() {}

    vec2i(int X, int Y) {
        x = X;
        y = Y;
    }

    vec2i operator+(vec2i vec) {
        return vec2i(vec.x + x, vec.y + y);
    }

    vec2i operator-(vec2i vec) {
        return vec2i(x - vec.x, y - vec.y);
    }

    vec2i operator*(int num) {
        return vec2i(x * num, y * num);
    }

    vec2i operator/(int num) {
        return vec2i(x / num, y / num);
    }

    int x, y;
};


float Absolute(float num);

float pow(float pow);

vec3 Compute2Vectors(vec3 vStart, vec3 vEnd);

vec3 Cross(vec3 vec1, vec3 vec2);

float VectorLength(vec3 vNormal);

vec3 Normalize(vec3 vec);

vec3 Normal(vec3 vPolygon[]);

float Distance(vec3 vPoint1, vec3 vPoint2);

float Dot(vec3 vec1, vec3 vec2);

bool isZeroVector(vec3 v);

bool signum(float value);

inline float clamp(const float v, const float c0, const float c1) {
    return min(max(v, c0), c1);
}

#define saturate(x) clamp(x, 0, 1)

inline unsigned int getUpperPowerOfTwo(const unsigned int x) {
    unsigned int i = 1;
    while (i < x) i += i;

    return i;
}


inline void
xmCalculateNTB(vec3 VertA, vec3 VertB, vec3 VertC, vec2 UvA, vec2 UvB, vec2 UvC, vec3 *vNormal, vec3 *vBitangent,
               vec3 *vTangent, xmFace face) {
    vec3 tangent;
    vec3 bitangent;
    vec3 normal;

    vec3 v_Edge1 = VertB - VertA;
    vec3 v_Edge2 = VertC - VertA;
    vec2 v_Edge1uvs = UvB - UvA;
    vec2 v_Edge2uvs = UvC - UvA;

    float cp = v_Edge1uvs.y * v_Edge2uvs.x - v_Edge1uvs.x * v_Edge2uvs.y;

    if (cp != 0.0f) {
        float mul = 1.0f / cp;
        tangent = Normalize((v_Edge1 * -v_Edge2uvs.y + v_Edge2 * v_Edge1uvs.y) * mul);
        bitangent = Normalize((v_Edge1 * -v_Edge2uvs.x + v_Edge2 * v_Edge1uvs.x) * mul);
        normal = Normalize(Cross(v_Edge1, v_Edge2));

        vNormal[face.a].x = normal.x;
        vNormal[face.a].y = normal.y;
        vNormal[face.a].z = normal.z;

        vNormal[face.b].x = normal.x;
        vNormal[face.b].y = normal.y;
        vNormal[face.b].z = normal.z;

        vNormal[face.c].x = normal.x;
        vNormal[face.c].y = normal.y;
        vNormal[face.c].z = normal.z;

        vBitangent[face.a].x = bitangent.x;
        vBitangent[face.a].y = bitangent.y;
        vBitangent[face.a].z = bitangent.z;

        vBitangent[face.b].x = bitangent.x;
        vBitangent[face.b].y = bitangent.y;
        vBitangent[face.b].z = bitangent.z;

        vBitangent[face.c].x = bitangent.x;
        vBitangent[face.c].y = bitangent.y;
        vBitangent[face.c].z = bitangent.z;

        vTangent[face.a].x = tangent.x;
        vTangent[face.a].y = tangent.y;
        vTangent[face.a].z = tangent.z;

        vTangent[face.b].x = tangent.x;
        vTangent[face.b].y = tangent.y;
        vTangent[face.b].z = tangent.z;

        vTangent[face.c].x = tangent.x;
        vTangent[face.c].y = tangent.y;
        vTangent[face.c].z = tangent.z;
    }
}


#endif
