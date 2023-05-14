//
// Created by jafmalheiro on 12/05/2023.
//

#include <chrono>
#include <random>
#include <omp.h>
#include "DistributedShader.hpp"
#include "AreaLight.hpp"

RGB DistributedShader::areaLight(Intersection isect, Phong* f, Light* l, RGB color, std::uniform_real_distribution<float> distribution, std::default_random_engine generator) {
    // Perform Monte Carlo sampling of the area light
    auto *areaLight = dynamic_cast<AreaLight *>(l);
    int num_light_samples = 4;
    RGB accumulated_light(0., 0., 0.);
    omp_set_num_threads(4);
#pragma omp parallel default(none) shared(distribution,generator,areaLight, isect, f, num_light_samples, accumulated_light) firstprivate(color)
    {
        RGB local_accumulated_light(0.,0.,0.);
#pragma omp for
        for (int sample = 0; sample < num_light_samples; ++sample) {
            float r[2] = {distribution(generator), distribution(generator)};

            Point sample_position;
            float light_pdf;
            RGB light_intensity = areaLight->Sample_L(r, &sample_position, light_pdf);

            Vector direction_to_light = sample_position.vec2point(isect.p);
            direction_to_light.normalize();

            Ray shadow_ray(isect.p + isect.gn * EPSILON, direction_to_light);

            // If the light sample is visible, calculate its contribution
            if (scene->visibility(shadow_ray, std::sqrt(direction_to_light.dot(direction_to_light)) - EPSILON)) {
                // The incident light direction is the normalized direction to the light
                Vector wi = direction_to_light.normalized();
                // The outgoing direction is the direction of the outgoing ray
                Vector wo = -isect.wo;
                // Calculate the contribution of the light sample to the accumulated light
                local_accumulated_light += light_intensity * f->f(wi, wo) * std::max(0.f, direction_to_light.dot(isect.sn)) *
                                           (1 / (direction_to_light.dot(direction_to_light) * light_pdf));
            }
        }

#pragma omp critical
        accumulated_light += local_accumulated_light;
    }

    RGB final_light = accumulated_light / float(num_light_samples);
    color += final_light;
    return color;
}


RGB DistributedShader::pointLight(Intersection isect, Phong* f, Light* l, RGB color) {
    std::cout << "PointLight used in shading." << std::endl;
    if (!f->Kd.isZero()) {
        Point lpoint;
        // get the position and radiance of the light source
        RGB L = (l)->Sample_L(nullptr, &lpoint);

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
    return color;
}

RGB DistributedShader::directLighting(Intersection isect, Phong* f) {
    RGB color(0.,0.,0.);
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::uniform_real_distribution<float> distribution(0.0, 1.0);

    for (auto & light : scene->lights) {
        if (light->type == AMBIENT_LIGHT) { // is it an ambient light ?
            if (!f->Ka.isZero()) {
                RGB Ka = f->Ka;
                color += Ka * light->L();
            }
            continue;
        }
        if (light->type == POINT_LIGHT) {
            color = pointLight(isect, f, light, color);
        }
        if (light->type == AREA_LIGHT) {
            color = areaLight(isect, f, light, color, distribution, generator);
        }
    }
    return color;
}

RGB DistributedShader::specularReflection(Intersection isect, Phong *f){
    // generate the specular ray
    float cos = isect.gn.dot(isect.wo);
    Vector Rdir = 2.f * cos * isect.gn - isect.wo;
    Ray specular(isect.p, Rdir);
    specular.adjustOrigin(isect.gn);
    Intersection s_isect;

    // trace ray
    bool intersected = scene->trace(specular, &s_isect);

    // shade this intersection
    RGB color = shade(intersected, s_isect, 0);

    return color;
}

RGB DistributedShader::shade(bool intersected, Intersection isect, int depth) {
    RGB color(0.,0.,0.);
    if (!intersected) return (background);

    // intersection with a light source
    if (isect.isLight) return isect.Le;
    Phong *f = (Phong *)isect.f;

    // if there is a specular component sample it
    if (!f->Ks.isZero()) color += specularReflection (isect, f);

    // if there is a diffuse component do direct light
    if (!f->Kd.isZero()) {
        color += directLighting(isect, f);

        /**
        // Monte Carlo sampling of the diffuse component
        int num_diffuse_samples = 16;  // Number of samples for the diffuse component
        RGB accumulated_diffuse(0.,0.,0.);

        for (int sample = 0; sample < num_diffuse_samples; ++sample) {
            // Generate a random direction over the hemisphere
            Vector wo;
            float pdf;
            RGB brdf_val = f->Sample_f(isect.wo, &pdf, &wo);

            // Trace a ray in this direction and get the radiance
            Ray bounce(isect.p, wo);
            bounce.adjustOrigin(isect.gn);
            Intersection bounce_isect;
            bool intersected = scene->trace(bounce, &bounce_isect);
            RGB bounce_radiance = shade(intersected, bounce_isect);

            // Add the contribution of this sample to the accumulated diffuse
            accumulated_diffuse += brdf_val * bounce_radiance / pdf;
        }

        RGB final_diffuse = accumulated_diffuse / float(num_diffuse_samples);
        color += final_diffuse;
    */
    }

    return color;
}

