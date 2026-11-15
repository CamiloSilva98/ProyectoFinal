#ifndef MODELOONDA_H
#define MODELOONDA_H

#include "modelofisico.h"


class ModeloOnda : public ModeloFisico
{
private:
    float velocidadSonido;
    float radioMax;
    float intensidadInicial;
    float tiempo;

public:
    ModeloOnda(float velocidadSonido, float radioMax, float intensidadInicial)
        : velocidadSonido(velocidadSonido), radioMax(radioMax),
        intensidadInicial(intensidadInicial), tiempo(0.0f) {}

    // la onda no se desplaza, solo avanza su tiempo
    void calcularMovimiento(float &x, float &y, float &vx, float &vy, float dt) override
    {
        (void)x; (void)y; (void)vx; (void)vy;
        tiempo += dt;
    }

    float calcularRadio() const
    {
        float r = velocidadSonido * tiempo;
        return r > radioMax ? radioMax : r;
    }

    float calcularIntensidad(float distancia) const
    {
        if (distancia < 0.0f || distancia > radioMax) return 0.0f;
        return intensidadInicial * (1.0f - distancia / radioMax);
    }

    bool haAlcanzado(float distancia) const { return velocidadSonido * tiempo >= distancia; }
    bool haTerminado() const { return velocidadSonido * tiempo >= radioMax; }
};

#endif // MODELOONDA_H
