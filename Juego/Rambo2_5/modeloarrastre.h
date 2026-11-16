#ifndef MODELOARRASTRE_H
#define MODELOARRASTRE_H
#include <cmath>
#include "modelofisico.h"
class ModeloArrastre : public ModeloFisico
{
private:
    float arrastre;
    float masa;

public:
    ModeloArrastre(float arrastre, float masa)
        : arrastre(arrastre), masa(masa) {}

    void calcularMovimiento(float &x, float &y, float &vx, float &vy, float dt) override
    {
        float factor = std::exp(-(arrastre / masa) * dt);
        vx *= factor;
        vy *= factor;
        x += vx * dt;
        y += vy * dt;
    }
};


#endif // MODELOARRASTRE_H
