//
// Created by jafmalheiro on 23/03/2023.
//

#ifndef VI_RT_SHADER_HPP
#define VI_RT_SHADER_HPP

#include "scene.hpp"
#include "RGB.hpp"

class Shader {
protected:
    Scene *scene;
public:
    explicit Shader (Scene *_scene): scene(_scene) {}
    ~Shader () = default;
    virtual RGB shade (bool intersected, Intersection isect) {return {};}
};

#endif //VI_RT_SHADER_HPP
