//
// Created by jafmalheiro on 14/05/2023.
//

#include "PathTracerShader.hpp"
#include "AreaLight.hpp"
#include "PointLight.hpp"

RGB PathTracerShader::areaLight(Intersection isect, Phong* f, Light* l, RGB color, std::uniform_real_distribution<float> distribution, std::default_random_engine generator) {
    int num_samples = 3;
    RGB accumulated_color(0., 0., 0.);

    if (!f->Kd.isZero()) {
        RGB Kd = f->Kd;
        auto* al = dynamic_cast<AreaLight*>(l);

        for (int i = 0; i < num_samples; ++i) {
            float rnd[2];
            rnd[0] = distribution(generator);
            rnd[1] = distribution(generator);

            // Iterate over all the area light sources
            for (size_t j = 0; j < al->gems.size(); ++j) {
                RGB L;
                Point lpoint;
                float l_pdf;

                // Sample the area light source
                L = al->Sample_L(rnd, &lpoint, l_pdf);

                Vector Ldir = isect.p.vec2point(lpoint);
                const float Ldistance = Ldir.norm();
                Ldir.normalize();

                float cosL = Ldir.dot(isect.sn);
                float cosL_LA = Ldir.dot(al->gems[j]->normal);

                if (cosL > 0. && cosL_LA <= 0.) {
                    Ray shadow(isect.p, Ldir);
                    shadow.adjustOrigin(isect.gn);

                    if (scene->visibility(shadow, Ldistance - EPSILON)) {
                        accumulated_color += (Kd * L * cosL) / l_pdf;
                    }
                }
            }
        }
    }

    color += accumulated_color / num_samples;
    return color;
}


RGB PathTracerShader::pointLight(Intersection isect, Phong* f, Light* l, RGB color) {
    if (!f->Kd.isZero()) {
        auto* pl = dynamic_cast<PointLight*>(l);

        for (int i = 0; i < pl->positions.size(); i++) {
            // get the position and radiance of the light source
            Point lpoint;
            RGB L = pl->colors[i];

            // compute the direction from the intersection to the light
            Vector Ldir = isect.p.vec2point(pl->positions[i]);

            // Compute the distance between the intersection and the light source
            const float Ldistance = Ldir.norm();

            Ldir.normalize(); // now normalize Ldir

            // compute the cosine (Ldir , shading normal)
            float cosL = Ldir.dot(isect.sn);

            if (cosL > 0.) { // the light is NOT behind the primitive

                // generate the shadow ray
                Ray shadow(isect.p, Ldir);

                // adjust origin EPSILON along the normal: avoid self occlusion
                shadow.adjustOrigin(isect.gn);

                if (scene->visibility(shadow, Ldistance - EPSILON)) // light source not occluded
                    color += f->Kd * L * cosL;
            } // end cosL > 0.
            // the light is behind the primitive
        }
    }
    return color;
}


RGB PathTracerShader::directLighting(Intersection isect, Phong* f) {
    RGB color(0.,0.,0.);
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::uniform_real_distribution<float> distribution(0.0, 1.0);

    // Seleciona aleatoriamente uma luz
    int chosen_idx = (int)(floor(distribution(generator) * scene->numLights));
    Light* chosen_light = scene->lights[chosen_idx];

    if (chosen_light->type == AMBIENT_LIGHT) {
        if (!f->Ka.isZero()) {
            RGB Ka = f->Ka;
            color += Ka * chosen_light->L();
        }
    } else if (chosen_light->type == POINT_LIGHT) {
        color = pointLight(isect, f, chosen_light, color);
    } else if (chosen_light->type == AREA_LIGHT) {
        color = areaLight(isect, f, chosen_light, color, distribution, generator);
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
    diffuse.adjustOrigin(isect.gn);

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

    float gamma = 2.2;
    color.R = pow(color.R, 1.0 / gamma);
    color.G = pow(color.G, 1.0 / gamma);
    color.B = pow(color.B, 1.0 / gamma);

    return color;
};


