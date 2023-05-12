//
//  perspective.cpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 10/02/2023.
//
#include "perspective.hpp"

Perspective::Perspective(Point Eye, const Point At, Vector Up, const int W, const int H, const float fovW, const float fovH) : Eye(Eye), At(At), Up(Up), W(W), H(H), fovW(fovW), fovH(fovH) {
    Vector dir = Eye.vec2point(At);
    dir.normalize();
    right = Up.cross(dir);
    right.normalize();  // Normalize 'right'

    c2w[0][0] = right.X;
    c2w[0][1] = right.Y;
    c2w[0][2] = right.Z;

    c2w[1][0] = Up.X;
    c2w[1][1] = Up.Y;
    c2w[1][2] = Up.Z;

    c2w[2][0] = dir.X;
    c2w[2][1] = dir.Y;
    c2w[2][2] = dir.Z;
}

bool Perspective::GenerateRay(int x, int y, Ray *r, const float *cam_jitter) {
    float xs = (2.0f * (x + 0.5f) / W) - 1.0f;

    float ys = (2.0f * (y + 0.5f) / H) - 1.0f;

    float xc = xs * tan(fovW / 2);
    float yc = ys * tan(fovH / 2);

    Vector direction = Vector(xc, yc, 1);

    Vector worldDirection = Vector(
            c2w[0][0] * direction.X + c2w[1][0] * direction.Y + c2w[2][0] * direction.Z,
            c2w[0][1] * direction.X + c2w[1][1] * direction.Y + c2w[2][1] * direction.Z,
            c2w[0][2] * direction.X + c2w[1][2] * direction.Y + c2w[2][2] * direction.Z
    );

    *r = Ray(Eye, worldDirection);

    return true;
}
