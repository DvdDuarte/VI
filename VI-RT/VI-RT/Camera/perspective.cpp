//
//  perspective.cpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 10/02/2023.
//
#include "perspective.hpp"

Perspective::Perspective(const Point Eye, const Point At, const Vector Up, const int W, const int H, const float fovW, const float fovH) : Eye(Eye), At(At), Up(Up), W(W), H(H), fovW(fovW), fovH(fovH) {
    Vector dir = Eye.vec2point(At);
    dir.Normalize();
    right = Vector::Cross(dir, Up);
    right.Normalize();  // Normalize 'right'
    up = Vector::Cross(right, dir);
    forward = Vector::Cross(up, right);
    dist = 1.0f / std::tan(fovW * 0.5f);

    c2w[0][0] = right.X;
    c2w[1][0] = right.Y;
    c2w[2][0] = right.Z;

    c2w[0][1] = up.X;
    c2w[1][1] = up.Y;
    c2w[2][1] = up.Z;

    c2w[0][2] = forward.X;
    c2w[1][2] = forward.Y;
    c2w[2][2] = forward.Z;
}

bool Perspective::GenerateRay(int x, int y, Ray *r, const float *cam_jitter) {
    float xs = (2.0f * (x + 0.5f) / W) - 1.0f;

    float ys = (2.0f * (y + 0.5f) / H) - 1.0f;

    float xc = xs * tan(fovW / 2);
    float yc = ys * tan(fovH / 2);

    Vector direction = Vector(xc, yc, 1);

    Vector worldDirection = Vector(
            c2w[0][0] * direction.X + c2w[0][1] * direction.Y + c2w[0][2] * direction.Z,
            c2w[1][0] * direction.X + c2w[1][1] * direction.Y + c2w[1][2] * direction.Z,
            c2w[2][0] * direction.X + c2w[2][1] * direction.Y + c2w[2][2] * direction.Z
    );

    *r = Ray(Eye, worldDirection);

    return true;
}
