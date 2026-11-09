#include "enemigo.h"

Enemigo::Enemigo(float x, float y, int salud, float velocidad)
    : entidad(x, y, salud), velocidad(velocidad) {}

Enemigo::~Enemigo() {}

// el enemigo solo actua mientras esta vivo
void Enemigo::actualizar(float dt)
{
    if (!estaViva()) return;
    mover(dt);
}

float Enemigo::getVelocidad() const { return velocidad; }
void Enemigo::setVelocidad(float v) { velocidad = v; }
