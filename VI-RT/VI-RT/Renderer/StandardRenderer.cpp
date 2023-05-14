//
// Created by jafmalheiro on 23/03/2023.
//

#include "StandardRenderer.hpp"
#include "perspective.hpp"
#include <omp.h>

const int spp = 4;

void StandardRenderer::Render() {
    int W = 0, H = 0;  // resolution
    int ss, x = 0;

    // get resolution from the camera
    Perspective* perspCam = dynamic_cast<Perspective*>(cam);
    perspCam->getResolution(&W, &H);

    // set the number of threads for parallel processing
    omp_set_num_threads(4);

    // main rendering loop: get primary rays from the camera until done
#pragma omp parallel default(none) shared(perspCam, W, H) private(ss, x)
    {
#pragma omp for schedule(dynamic, 1) // Parallelize outer loop
        for (int y = 0; y < H; y++) {  // loop over rows
            for (x = 0; x < W; x++) { // loop over columns
                RGB color(0., 0., 0.), this_color;
                Ray primary;
                Intersection isect;
                bool intersected;
                int depth = 0;

                for (ss = 0; ss < spp; ss++) {

                    float jitter[2];

                    jitter[0] = getRandom(0, 1);
                    jitter[1] = getRandom(0, 1);

                    // Generate Ray (camera)
                    perspCam->GenerateRay(x, y, &primary, jitter);

                    // trace ray (scene)
                    intersected = scene->trace(primary, &isect);

                    // shade this intersection (shader)
                    this_color = shd->shade(intersected, isect, depth);

                    color += this_color;
                }
                color = color / spp;

                // write the result into the image frame buffer (image)
                img->set(x, y, color);

            } // loop over columns
        }   // loop over rows
    }
}


