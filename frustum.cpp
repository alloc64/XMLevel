#include "./main.h"

void Frustum::Update() {
    float ClipMatrix[16];
    float ProjectionMatrix[16];
    float ModelviewMatrix[16];

    glGetFloatv(GL_PROJECTION_MATRIX, ProjectionMatrix);
    glGetFloatv(GL_MODELVIEW_MATRIX, ModelviewMatrix);

    ClipMatrix[0] = ModelviewMatrix[0] * ProjectionMatrix[0];
    ClipMatrix[1] = ModelviewMatrix[1] * ProjectionMatrix[5];
    ClipMatrix[2] = ModelviewMatrix[2] * ProjectionMatrix[10] + ModelviewMatrix[3] * ProjectionMatrix[14];
    ClipMatrix[3] = ModelviewMatrix[2] * ProjectionMatrix[11];
    ClipMatrix[4] = ModelviewMatrix[4] * ProjectionMatrix[0];
    ClipMatrix[5] = ModelviewMatrix[5] * ProjectionMatrix[5];
    ClipMatrix[6] = ModelviewMatrix[6] * ProjectionMatrix[10] + ModelviewMatrix[7] * ProjectionMatrix[14];
    ClipMatrix[7] = ModelviewMatrix[6] * ProjectionMatrix[11];
    ClipMatrix[8] = ModelviewMatrix[8] * ProjectionMatrix[0];
    ClipMatrix[9] = ModelviewMatrix[9] * ProjectionMatrix[5];
    ClipMatrix[10] = ModelviewMatrix[10] * ProjectionMatrix[10] + ModelviewMatrix[11] * ProjectionMatrix[14];
    ClipMatrix[11] = ModelviewMatrix[10] * ProjectionMatrix[11];
    ClipMatrix[12] = ModelviewMatrix[12] * ProjectionMatrix[0];
    ClipMatrix[13] = ModelviewMatrix[13] * ProjectionMatrix[5];
    ClipMatrix[14] = ModelviewMatrix[14] * ProjectionMatrix[10] + ModelviewMatrix[15] * ProjectionMatrix[14];
    ClipMatrix[15] = ModelviewMatrix[14] * ProjectionMatrix[11];

    FrustumMatrix[0][0] = ClipMatrix[3] - ClipMatrix[0];
    FrustumMatrix[0][1] = ClipMatrix[7] - ClipMatrix[4];
    FrustumMatrix[0][2] = ClipMatrix[11] - ClipMatrix[8];
    FrustumMatrix[0][3] = ClipMatrix[15] - ClipMatrix[12];

    float t = float(sqrt(FrustumMatrix[0][0] * FrustumMatrix[0][0] + FrustumMatrix[0][1] * FrustumMatrix[0][1] +
                         FrustumMatrix[0][2] * FrustumMatrix[0][2]));

    FrustumMatrix[0][0] /= t;
    FrustumMatrix[0][1] /= t;
    FrustumMatrix[0][2] /= t;
    FrustumMatrix[0][3] /= t;

    FrustumMatrix[1][0] = ClipMatrix[3] + ClipMatrix[0];
    FrustumMatrix[1][1] = ClipMatrix[7] + ClipMatrix[4];
    FrustumMatrix[1][2] = ClipMatrix[11] + ClipMatrix[8];
    FrustumMatrix[1][3] = ClipMatrix[15] + ClipMatrix[12];

    t = float(sqrt(FrustumMatrix[1][0] * FrustumMatrix[1][0] + FrustumMatrix[1][1] * FrustumMatrix[1][1] +
                   FrustumMatrix[1][2] * FrustumMatrix[1][2]));

    FrustumMatrix[1][0] /= t;
    FrustumMatrix[1][1] /= t;
    FrustumMatrix[1][2] /= t;
    FrustumMatrix[1][3] /= t;

    FrustumMatrix[2][0] = ClipMatrix[3] + ClipMatrix[1];
    FrustumMatrix[2][1] = ClipMatrix[7] + ClipMatrix[5];
    FrustumMatrix[2][2] = ClipMatrix[11] + ClipMatrix[9];
    FrustumMatrix[2][3] = ClipMatrix[15] + ClipMatrix[13];

    t = float(sqrt(FrustumMatrix[2][0] * FrustumMatrix[2][0] + FrustumMatrix[2][1] * FrustumMatrix[2][1] +
                   FrustumMatrix[2][2] * FrustumMatrix[2][2]));

    FrustumMatrix[2][0] /= t;
    FrustumMatrix[2][1] /= t;
    FrustumMatrix[2][2] /= t;
    FrustumMatrix[2][3] /= t;

    FrustumMatrix[3][0] = ClipMatrix[3] - ClipMatrix[1];
    FrustumMatrix[3][1] = ClipMatrix[7] - ClipMatrix[5];
    FrustumMatrix[3][2] = ClipMatrix[11] - ClipMatrix[9];
    FrustumMatrix[3][3] = ClipMatrix[15] - ClipMatrix[13];

    t = float(sqrt(FrustumMatrix[3][0] * FrustumMatrix[3][0] + FrustumMatrix[3][1] * FrustumMatrix[3][1] +
                   FrustumMatrix[3][2] * FrustumMatrix[3][2]));

    FrustumMatrix[3][0] /= t;
    FrustumMatrix[3][1] /= t;
    FrustumMatrix[3][2] /= t;
    FrustumMatrix[3][3] /= t;

    FrustumMatrix[4][0] = ClipMatrix[3] - ClipMatrix[2];
    FrustumMatrix[4][1] = ClipMatrix[7] - ClipMatrix[6];
    FrustumMatrix[4][2] = ClipMatrix[11] - ClipMatrix[10];
    FrustumMatrix[4][3] = ClipMatrix[15] - ClipMatrix[14];

    t = float(sqrt(FrustumMatrix[4][0] * FrustumMatrix[4][0] + FrustumMatrix[4][1] * FrustumMatrix[4][1] +
                   FrustumMatrix[4][2] * FrustumMatrix[4][2]));

    FrustumMatrix[4][0] /= t;
    FrustumMatrix[4][1] /= t;
    FrustumMatrix[4][2] /= t;
    FrustumMatrix[4][3] /= t;

    FrustumMatrix[5][0] = ClipMatrix[3] + ClipMatrix[2];
    FrustumMatrix[5][1] = ClipMatrix[7] + ClipMatrix[6];
    FrustumMatrix[5][2] = ClipMatrix[11] + ClipMatrix[10];
    FrustumMatrix[5][3] = ClipMatrix[15] + ClipMatrix[14];

    t = float(sqrt(FrustumMatrix[5][0] * FrustumMatrix[5][0] + FrustumMatrix[5][1] * FrustumMatrix[5][1] +
                   FrustumMatrix[5][2] * FrustumMatrix[5][2]));

    FrustumMatrix[5][0] /= t;
    FrustumMatrix[5][1] /= t;
    FrustumMatrix[5][2] /= t;
    FrustumMatrix[5][3] /= t;
}

bool Frustum::SphereInFrustum(vec3 Center, float Radius) {
    for (int i = 0; i < 6; i++) {
        if (FrustumMatrix[i][0] * Center.x + FrustumMatrix[i][1] * Center.y + FrustumMatrix[i][2] * Center.z +
            FrustumMatrix[i][3] <= -Radius)
            return false;
    }
    return true;
} 
