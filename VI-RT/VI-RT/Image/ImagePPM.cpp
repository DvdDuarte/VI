//
//  ImagePPM.cpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 09/03/2023.
//

#include "ImagePPM.hpp"
#include <iostream>
#include <algorithm>

void ImagePPM::ToneMap() {
    imageToSave = new PPM_pixel[W * H];

    for (int j = 0; j < H; j++) {
        for (int i = 0; i < W; ++i) {
            imageToSave[j * W + i].val[0] = (unsigned char)(std::min(1.f, imagePlane[j * W + i].R) * 255);
            imageToSave[j * W + i].val[1] = (unsigned char)(std::min(1.f, imagePlane[j * W + i].G) * 255);
            imageToSave[j * W + i].val[2] = (unsigned char)(std::min(1.f, imagePlane[j * W + i].B) * 255);
        }
    }
}

bool ImagePPM::Save(std::string filename) {
    ToneMap();

    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file " << filename << " for writing" << std::endl;
        return false;
    }

    file << "P6\n";
    file << W << " " << H << "\n";
    file << "255\n";

    file.write((char*)imageToSave, W * H * sizeof(PPM_pixel));

    file.close();

    return true;
}
