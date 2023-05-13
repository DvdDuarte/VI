//
// Created by jafmalheiro on 23/03/2023.
//

#ifndef VI_RT_STANDARDRENDERER_HPP
#define VI_RT_STANDARDRENDERER_HPP

#include "renderer.hpp"

class StandardRenderer: public Renderer {
public:
    StandardRenderer (Camera *cam, Scene * scene, Image * img, Shader *shd): Renderer(cam, scene, img, shd) {}
    void Render ();

    float getRandom(int min, int max) {
        srand(time(nullptr)); // Seed the random number generator with the current time

        float random = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);

        return min + random * (max - min);
    };
};

#endif //VI_RT_STANDARDRENDERER_HPP
