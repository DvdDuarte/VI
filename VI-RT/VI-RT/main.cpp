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
#include "Shader/PathTracerShader.hpp"
#include "Image/Converters/ImageConverter.hpp"

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
    //AmbientLight ambient(RGB(0.05,0.05,0.05));
    //scene.lights.push_back(&ambient);
    //scene.numLights++;
    //std::cout << "Ambient Light: SUCCESS!! :-)\n";

    // add a point light to the scene
    //auto* pl = new PointLight();
    //pl->addSource(RGB(0.65,0.65,0.65), Point(288,508,282));
    //pl->addSource(RGB(0.75,0.75,0.75), Point(200,400,300));
    //scene.lights.push_back(pl);
    //scene.numLights++;
    //std::cout << "Point Light: SUCCESS!! :-)\n";

    // add first area light to the scene
    Point v1 = {343, 548, 227};
    Point v2 = {343, 548, 332};
    Point v3 = {213, 548, 332};
    Vector n = {0, -1, 0};
    RGB power = {0.1, 0.1, 0.1};

    auto* al1 = new AreaLight(power);
    al1->addSource(v1, v2, v3, n);
    scene.lights.push_back(al1);
    scene.numLights++;

    // add second area light to the scene
    v1 = {343, 548, 332};
    v2 = {213, 548, 332};
    v3 = {213, 548, 227};
    n = {0, -1, 0};
    power = {0.1, 0.1, 0.1};
    auto* al2 = new AreaLight(power);
    al2->addSource(v1, v2, v3, n);
    scene.lights.push_back(al2);
    scene.numLights++;

    // add third area light to the scene
    v1 = {130, 548, 296};
    v2 = {240, 548, 272};
    v3 = {240, 548, 378};
    n = {0, -1, 0};
    power = {0.1, 0.1, 0.1};
    auto* al3 = new AreaLight(power);
    al3->addSource(v1, v2, v3, n);
    scene.lights.push_back(al3);
    scene.numLights++;

    // add forth area light to the scene
    v1 = {240, 548, 378};
    v2 = {130, 548, 402};
    v3 = {130, 548, 296};
    n = {0, -1, 0};
    power = {0.1, 0.1, 0.1};
    auto* al4 = new AreaLight(power);
    al4->addSource(v1, v2, v3, n);
    scene.lights.push_back(al4);
    scene.numLights++;

    // add fifth area light to the scene
    v1 = {423, 548, 329};
    v2 = {314, 548, 355};
    v3 = {314, 548, 459};
    n = {0, -1, 0};
    power = {0.1, 0.1, 0.1};
    auto* al5 = new AreaLight(power);
    al5->addSource(v1, v2, v3, n);
    scene.lights.push_back(al5);
    scene.numLights++;

    // add sixth area light to the scene
    v1 = {314, 548, 459};
    v2 = {423, 548, 433};
    v3 = {423, 548, 329};
    n = {0, -1, 0};
    power = {0.1, 0.1, 0.1};
    auto* al6 = new AreaLight(power);
    al6->addSource(v1, v2, v3, n);
    scene.lights.push_back(al6);
    scene.numLights++;

    std::cout << "Area Lights: SUCCESS!! :-)\n";

    // Image resolution
    const int W = 1024;
    const int H = 1024;

    img = new ImagePPM(W,H);

    // Camera parameters
    const Point Eye = {280,275,-165};
    const Point At = {280,265,0};
    const Vector Up = {0,-1,0};
    const float fovW = 90, fovH = fovW * H/W;
    cam = new Perspective(Eye, At, Up, W, H, fovW, fovH);

    // create the shader
    RGB background(0.05, 0.05, 0.55);
    std::cout << "everything done -> going inside the shader: SUCCESS!! :-)\n";
    shd = new PathTracerShader(&scene, background);
    // declare the renderer
    StandardRenderer myRender(cam, &scene, img, shd);
    // render
    myRender.Render();

    // save the image
    img->Save("MyImage.ppm");

    // PPM -> JPG
    converter("MyImage.ppm", "jpg");

    // PPM -> PFM
    converter("MyImage.ppm", "pfm");

    // PPM -> OpenEXR
    converter("MyImage.ppm", "exr");

    std::cout << "That's all, folks!" << std::endl;
    return 0;
}
