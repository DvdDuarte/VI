//
//  Ray.hpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 30/01/2023.
//

#pragma once

#include "vector.hpp"

class Ray {
public:
    Point o; // ray origin
    Vector dir; // ray direction
    int x{}, y{};
    Ray () {}
    Ray(Point o, Vector d) : o(o), dir(d), x(0), y(0) {}
    ~Ray() = default;
};

