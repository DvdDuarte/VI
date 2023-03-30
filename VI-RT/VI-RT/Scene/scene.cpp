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

    // Load materials
    for (auto mat = materials.begin(); mat != materials.end(); mat++) {

        auto *material = new Phong();
        material->Ka = RGB(mat->ambient[0], mat->ambient[1], mat->ambient[2]);
        material->Kd = RGB(mat->diffuse[0], mat->diffuse[1], mat->diffuse[2]);
        material->Ks = RGB(mat->specular[0], mat->specular[1], mat->specular[2]);
        material->Kt = RGB(mat->transmittance[0], mat->transmittance[1], mat->transmittance[2]);

        this->numBRDFs++;
        this->BRDFs.push_back(material);
    }

    // Load shapes
    for (auto shp = shapes.begin(); shp != shapes.end() ; shp++) {

        // for each shape create a primitive (Mesh + material_ndx)

        auto *prim = new Primitive();
        Mesh *mesh = new Mesh();
        prim->g = mesh;

        // iterate shapes
        for (auto ver = shp->mesh.indices.begin(); ver != shp->mesh.indices.end();) {

            // Create a new face for the mesh
            Face *face = new Face();

            face->bb.min = Point(0,0,0);
            face->bb.max = Point(0,0,0);

            // Loop over vertices in the face.
            for (size_t v = 0; v < 3; v++) {

                // access to vertex
                tinyobj::real_t vx = attrib.vertices[3*ver->vertex_index];
                tinyobj::real_t vy = attrib.vertices[3*ver->vertex_index+1];
                tinyobj::real_t vz = attrib.vertices[3*ver->vertex_index+2];

                Point ver_now = Point(vx,vy,vz);
                int index = mesh->get_index(ver_now);
                if(index != -1) face->vert_ndx[v] = index;
                else {
                    mesh->vertices.push_back(ver_now);
                    mesh->numVertices++;
                    face->vert_ndx[v] = mesh->numVertices-1;
                }
                face->bb.update(ver_now);

                ver++;
            }

            mesh->faces.push_back(*face);
            mesh->numFaces++;
        }

        prim->material_ndx = shp->mesh.material_ids[0];

        prims.push_back(prim);
        numPrimitives++;
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
                curr_isect.f = BRDFs[(*prim_itr)->material_ndx];
                *isect = curr_isect;
            }
            else if (curr_isect.depth < isect->depth) {
                curr_isect.f = BRDFs[(*prim_itr)->material_ndx];
                *isect = curr_isect;

            }
        }
    }
    return intersection;
}

