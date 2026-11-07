#include "rambo.h"

Rambo::Rambo(float x, float y, int salud, int municion, float velocidad)
    : entidad(x, y, salud), municion(municion), velocidad(velocidad), fatiga(0.0f){}

// por ahora no hay nada que dependa solo del tiempo
void Rambo::actualizar(float dt)
{
    (void)dt;
}

// dirX y dirY van de -1 a 1, el nivel decide que direcciones usa
void Rambo::mover(float dirX, float dirY, float dt)
{
    x += dirX * velocidad * dt;
    y += dirY * velocidad * dt;
}

// devuelve true si pudo disparar, gasta una municion
bool Rambo::disparar()
{
    if (municion <= 0) return false;
    municion--;
    return true;
}

void Rambo::agregarMunicion(int cantidad)
{
    municion += cantidad;
}

void Rambo::reducirFatiga(float cantidad)
{
    fatiga -= cantidad;
    if (fatiga < 0.0f) fatiga = 0.0f;
}

float Rambo::getFatiga() const { return fatiga; }
int Rambo::getMunicion() const { return municion; }
float Rambo::getVelocidad() const { return velocidad; }
void Rambo::setVelocidad(float v) { velocidad = v; }

