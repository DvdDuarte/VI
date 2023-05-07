//
//  Scene.hpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 30/01/2023.
//

#ifndef Scene_hpp
#define Scene_hpp

#include <iostream>
#include <string>
#include <vector>
#include "primitive.hpp"
#include "light.hpp"
#include "ray.hpp"
#include "intersection.hpp"
#include "BRDF.hpp"
#include <memory>

class Scene {
public:
    std::vector<std::shared_ptr<Primitive>> prims;
    std::vector<std::shared_ptr<BRDF>> BRDFs;
    std::vector<std::shared_ptr<Light>> lights;
    int numPrimitives, numLights, numBRDFs;
    Scene (): numPrimitives(0), numLights(0), numBRDFs(0) {}
    bool Load (const std::string &fname);
    bool SetLights (void) { return true; };
    bool trace (Ray r, Intersection *isect);
    void printSummary(void) {
        std::cout << "#primitives = " << numPrimitives << " ; ";
        std::cout << "#lights = " << numLights << " ; ";
        std::cout << "#materials = " << numBRDFs << " ;" << std::endl;
    }
    void addLight(const std::shared_ptr<Light>& light) {
        lights.push_back(light);
    }
};

#endif /* Scene_hpp */
