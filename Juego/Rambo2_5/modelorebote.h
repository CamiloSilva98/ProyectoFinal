#ifndef MODELOREBOTE_H
#define MODELOREBOTE_H

#include "modelofisico.h"

class ModeloRebote : public ModeloFisico
{
private:
    float restitucion;   // coeficiente entre 0 y 1

public:
    ModeloRebote(float restitucion) : restitucion(restitucion) {}

    // entre choques el movimiento es solo desplazamiento, el rebote se aplica aparte
    void calcularMovimiento(float &x, float &y, float &vx, float &vy, float dt) override
    {
        x += vx * dt;
        y += vy * dt;
    }

    // refleja la velocidad respecto a la normal (nx, ny) de la superficie, que debe ser unitaria
    void calcularRebote(float &vx, float &vy, float nx, float ny)
    {
        float vn = vx * nx + vy * ny;
        vx = restitucion * (vx - 2.0f * vn * nx);
        vy = restitucion * (vy - 2.0f * vn * ny);
    }

    void setRestitucion(float r) { restitucion = r; }
    float getRestitucion() const { return restitucion; }
};

#endif