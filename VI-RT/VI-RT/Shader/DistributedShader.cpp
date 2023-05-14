//
// Created by jafmalheiro on 12/05/2023.
//

#include <chrono>
#include <random>
#include "DistributedShader.hpp"
#include "AreaLight.hpp"

RGB DistributedShader::directLighting(Intersection isect, Phong* f) {
    RGB color(0.,0.,0.);
    int numLights = scene->lights.size();  // Get the number of lights in the scene
    int lightThreshold = 10;  // Define your own threshold

    for (auto l = scene->lights.begin(); l != scene->lights.end(); l++) {
        if ((*l)->type == AMBIENT_LIGHT) { // is it an ambient light ?
            if (!f->Ka.isZero()) {
                RGB Ka = f->Ka;
                color += Ka * (*l)->L();
            }
            continue;
        }
        if ((*l)->type == POINT_LIGHT) { // is it a point light ?
            if (!f->Kd.isZero()) {
                Point lpoint;
                // get the position and radiance of the light source
                RGB L = (*l)->Sample_L(nullptr, &lpoint);

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
        if ((*l)->type == AREA_LIGHT) { // is it an area light ?
            if (numLights < lightThreshold) {
                if (!f->Kd.isZero()) {
                    RGB L, Kd = f->Kd;
                    Point lpoint;
                    float l_pdf;
                    AreaLight *al = dynamic_cast<AreaLight *>(*l);

                    float rnd[2];
                    rnd[0] = ((float) rand()) / ((float) RAND_MAX);
                    rnd[1] = ((float) rand()) / ((float) RAND_MAX);
                    L = al->Sample_L(rnd, &lpoint, l_pdf);

                    // compute the direction from the intersection point to the light source
                    Vector Ldir = isect.p.vec2point(lpoint);
                    const float Ldistance = Ldir.norm();

                    // now normalize Ldir
                    Ldir.normalize();

                    // cosine between Ldir and the shading normal at the intersection point
                    float cosL = Ldir.dot(isect.sn);

                    // cosine between Ldir and the area light source normal
                    float cosL_LA = Ldir.dot(al->gem->normal);

                    // shade
                    if (cosL > 0. and
                        cosL_LA <= 0.) { // light NOT behind primitive AND light normal points to the ray o

                        // generate the shadow ray
                        Ray shadow(isect.p, Ldir);

                        // adjust origin EPSILON along the normal: avoid self occlusion
                        shadow.adjustOrigin(isect.gn);

                        if (scene->visibility(shadow, Ldistance - EPSILON)) { // light source not occluded
                            color += (Kd * L * cosL) / l_pdf;
                        }
                    } // end cosL > 0.
                }
            } else {
                // Perform Monte Carlo sampling of the area light
                auto *areaLight = dynamic_cast<AreaLight *>(*l);
                int num_light_samples = 64;
                RGB accumulated_light(0., 0., 0.);

#pragma omp parallel for default(none) shared(areaLight, isect, f, num_light_samples) reduction(+:accumulated_light)
                for (int sample = 0; sample < num_light_samples; ++sample) {
                    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
                    std::default_random_engine generator(seed);
                    std::uniform_real_distribution<float> distribution(0.0, 1.0);
                    float r1 = distribution(generator);  // Generates a random float between 0 and 1
                    float r2 = distribution(generator);  // Generate a random float between 0 and 1
                    float r[2] = {r1, r2};

                    Point sample_position;
                    float light_pdf;
                    RGB light_intensity = areaLight->Sample_L(r, &sample_position, light_pdf);

                    Vector direction_to_light = sample_position.vec2point(isect.p);
                    float distance_squared = direction_to_light.dot(direction_to_light);
                    direction_to_light.normalize();

                    // Adjust the origin point of the ray to avoid self-intersection
                    Point adjusted_origin(isect.p.X + isect.gn.X * EPSILON, isect.p.Y + isect.gn.Y * EPSILON,
                                          isect.p.Z + isect.gn.Z * EPSILON);
                    Ray shadow_ray(adjusted_origin, direction_to_light);

                    bool lightVisible = scene->visibility(shadow_ray, std::sqrt(distance_squared) - EPSILON);

                    // If the light sample is visible, calculate its contribution
                    if (lightVisible) {
                        // The incident light direction is the normalized direction to the light
                        Vector wi = direction_to_light.normalized();
                        // The outgoing direction is the direction of the outgoing ray
                        Vector wo = -isect.wo;
                        RGB brdf_val = f->f(wi, wo);
                        // Calculate the cosine of the angle between the incident light direction and the surface normal
                        float cos_theta = std::max(0.f, direction_to_light.dot(isect.sn));
                        // Calculate the contribution of the light sample to the accumulated light
                        accumulated_light += brdf_val * cos_theta * (light_intensity / (distance_squared * light_pdf));
                    }
                }

                RGB final_light = accumulated_light / float(num_light_samples);
                color += final_light;
            }
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
    RGB color = shade(intersected, s_isect);

    return color;
}

RGB DistributedShader::shade(bool intersected, Intersection isect) {
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

        // Monte Carlo sampling of the diffuse component
        int num_diffuse_samples = 64;  // Number of samples for the diffuse component
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
    }

    return color;
}

