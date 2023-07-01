#ifndef VI_RT_AREALIGHT_HPP
#define VI_RT_AREALIGHT_HPP

#include "light.hpp"
#include "RGB.hpp"
#include "vector.hpp"
#include "Triangle.hpp"

class AreaLight: public Light {
public:
    std::vector<Triangle*> gems; // Vetor para armazenar múltiplas fontes de luz
    std::vector<float> pdfs; // Vetor para armazenar os valores de pdf correspondentes a cada fonte de luz
    RGB power; // Potência total da luz de área

    AreaLight (RGB _power) : power(_power) {
        type = AREA_LIGHT;
    }

    ~AreaLight () {
        for (auto gem : gems) {
            delete gem;
        }
    }

    void addSource(Point _v1, Point _v2, Point _v3, Vector _n) {
        Triangle* gem = new Triangle(_v1, _v2, _v3, _n);
        gems.push_back(gem);
        float pdf = 1.f / gem->area();
        pdfs.push_back(pdf);
    }

    RGB Sample_L (float *r, Point *p, float &_pdf) {
        // Escolha aleatoriamente uma das fontes de luz
        int index = static_cast<int>(r[0] * gems.size());
        Triangle* gem = gems[index];
        float pdf = pdfs[index];

        // Amostragem da fonte de luz selecionada
        const float sqrt_r0 = sqrtf(r[1]);
        const float alpha = 1.f - sqrt_r0;
        const float beta = (1.f - r[2]) * sqrt_r0;
        const float gamma = r[2] * sqrt_r0;
        p->X = alpha * gem->v1.X + beta * gem->v2.X + gamma * gem->v3.X;
        p->Y = alpha * gem->v1.Y + beta * gem->v2.Y + gamma * gem->v3.Y;
        p->Z = alpha * gem->v1.Z + beta * gem->v2.Z + gamma * gem->v3.Z;
        _pdf = pdf;

        // Retorna a intensidade da fonte de luz selecionada
        return power * pdf;
    }
};

#endif // VI_RT_AREALIGHT_HPP
