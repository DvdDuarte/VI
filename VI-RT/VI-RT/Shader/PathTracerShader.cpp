//
// Created by jafmalheiro on 14/05/2023.
//

#include <omp.h>
#include "PathTracerShader.hpp"
#include "AreaLight.hpp"

RGB PathTracerShader::areaLight(Intersection isect, Phong* f, Light* l, RGB color, std::uniform_real_distribution<float> distribution, std::default_random_engine generator) {
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

RGB PathTracerShader::areaLight_V2(Intersection isect, Phong* f, Light* l, RGB color, std::uniform_real_distribution<float> distribution, std::default_random_engine generator){
    if (!f->Kd.isZero()) {
        RGB L, Kd = f->Kd;
        Point lpoint;
        float l_pdf;
        AreaLight *al = dynamic_cast<AreaLight*>(l);

        float rnd[2];
        rnd[0] = distribution(generator);
        rnd[1] = distribution(generator);
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
        if (cosL>0. and cosL_LA<=0.) { // light NOT behind primitive AND light normal points to the ray o

            // generate the shadow ray
            Ray shadow(isect.p, Ldir);

            // adjust origin EPSILON along the normal: avoid self occlusion
            shadow.adjustOrigin(isect.gn);

            if (scene->visibility(shadow, Ldistance-EPSILON)) { // light source not occluded
                color += (Kd * L * cosL) / l_pdf;
            }
        } // end cosL > 0.
    }

    return color;
}

RGB PathTracerShader::pointLight(Intersection isect, Phong* f, Light* l, RGB color) {
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

RGB PathTracerShader::directLighting(Intersection isect, Phong* f) {
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
            color = areaLight_V2(isect, f, light, color, distribution, generator);
        }
    }
    return color;
}

RGB PathTracerShader::specularReflection(Intersection isect, Phong *f, int depth){
    RGB color(0.,0.,0.); Vector Rdir, s_dir; float pdf; Intersection s_isect;
    float cos = isect.gn.dot(isect.wo);
    Rdir = 2.f * cos * isect.gn - isect.wo;
    if (f->Ns >= 1000) { // ideal specular
        Ray specular(isect.p, Rdir);
        specular.adjustOrigin(isect.gn);
        // trace ray
        bool intersected = scene->trace(specular, &s_isect);
        RGB Rcolor = shade(intersected, s_isect, depth + 1);
        color = (f->Ks * Rcolor);
    }

    if (f->Ns < 1000) { // glossy materials
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::default_random_engine generator(seed);
        std::uniform_real_distribution<float> distribution(0.0, 1.0);

        float rnd[2] = {distribution(generator), distribution(generator)};
        Vector S_around_N;
        const float cos_theta = powf(rnd[1], 1. / (f->Ns + 1.));
        S_around_N.Z = cos_theta;
        const float aux_r1 = powf(rnd[1], 2. / (f->Ns + 1.));
        S_around_N.Y = sinf(2. * M_PI * rnd[0]) * sqrtf(1. - aux_r1);
        S_around_N.X = cosf(2. * M_PI * rnd[0]) * sqrtf(1. - aux_r1);
        pdf = (f->Ns + 1.f) * powf(cos_theta, f->Ns) / (2.f * M_PI);
        Vector Rx, Ry;
        Rdir.CoordinateSystem(&Rx, &Ry);
        s_dir = S_around_N.Rotate(Rx, Ry, Rdir);
        Ray specular(isect.p, s_dir);
        specular.adjustOrigin(isect.gn);
        bool intersected = scene->trace(specular, &s_isect);
        RGB Rcolor = shade(intersected, s_isect, depth + 1);
        color = (f->Ks * Rcolor * powf(cos_theta, f->Ns) / (2.f * M_PI)) / pdf;
    }
        return color;

}

RGB PathTracerShader::diffuseReflection(Intersection isect, Phong *f, int depth) {
    RGB color(0.,0.,0.); Vector dir; float pdf;

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::uniform_real_distribution<float> distribution(0.0, 1.0);

    // actual direction distributed around N: 2 random number in [0,1[
    float rnd[2] = { distribution(generator),distribution(generator) };
    Vector D_around_Z;
    float cos_theta= D_around_Z.Z = sqrtf(rnd[1]); // cos sampling
    D_around_Z.Y = sinf(2.*M_PI*rnd[0])*sqrtf(1.-rnd[1]);
    D_around_Z.X = cosf(2.*M_PI*rnd[0])*sqrtf(1.-rnd[1]);
    pdf = cos_theta / ( M_PI );

    // generate a coordinate system from N
    Vector Rx, Ry;
    isect.gn.CoordinateSystem(&Rx, &Ry);

    Ray diffuse(isect.p, D_around_Z.Rotate (Rx, Ry, isect.gn));

    // OK, we have the ray : trace and shade it recursively
    bool intersected = scene->trace(diffuse, &isect);

    // if light source return 0 ; handled by direct
    if (!isect.isLight) { // shade this intersection
        RGB Rcolor = shade (intersected, isect, depth+1);
        color = (f->Kd * cos_theta * Rcolor) /pdf ;
    }
    return color;
}


RGB PathTracerShader::shade(bool intersected, Intersection isect, int depth) {
    RGB color(0.,0.,0.);
    if (!intersected) return (background);

    // intersection with a light source
    if (isect.isLight) return isect.Le;
    Phong *f = (Phong *)isect.f;

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::uniform_real_distribution<float> distribution(0.0, 1.0);

    float rnd_russian = distribution(generator);
    if (depth < MAX_DEPTH  || rnd_russian < continue_p) {
        RGB lcolor;

        // random select between specular and diffuse
        float s_p = f->Ks.Y() /(f->Ks.Y()+f->Kd.Y());
        float rnd = distribution(generator);
        if (rnd < s_p) // do specular
            lcolor = specularReflection (isect, f, depth+1) / s_p;
        else // do diffuse
            lcolor = diffuseReflection (isect, f, depth+1) / (1.-s_p);
        color += lcolor;
    }
// if there is a diffuse component do direct light
    if (!f->Kd.isZero()) color += directLighting(isect, f);
    return color;
};


