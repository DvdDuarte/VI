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

    Point p0 = vertices[f.vert_ndx[0]];
    Point p1 = vertices[f.vert_ndx[1]];
    Point p2 = vertices[f.vert_ndx[2]];

    float a,factor,u,v,t;
    Vector edge1, edge2, h, s, q, wo;

    edge1 = Vector(p1.X - p0.X, p1.Y - p0.Y, p1.Z - p0.Z);
    edge2 = Vector(p2.X - p0.X, p2.Y - p0.Y, p2.Z - p0.Z);

    Vector n = edge1.cross(edge2);
    n.Normalize();

    // intersect ray with triangle plane
    float nd = n.dot(r.dir);
    if (nd == 0.f) return false; // ray is parallel to triangle
    t = n.dot(Vector(p0.X - r.o.X, p0.Y - r.o.Y, p0.Y - r.o.Y)) / nd;
    if (t <= 0.f) return false; // triangle is behind the ray

    // compute intersection point
    Point p = Point(r.o.X + r.dir.X * t, r.o.Y + r.dir.Y * t, r.o.Z + r.dir.Z * t);

    // inside/outside test
    Vector c;
    bool inside = true;
    for (int i = 0; i < 3; ++i) {
        Vector c1 = Vector(vertices[f.vert_ndx[(i + 1) % 3]].X - vertices[f.vert_ndx[i]].X,
                           vertices[f.vert_ndx[(i + 1) % 3]].Y - vertices[f.vert_ndx[i]].Y,
                           vertices[f.vert_ndx[(i + 1) % 3]].Z - vertices[f.vert_ndx[i]].Z);
        Vector c2 = Vector(p.X - vertices[f.vert_ndx[i]].X,
                           p.Y - vertices[f.vert_ndx[i]].Y,
                           p.Y - vertices[f.vert_ndx[i]].Y);
        c = c1.cross(c2);
        if (c.dot(n) < 0.f) {
            inside = false;
            break;
        }
    }
    if (!inside) return false;

    // set intersection info
    isect->p = p;
    isect->gn = n;
    isect->sn = n;

    wo = Vector(-r.dir.X, -r.dir.Y, -r.dir.Z);
    wo.Normalize();

    isect->wo = wo;
    isect->depth = t;
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
