//
//  ImagePPM.cpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 09/03/2023.
//

#include "ImagePPM.hpp"
#include <iostream>
#include <algorithm>

// Constructor
ImagePPM::ImagePPM(const int W, const int H) : Image(W, H) {
    imagePlane = new RGB[W * H];
    imageToSave = nullptr;
}

// Destructor
ImagePPM::~ImagePPM() {
    delete[] imagePlane;
    delete[] imageToSave;
}

void ImagePPM::SetPixel(const int x, const int y, const RGB &color) {
    imagePlane[y * W + x] = color;
}

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

    delete[] imageToSave;
    imageToSave = nullptr;

    return true;
}

bool ImagePPM::Load(const std::string& filename) {
    // Open file for reading
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file " << filename << " for reading" << std::endl;
        return false;
    }

    // Read header information from file
    std::string magic_number;
    int width, height, max_val;
    file >> magic_number >> width >> height >> max_val;
    file.get(); // Consume the newline character

    if (magic_number != "P6") {
        std::cerr << "Unsupported PPM format. Only P6 is supported." << std::endl;
        return false;
    }

    delete[] imagePlane;
    imagePlane = new RGB[width * height];

    for (int j = 0; j < height; ++j) {
        for (int i = 0; i < width; ++i) {
            unsigned char r, g, b;
            file.read((char*)&r, 1);
            file.read((char*)&g, 1);
            file.read((char*)&b, 1);

            imagePlane[j * width + i].R = r / 255.0f;
            imagePlane[j * width + i].G = g / 255.0f;
            imagePlane[j * width + i].B = b / 255.0f;
        }
    }

    file.close();

    W = width;
    H = height;

    return true;
}
