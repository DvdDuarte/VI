//
//  camera.hpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 10/02/2023.
//

#pragma once
#include "ray.hpp"

// based on pbrt book, sec 6.1, pag. 356
class Camera {
public:
    virtual ~Camera() = default;
    virtual bool GenerateRay(int x, int y, Ray *r, const float *cam_jitter = nullptr) = 0;
};
