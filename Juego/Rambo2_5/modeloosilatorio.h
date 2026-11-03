#ifndef MODELOOSCILATORIO_H
#define MODELOOSCILATORIO_H

#include <cmath>
#include "modelofisico.h"

class ModeloOscilatorio : public ModeloFisico
{
private:
    float amplitud;     // A
    float frecuencia;   // omega, velocidad angular
    float fase;         // fase acumulada

public:
    ModeloOscilatorio(float amplitud, float frecuencia, float fase = 0.0f)
        : amplitud(amplitud), frecuencia(frecuencia), fase(fase) {}

    // aqui solo avanza la fase, x y y no se tocan porque el modelo es angular
    void calcularMovimiento(float &x, float &y, float &vx, float &vy, float dt) override
    {
        (void)x; (void)y; (void)vx; (void)vy;
        fase += frecuencia * dt;
    }

    // theta(t) = theta0 + A sin(fase)
    float calcularAngulo(float thetaCentro) const
    {
        return thetaCentro + amplitud * std::sin(fase);
    }

    void setAmplitud(float a) { amplitud = a; }
    void setFrecuencia(float w) { frecuencia = w; }
    float getAmplitud() const { return amplitud; }
    float getFrecuencia() const { return frecuencia; }
};

#endif