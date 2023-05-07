//
// Created by jafmalheiro on 23/03/2023.
//

#include "StandardRenderer.hpp"
#include "perspective.hpp"

void StandardRenderer::Render () {
    int W=0,H=0;  // resolution
    int x,y;

    // get resolution from the camera
    Perspective* perspCam = dynamic_cast<Perspective*>(cam);
    perspCam->getResolution(&W, &H);

    // main rendering loop: get primary rays from the camera until done
    for (y=0 ; y< H ; y++) {  // loop over rows
        for (x=0 ; x< W ; x++) { // loop over columns
            Ray primary;
            Intersection isect;
            bool intersected;
            RGB color;
            int depth = 0;

            // Generate Ray (camera)
            perspCam->GenerateRay(x,y, &primary, nullptr);
            primary.x = x;
            primary.y = y;

            if(x == 512 && y == 512) {
                printf("here");
            }

            // trace ray (scene)
            intersected = scene->trace(primary, &isect);

            // shade this intersection (shader)
            color = shd->shade(intersected, isect, depth);

            // write the result into the image frame buffer (image)
            img->set(x,y,color);

        } // loop over columns
    }   // loop over rows
}
