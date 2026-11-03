#include "entidad.h"

entidad::entidad(float x, float y, int salud)
    : x(x), y(y), salud(salud){}

entidad::~entidad(){}

// reduce la salud, no deja que baje de cero
void entidad::recibirDano(int dano)
{
    salud -= dano;
    if (salud < 0) salud = 0;
}

float entidad::getX() const { return x; }

float entidad::getY() const { return y; }

int entidad::getSalud() const { return salud; }

bool entidad::estaViva() const { return salud > 0; }
