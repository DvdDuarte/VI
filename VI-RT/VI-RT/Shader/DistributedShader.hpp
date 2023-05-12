//
// Created by jafmalheiro on 12/05/2023.
//

#ifndef VI_RT_DISTRIBUTEDSHADER_HPP
#define VI_RT_DISTRIBUTEDSHADER_HPP


#include "Shader.hpp"
#include "Phong.hpp"

class DistributedShader: public Shader {
    RGB background;
    RGB directLighting(Intersection isect, Phong *f);
    RGB specularReflection(Intersection isect, Phong *f);
public:
    DistributedShader(Scene *scene, RGB bg) : background(bg), Shader(scene) {}
    RGB shade(bool intersected, Intersection isect);
};

#endif //VI_RT_DISTRIBUTEDSHADER_HPP
