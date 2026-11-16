#include "entidad.h"


entidad::entidad(float x, float y, int salud, float radio)
    : x(x), y(y), salud(salud), radio(radio){}

entidad::~entidad(){}

// reduce la salud, no deja que baje de cero
void entidad::recibirDano(int dano)
{
    salud -= dano;
    if (salud < 0) salud = 0;
}

float entidad::getX() const { return x; }

float entidad::getY() const { return y; }

float entidad::getRadio() const { return radio; }

void entidad::setPosicion(float nuevoX, float nuevoY)
{
    x = nuevoX;
    y = nuevoY;
}

int entidad::getSalud() const { return salud; }

bool entidad::estaViva() const { return salud > 0; }
