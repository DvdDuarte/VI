//
//  mesh.cpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 05/02/2023.
//

#include "mesh.hpp"
#include <float.h>

// see pbrt book (3rd ed.), sec 3.6.2, pag 157
bool Mesh::TriangleIntersect (Ray r, Face f, Intersection *isect) {

    if(!f.bb.intersect(r))
        return false;

    const float EPSILON = 0.0000001;
    Point p0 = vertices[f.vert_ndx[0]];
    Point p1 = vertices[f.vert_ndx[1]];
    Point p2 = vertices[f.vert_ndx[2]];

    float a,factor,u,v,t;
    Vector edge1, edge2, h, s, q, wo;

    edge1 = Vector(p1.X - p0.X, p1.Y - p0.Y, p1.Z - p0.Z);
    edge2 = Vector(p2.X - p0.X, p2.Y - p0.Y, p2.Z - p0.Z);

    h = r.dir.cross(edge2);
    a = edge1.dot(h);

    if (a > -EPSILON && a < EPSILON)
        return false;

    factor = 1.0 / a;
    s = Vector(r.o.X - p0.X, r.o.Y - p0.Y, r.o.Z - p0.Z);
    u = factor * s.dot(h);

    if(u < 0.0 || u > 1.0)
        return false;

    t = factor * edge2.dot(q);
    if (t > EPSILON) { // ray intersection
        // Compute the intersection point and update the Intersection object
        Point intersectionPoint = r.o + Point(r.dir.X * t, r.dir.Y * t, r.dir.Z * t);
        Vector normal;
        if (f.hasShadingNormals) {
            // Interpolate the shading normals at the intersection point
            normal = normals[f.vert_normals_ndx[0]] * (1.0 - u - v) +
                     normals[f.vert_normals_ndx[1]] * u +
                     normals[f.vert_normals_ndx[2]] * v;
            normal.normalize();
        } else {
            // Use the geometric normal of the face
            normal = f.geoNormal;
        }

        wo = Vector(r.dir.X, r.dir.Y, r.dir.Z);
        *isect = Intersection(intersectionPoint, normal, wo, t);
        return true;
    }
    return false;
}

bool Mesh::intersect (Ray r, Intersection *isect) {
    bool intersect = true, intersect_this_face;
    Intersection min_isect, curr_isect;
    float min_depth=FLT_MAX;
    // intersect the ray with the mesh BB
    

    if (!intersect) return false;
    
    // If it intersects then loop through the faces
    intersect = false;
    for (auto face_it=faces.begin() ; face_it != faces.end() ; face_it++) {
        intersect_this_face = TriangleIntersect(r, *face_it, &curr_isect);
        if (!intersect_this_face) continue;
        
        intersect = true;
        if (curr_isect.depth < min_depth) {  // this is closer
            min_depth = curr_isect.depth;
            min_isect = curr_isect;
        }
    }

    isect = &min_isect;
    
    return intersect;
}
