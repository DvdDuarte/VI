#ifndef PointLight_hpp
#define PointLight_hpp

#include "light.hpp"

class PointLight: public Light {
public:
    std::vector<Point> positions; // Vetor para armazenar múltiplas posições de luz
    std::vector<RGB> colors; // Vetor para armazenar as cores correspondentes a cada posição de luz

    PointLight() {
        type = POINT_LIGHT;
    }

    ~PointLight() {}

    void addSource(RGB _color, Point _pos) {
        colors.push_back(_color);
        positions.push_back(_pos);
    }

    RGB Sample_L (float *prob, Point *p) {
        // Escolha aleatoriamente uma das posições de luz
        int index = static_cast<int>(prob[0] * positions.size());
        Point pos = positions[index];
        RGB color = colors[index];

        // Retorna a intensidade da fonte de luz selecionada
        *p = pos;
        return color;
    }
};

#endif /* PointLight_hpp */