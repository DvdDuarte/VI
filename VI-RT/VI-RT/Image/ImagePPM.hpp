//
//  ImagePPM.hpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 09/03/2023.
//

#pragma once

#include <fstream>
#include <algorithm>
#include "Image.hpp"

class ImagePPM: public Image {
    typedef struct {
        unsigned char val[3];  // r,g,b
    } PPM_pixel;
    PPM_pixel *imageToSave;
    void ToneMap ();
public:
    ImagePPM(const int W, const int H):Image(W, H) {}
    bool Save (std::string filename);
};

