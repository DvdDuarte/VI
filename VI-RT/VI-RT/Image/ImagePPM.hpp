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

class ImagePPM : public Image {
private:
    struct PPM_pixel {
        unsigned char val[3]; // r, g, b
    };

    PPM_pixel* imageToSave;

    void ToneMap();

public:
    ImagePPM(const int width, const int height);

    ~ImagePPM();

    void SetPixel(const int x, const int y, const RGB& color);

    bool Save(const std::string filename);
    bool Load(const std::string& filename);
};

