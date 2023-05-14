//
//  main.cpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 30/01/2023.
//

#include <iostream>
#include "scene.hpp"
#include "perspective.hpp"
#include "renderer.hpp"
#include "ImagePPM.hpp"
#include "AmbientLight.hpp"
#include "StandardRenderer.hpp"
#include "PointLight.hpp"
#include "Shader/WhittedShader.hpp"
#include "AreaLight.hpp"
#include "Shader/DistributedShader.hpp"

int main(int argc, const char * argv[]) {
    Scene scene;
    Perspective *cam; // Camera
    ImagePPM *img;    // Image
    Shader *shd;
    bool success;

    success = scene.Load("../VI-RT/Scene/tinyobjloader/models/cornell_box_VI.obj");

    if (!success) {
        std::cout << "ERROR!! :o\n";
        return 1;
    }
    std::cout << "Scene Load: SUCCESS!! :-)\n";
    scene.printSummary();
    std::cout << std::endl;

    /*
    // add an ambient light to the scene
    AmbientLight ambient(RGB(0.05,0.05,0.05));
    scene.lights.push_back(&ambient);
    scene.numLights++;
    std::cout << "Ambient Light: SUCCESS!! :-)\n";
    */

    /*
    // add a point light to the scene
    auto *pl1 = new PointLight(RGB(0.65,0.65,0.65),
                                     Point(288,508,282));
    scene.lights.push_back(pl1);
    scene.numLights++;
    std::cout << "Point Light: SUCCESS!! :-)\n";
    */

// add first area light to the scene
    Point v1 = {0, 548, 0};
    Point v2 = {111.2, 548, 0};
    Point v3 = {111.2, 548, 559.2};
    Vector n = {0, -1, 0};
    RGB power = {0.9, 0.9, 0.9};

    auto* al1 = new AreaLight(power, v1, v2, v3, n);
    scene.lights.push_back(al1);
    scene.numLights++;

// add second area light to the scene
    v1 = {111.2, 548, 0};
    v2 = {222.4, 548, 0};
    v3 = {222.4, 548, 559.2};

    auto* al2 = new AreaLight(power, v1, v2, v3, n);
    scene.lights.push_back(al2);
    scene.numLights++;

// add third area light to the scene
    v1 = {222.4, 548, 0};
    v2 = {333.6, 548, 0};
    v3 = {333.6, 548, 559.2};

    auto* al3 = new AreaLight(power, v1, v2, v3, n);
    scene.lights.push_back(al3);
    scene.numLights++;

// add fourth area light to the scene
    v1 = {333.6, 548, 0};
    v2 = {444.8, 548, 0};
    v3 = {444.8, 548, 559.2};

    auto* al4 = new AreaLight(power, v1, v2, v3, n);
    scene.lights.push_back(al4);
    scene.numLights++;

// add fifth area light to the scene
    v1 = {444.8, 548, 0};
    v2 = {556, 548, 0};
    v3 = {556, 548, 559.2};

    auto* al5 = new AreaLight(power, v1, v2, v3, n);
    scene.lights.push_back(al5);
    scene.numLights++;

    std::cout << "Area Lights: SUCCESS!! :-)\n";

    // Image resolution
    const int W= 1024;
    const int H= 1024;

    img = new ImagePPM(W,H);

    // Camera parameters
    const Point Eye ={280,275,-165}, At={280,265,0};
    const Vector Up={0,-1,0};
    const float fovW = 90, fovH = fovW * H/W;
    cam = new Perspective(Eye, At, Up, W, H, fovW, fovH);

    // create the shader
    RGB background(0.05, 0.05, 0.55);
    std::cout << "everything done -> going inside the shader: SUCCESS!! :-)\n";
    shd = new DistributedShader(&scene, background);
    // declare the renderer
    StandardRenderer myRender (cam, &scene, img, shd);
    // render
    myRender.Render();

    // save the image
    img->Save("MyImage.ppm");

    std::cout << "That's all, folks!" << std::endl;
    return 0;
}
