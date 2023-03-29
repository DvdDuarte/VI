//
//  AABB.hpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 30/01/2023.
//

#ifndef BB_hpp
#define BB_hpp

#include "vector.hpp"
#include "ray.hpp"

typedef struct BB {
    Point min, max;
    void update (Point p) {
        if (p.X < min.X) min.X = p.X;
        else if (p.X > max.X) max.X = p.X;
        if (p.Y < min.Y) min.Y = p.Y;
        else if (p.Y > max.Y) max.Y = p.Y;
        if (p.Z < min.Z) min.Z = p.Z;
        else if (p.Z > max.Z) max.Z = p.Z;
    }
    bool intersect (Ray r) {

        // Absolute distances to lower and upper box coordinates

        Point invRayDir = Point(1/r.dir.X, 1/r.dir.Y, 1/r.dir.Z);

        Point tLowerAux = Point(min.X - r.o.X, min.Y - r.o.Y, min.Z - r.o.Z);
        Point tUpperAux = Point(max.X - r.o.X, max.Y - r.o.Y, max.Z - r.o.Z);

        Vector tLower = Vector(tLowerAux.X * invRayDir.X,
                               tLowerAux.Y * invRayDir.Y,
                               tLowerAux.Y * invRayDir.Y);
        Vector tUpper = Vector(tUpperAux.X * invRayDir.X,
                               tUpperAux.Y * invRayDir.Y,
                               tUpperAux.Y * invRayDir.Y);

        // Easy to remember: ``max of mins, and min of maxes''

        float tBoxMin = std::fmax(std::fmax(tLower.X, tLower.Y), tLower.Z);
        float tBoxMax = std::fmin(std::fmin(tUpper.X, tUpper.Y), tUpper.Z);

        return tBoxMin <= tBoxMax;
    }
} BB;

#endif /* AABB_hpp */
