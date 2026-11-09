#include "armaoscilante.h"
#include <cmath>

ArmaOscilante::ArmaOscilante(float x, float y, float thetaCentro, float velocidadProyectil,
                             int danoProyectil, ModeloOscilatorio *oscilador,
                             ModeloFisico *modeloProyectil, ModeloRebote *rebote)
    : entidad(x, y, 1), thetaCentro(thetaCentro), anguloActual(thetaCentro),
    velocidadProyectil(velocidadProyectil), danoProyectil(danoProyectil),
    oscilador(oscilador), modeloProyectil(modeloProyectil), rebote(rebote)
{
}

void ArmaOscilante::actualizar(float dt)
{
    actualizarAngulo(dt);
}

// avanza la fase del oscilador y recalcula el angulo del arma
void ArmaOscilante::actualizarAngulo(float dt)
{
    if (oscilador == nullptr) return;

    // el modelo oscilatorio no usa estas variables, solo avanza su fase
    float a = 0, b = 0, c = 0, d = 0;
    oscilador->calcularMovimiento(a, b, c, d, dt);
    anguloActual = oscilador->calcularAngulo(thetaCentro);
}

// crea el proyectil con la direccion actual del arma
Proyectil *ArmaOscilante::disparar()
{
    float vx = velocidadProyectil * std::cos(anguloActual);
    float vy = velocidadProyectil * std::sin(anguloActual);
    return new Proyectil(x, y, vx, vy, danoProyectil, modeloProyectil, rebote, 1);
}

float ArmaOscilante::getAngulo() const { return anguloActual; }