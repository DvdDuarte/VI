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

        Point tLower = (min - r.o); //* 1/r.dir;
        Point tUpper = (max - r.o); // * 1/r.dir

        // Easy to remember: ``max of mins, and min of maxes''

        float tBoxMin, tBoxMax;

        return tBoxMin <= tBoxMax;
    }
} BB;

#endif /* AABB_hpp */
