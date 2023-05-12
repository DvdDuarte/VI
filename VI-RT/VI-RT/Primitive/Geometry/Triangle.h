//
// Created by kiko on 12/05/2023.
//

#ifndef VI_RT_TRIANGLE_H
#define VI_RT_TRIANGLE_H


#include <cmath>

#include "vector.hpp"
#include "BB.hpp"
#include "intersection.hpp"
#include "geometry.hpp"

class Triangle: public Geometry {
public:
    Point v1, v2, v3;
    Vector normal,edge1, edge2;
    BB bb;
    
    bool intersect (Ray r, Intersection *isect);
    Triangle(Point _v1, Point _v2, Point _v3, Vector _normal):
            v1(_v1), v2(_v2), v3(_v3), normal(_normal) {
        edge1 = v1.vec2point(v2); edge2 = v1.vec2point(v3);
        bb.min.set(v1.X, v1.Y, v1.Z); bb.max.set(v1.X, v1.Y, v1.Z);
        bb.update(v2); bb.update(v3);
    }
    double area() const {
        Vector cross = edge1.cross(edge2);
        double length = std::sqrt(cross.X * cross.X + cross.Y * cross.Y + cross.Z * cross.Z);
        double area = 0.5 * length;
        return area;
    }
};


#endif //VI_RT_TRIANGLE_H
