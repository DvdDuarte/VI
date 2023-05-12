//
//  AmbientShader.cpp
//  VI-RT-LPS
//
//  Created by Luis Paulo Santos on 14/03/2023.
//

#include <random>
#include <chrono>
#include "WhittedShader.hpp"
#include "Phong.hpp"
#include "ray.hpp"
#include "PointLight.hpp"
#include "AreaLight.h"

RGB WhittedShader::directLighting (Intersection isect, Phong *f) {
    RGB color(0.,0.,0.);

    // Loop over scene's light sources
    for (auto l = scene->lights.begin() ; l != scene->lights.end() ; l++) {

        if ((*l)->type == AMBIENT_LIGHT) {  // is it an ambient light ?
            if (!f->Ka.isZero()) {
                RGB Ka = f->Ka;
                color += Ka * (*l)->L();
            }
            continue;
        }
        if ((*l)->type == AREA_LIGHT) {
            // Perform Monte Carlo sampling of the area light
            auto* areaLight = dynamic_cast<AreaLight*>(*l);

            /*
             * popular numbers are 16, 64 and 256
             * However, the optimal number of samples can depend on various factors
             * such as the complexity of the light source and the shading model.
            */
            int num_light_samples = 64;

            RGB accumulated_light(0.,0.,0.);

            for (int sample = 0; sample < num_light_samples; ++sample) {
                unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
                std::default_random_engine generator(seed);
                std::uniform_real_distribution<float> distribution(0.0, 1.0);
                float r1 = distribution(generator);  // Generates a random float between 0 and 1
                float r2 = distribution(generator);  // Generate a random float between 0 and 1

                Point sample_position;
                float light_pdf;
                // RGB light_intensity = areaLight->Sample_L(r1, r2, &sample_position, &light_pdf);

                Vector direction_to_light = sample_position.vec2point(isect.p);
                float distance_squared = direction_to_light.dot(direction_to_light);
                direction_to_light.normalize();

                Ray shadow_ray(isect.p, direction_to_light);
            }

            RGB final_light = accumulated_light / float(num_light_samples);
            color = color + final_light;
        }
        if ((*l)->type == POINT_LIGHT) {  // is it a point light ?
            if (!f->Kd.isZero()) {
                Point lpoint;
                // get the position and radiance of the light source
                RGB L = (*l)->Sample_L(NULL, &lpoint);

                // compute the direction from the intersection to the light
                Vector Ldir = isect.p.vec2point(lpoint);

                // Compute the distance between the intersection and the light source
                const float Ldistance = Ldir.norm();

                Ldir.normalize(); // now normalize Ldir

                // compute the cosine (Ldir , shading normal)
                float cosL = Ldir.dot(isect.sn);

                if (cosL>0.) { // the light is NOT behind the primitive

                    // generate the shadow ray
                    Ray shadow(isect.p, Ldir);

                    // adjust origin EPSILON along the normal: avoid self occlusion
                    shadow.adjustOrigin(isect.gn);

                    if (scene->visibility(shadow, Ldistance-EPSILON)) // light source not occluded
                        color += f->Kd * L * cosL;
                } // end cosL > 0.

                // the light is behind the primitive

            }
        }
    }
    return color;
}

RGB WhittedShader::specularReflection (Intersection isect, Phong *f, int depth) {

    // generate the specular ray
    float cos = isect.gn.dot(isect.wo);
    Vector Rdir = 2.f * cos * isect.gn - isect.wo;
    Ray specular(isect.p, Rdir);
    specular.adjustOrigin(isect.gn);
    Intersection s_isect;

    // trace ray
    bool intersected = scene->trace(specular, &s_isect);

    // shade this intersection
    RGB color = shade(intersected, s_isect, depth);

    return color;
}

RGB WhittedShader::shade(bool intersected, Intersection isect, int depth) {
    RGB color(0.,0.,0.);

    // if no intersection, return background
    if (!intersected) {
        return (background);
    }

    // get the BRDF
    Phong *f = (Phong *)isect.f;

    // if there is a specular component sample it
    if (!f->Ks.isZero() && depth<4) {
        color += specularReflection (isect, f, depth+1);
    }

    color += directLighting(isect, f);

    return color;
};