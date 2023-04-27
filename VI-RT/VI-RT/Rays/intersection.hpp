//
//  Intersection.hpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 30/01/2023.
//

#pragma once

#include "vector.hpp"
#include "BRDF.hpp"

typedef struct Intersection {
public:
    Point p;
    Vector gn;  // geometric normal
    Vector sn;  // shading normal (the same as gn for the time being)
    Vector wo;
    float depth{};
    BRDF *f{};
    int x{}, y{};

    Intersection() = default;
    // from pbrt book, section 2.10, pag 116
    Intersection(const Point &p, const Vector &n, const Vector &wo, float depth) : p(p), gn(n), sn(n), wo(wo), depth(depth), f(nullptr) { }
} Intersection;
