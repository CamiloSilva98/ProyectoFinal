#ifndef MODELOPARABOLICO_H
#define MODELOPARABOLICO_H

#include "modelofisico.h"

class ModeloParabolico : public ModeloFisico
{
private:
    float g;   // gravedad, positiva hacia abajo

public:
    ModeloParabolico(float g) : g(g) {}

    // version por pasos para el ciclo del juego
    void calcularMovimiento(float &x, float &y, float &vx, float &vy, float dt) override
    {
        x += vx * dt;
        y += vy * dt + 0.5f * g * dt * dt;
        vy += g * dt;
    }

    // version directa con las ecuaciones x(t) y y(t)
    void calcularTrayectoria(float x0, float y0, float v0x, float v0y, float t, float &x, float &y)
    {
        x = x0 + v0x * t;
        y = y0 + v0y * t + 0.5f * g * t * t;
    }
};

#endif