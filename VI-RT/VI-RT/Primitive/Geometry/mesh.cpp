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

    const float kEpsilon = 1e-8;

    Point e0 = vertices[f.vert_ndx[0]];
    Point e1 = vertices[f.vert_ndx[1]];
    Point e2 = vertices[f.vert_ndx[2]];

    // First, we need to calculate the edge vectors for the triangle.
    Vector edge1 = Vector(e1.X - e0.X, e1.Y - e0.Y, e1.Z - e0.Z);
    Vector edge2 = Vector(e2.X - e0.X, e2.Y - e0.Y, e2.Z - e0.Z);

    // Then, we calculate the cross product of the ray direction and edge2.
    Vector pvec = r.dir.cross(edge2);

    // If the determinant is close to zero, the ray and triangle are parallel.
    float det = edge1.dot(pvec);

    if (fabs(det) < kEpsilon) {
        return false;
    }

    // Calculate the inverse determinant.
    float inv_det = 1.0f / det;

    // Calculate the distance from the ray origin to the first vertex of the triangle.

    Vector tvec = Vector(r.o.X - e0.X, r.o.Y - e0.Y, r.o.Y - e0.Y);

    // Calculate the u parameter.
    float u = tvec.dot( pvec) * inv_det;

    // If u is outside the range [0,1], the intersection is outside the triangle.
    if (u < 0.0f || u > 1.0f) {
        return false;
    }

    // Calculate the cross product of tvec and edge1.
    Vector qvec = tvec.cross( edge1);

    // Calculate the v parameter.
    float v = (r.dir).dot( qvec) * inv_det;

    // If v is outside the range [0,1], the intersection is outside the triangle.
    if (v < 0.0f || u + v > 1.0f) {
        return false;
    }

    // Calculate the distance from the ray origin to the triangle intersection point.
    float t = edge2.dot( qvec) * inv_det;

    if (t < kEpsilon) {
        return false;
    }
    // If we've made it this far, we have an intersection!
    Point hit_point = Point(r.o.X + t * r.dir.X, r.o.Y + t * r.dir.Y, r.o.Z + t * r.dir.Z);
    Vector geo_normal = edge1.cross(edge2);
    geo_normal.normalize();

    Vector shading_normal = geo_normal;
    if (f.hasShadingNormals) {
        shading_normal = normals[f.vert_normals_ndx[0]] * (1.0f - u - v) +
                         normals[f.vert_normals_ndx[1]] * u +
                         normals[f.vert_normals_ndx[2]] * v;
    }


    Vector r_aux = Vector(-r.dir.X, -r.dir.Y,-r.dir.Z);
    r_aux.normalize();
    *isect = Intersection(hit_point, geo_normal, r_aux, t);

    shading_normal.normalize();
    isect->sn = shading_normal;
    return true;


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
    
    return intersect;
}
