#include "piedra.h"
#include <cmath>

Piedra::Piedra(float x, float y, float vx, float vy, ModeloFisico *modelo)
    : Proyectil(x, y, vx, vy, 0, modelo)
{
}

bool Piedra::estaDetenida(float velocidadMinima) const
{
    float vx = getVelocidadX();
    float vy = getVelocidadY();
    return std::sqrt(vx * vx + vy * vy) < velocidadMinima;
}
