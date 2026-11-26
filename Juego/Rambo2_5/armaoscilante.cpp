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
    static const float LARGO_CANON = 37.0f;   // del pivote a la punta, en px de pantalla
    float puntaX = x + LARGO_CANON * std::cos(anguloActual);
    float puntaY = y + LARGO_CANON * std::sin(anguloActual);
    float vx = velocidadProyectil * std::cos(anguloActual);
    float vy = velocidadProyectil * std::sin(anguloActual);
    Proyectil *p = new Proyectil(puntaX, puntaY, vx, vy, danoProyectil, modeloProyectil, rebote, 1);
    p->setDeTorreta(true);
    return p;
}

float ArmaOscilante::getAngulo() const { return anguloActual; }
