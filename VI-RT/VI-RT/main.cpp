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
#include "Shader/AmbientShader.hpp"
#include "StandardRenderer.hpp"
#include "PointLight.hpp"
#include "Shader/WhittedShader.hpp"
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

    // add an ambient light to the scene
    AmbientLight ambient(RGB(0.05,0.05,0.05));
    scene.lights.push_back(&ambient);
    scene.numLights++;
    // add a point light to the scene
    PointLight *pl1 = new PointLight(RGB(0.65,0.65,0.65),
                                     Point(288,508,282));
    scene.lights.push_back(pl1);
    scene.numLights++;

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
