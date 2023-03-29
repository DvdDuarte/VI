//
//  Scene.cpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 30/01/2023.
//

#include "scene.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "primitive.hpp"
#include "mesh.hpp"
#include "Phong.hpp"

#include <iostream>
#include <set>
#include <vector>

using namespace tinyobj;

static void PrintInfo (const ObjReader myObj) {
    const tinyobj::attrib_t attrib = myObj.GetAttrib();
    const std::vector<tinyobj::shape_t> shapes = myObj.GetShapes();
    const std::vector<tinyobj::material_t> materials = myObj.GetMaterials();
    std::cout << "# of vertices  : " << (attrib.vertices.size() / 3) << std::endl;
    std::cout << "# of normals   : " << (attrib.normals.size() / 3) << std::endl;
    std::cout << "# of texcoords : " << (attrib.texcoords.size() / 2)
              << std::endl;

    std::cout << "# of shapes    : " << shapes.size() << std::endl;
    std::cout << "# of materials : " << materials.size() << std::endl;
    
    // Iterate shapes
    auto it_shape = shapes.begin();
    for ( ; it_shape != shapes.end() ; it_shape++) {
        // assume each face has 3 vertices
        std::cout << "Processing shape " << it_shape->name << std::endl;
        // iterate faces
        // assume each face has 3 vertices
        auto it_vertex = it_shape->mesh.indices.begin();
        for ( ; it_vertex != it_shape->mesh.indices.end() ; ) {
            // process 3 vertices
            for (int v=0 ; v<3 ; v++) {
                std::cout << it_vertex->vertex_index;
                it_vertex++;
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
        
        printf("There are %lu material indexes\n", it_shape->mesh.material_ids.size());
    }
    
}

/*
 Use tiny load to load .obj scene descriptions
 https://github.com/tinyobjloader/tinyobjloader
 */

bool Scene::Load (const std::string &fname) {
    ObjReader myObjReader;

    if (!myObjReader.ParseFromFile(fname)) {
        return false;
    }

    PrintInfo (myObjReader);

    const tinyobj::attrib_t attrib = myObjReader.GetAttrib();
    std::vector<tinyobj::shape_t> shapes = myObjReader.GetShapes();
    std::vector<tinyobj::material_t> materials = myObjReader.GetMaterials();

    numBRDFs = materials.size();
    BRDFs.resize(numBRDFs);
    // Load materials
    for (int i = 0; i < numBRDFs; i++) {

        Phong ph;
        ph.Ka = RGB(materials[i].ambient[0], materials[i].ambient[1], materials[i].ambient[2]);
        ph.Kd = RGB(materials[i].diffuse[0], materials[i].diffuse[1], materials[i].diffuse[2]);
        ph.Ks = RGB(materials[i].specular[0], materials[i].specular[1], materials[i].specular[2]);
        ph.Kt = RGB(materials[i].transmittance[0], materials[i].transmittance[1], materials[i].transmittance[2]);
        BRDFs[i] = new Phong(ph);
    }

    numPrimitives = shapes.size();
    // Load shapes
    auto it_shape = shapes.begin();
    for (int i = 0 ; it_shape != shapes.end() ; it_shape++, i++) {

        // for each shape create a primitive (Mesh + material_ndx)

        int num_normals = 0;
        Mesh *mesh = new Mesh();
        mesh->numFaces = it_shape->mesh.num_face_vertices.size();
        mesh->numVertices = it_shape->mesh.indices.size();

        // iterate faces
        size_t index_offset = 0;
        for (size_t f = 0; f < it_shape->mesh.num_face_vertices.size(); f++) {
            size_t fv = size_t(it_shape->mesh.num_face_vertices[f]);

            // Create a new face for the mesh
            Face *face = new Face();

            BB *b_box = new BB();
            b_box->min = Point(0,0,0);
            b_box->max = Point(0,0,0);

            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; v++) {

                tinyobj::index_t idx = it_shape->mesh.indices[index_offset + v];

                // Fill the face vertex indices vector
                face->vert_ndx[v] = idx.vertex_index;

                // access to vertex
                tinyobj::real_t vx = attrib.vertices[3*size_t(idx.vertex_index)+0];
                tinyobj::real_t vy = attrib.vertices[3*size_t(idx.vertex_index)+1];
                tinyobj::real_t vz = attrib.vertices[3*size_t(idx.vertex_index)+2];

                b_box->update(Point(vx,vy,vz));

                // Fill the mesh's vertices vector
                mesh->vertices.push_back(Point(vx,vy,vz));

                // Check if `normal_index` is zero or positive. negative = no normal data
                if (idx.normal_index >= 0) {

                    // Fill the face normal indices vector
                    face->vert_normals_ndx[v] = idx.normal_index;

                    // access to normal
                    tinyobj::real_t nx = attrib.normals[3*size_t(idx.normal_index)+0];
                    tinyobj::real_t ny = attrib.normals[3*size_t(idx.normal_index)+1];
                    tinyobj::real_t nz = attrib.normals[3*size_t(idx.normal_index)+2];

                    mesh->normals.push_back(Vector(nx, ny, nz));
                    mesh->numNormals++;

                } else mesh->numNormals = 0;
            }

            // Create geoNormal (interpolate triangle vertices)
            Point e0 = mesh->vertices[index_offset];
            Point e1 = mesh->vertices[index_offset+1];
            Point e2 = mesh->vertices[index_offset+2];

            Vector e0e1 = Vector(e1.X - e0.X, e1.Y - e0.Y, e1.Z - e0.Z);
            Vector e0e2 = Vector(e2.X - e0.X, e2.Y - e0.Y, e2.Z - e0.Z);

            Vector geoNorm = e0e1.cross(e0e2);
            geoNorm.normalize();

            face->geoNormal = geoNorm;

            // Does it have shading normals ?
            face->hasShadingNormals = false;

            face->bb = *b_box;

            mesh->faces.push_back(*face);

            index_offset += fv;
        }

        Primitive *prim = new Primitive();
        prim->g = mesh;
        prim->material_ndx = it_shape->mesh.material_ids[i];

        prims.push_back(prim);
    }

    return true;
}

bool Scene::trace (Ray r, Intersection *isect) {
    Intersection curr_isect;
    bool intersection = false;    
    
    if (numPrimitives==0) return false;
    
    // iterate over all primitives
    for (auto prim_itr = prims.begin() ; prim_itr != prims.end() ; prim_itr++) {
        if ((*prim_itr)->g->intersect(r, &curr_isect)) {
            if (!intersection) { // first intersection
                intersection = true;
                *isect = curr_isect;
                isect->f = BRDFs[(*prim_itr)->material_ndx];
            }
            else if (curr_isect.depth < isect->depth) {
                *isect = curr_isect;
            }
        }
    }
    return intersection;
}

