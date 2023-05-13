//
// Created by jafmalheiro on 23/03/2023.
//

#include "StandardRenderer.hpp"
#include "perspective.hpp"

const int spp=32;

void StandardRenderer::Render () {
    int W=0,H=0;  // resolution
    int x,y,ss;

    // get resolution from the camera
    Perspective* perspCam = dynamic_cast<Perspective*>(cam);
    perspCam->getResolution(&W, &H);

    // main rendering loop: get primary rays from the camera until done
    for (y=0 ; y< H ; y++) {  // loop over rows
        for (x=0 ; x< W ; x++) { // loop over columns
            RGB color(0.,0.,0.), this_color;
            Ray primary;
            Intersection isect;
            bool intersected;
            int depth = 0;

            for (ss=0 ; ss < spp ; ss++) {
                float jitterV[2];

                jitterV[0] = getRandom(0,1);
                jitterV[1] = getRandom(0,1);

                // Generate Ray (camera)
                cam->GenerateRay(x, y, &primary, jitterV);

                // trace ray (scene)
                intersected = scene->trace(primary, &isect);

                // shade this intersection (shader)
                this_color = shd->shade (intersected, isect, depth);
                color += this_color;
            }
            color = color.operator*(1/spp);

            // write the result into the image frame buffer (image)
            img->set(x,y,color);

        } // loop over columns
    }   // loop over rows
}
