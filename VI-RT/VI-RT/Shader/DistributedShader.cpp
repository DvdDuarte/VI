//
// Created by jafmalheiro on 12/05/2023.
//

#include "DistributedShader.hpp"

/**
RGB DistributedShader::directLighting (Intersection isect, Phong *f) {
    RGB color(0.,0.,0.);
    for (auto l = scene->lights.begin() ; l != scene->lights.end() ; l++) {
        if (l->type == AMBIENT_LIGHT) { // is it an ambient light ?
            //...
        }
        if (l->type == POINT_LIGHT) { // is it a point light ?
            //...
        }
        if (l->type == AREA_LIGHT) { // is it an area light ?
            //...
        } // end area light
    }
    return color;
}
**/

RGB DistributedShader::shade(bool intersected, Intersection isect) {
    RGB color(0.,0.,0.);
    if (!intersected) return (background);

    // intersection with a light source
    if (isect.isLight) return isect.Le;
    Phong *f = (Phong *)isect.f;

    // if there is a specular component sample it
    if (!f->Ks.isZero()) color += specularReflection (isect, f);

    // if there is a diffuse component do direct light
    if (!f->Kd.isZero()) color += directLighting(isect, f);

    return color;

};
