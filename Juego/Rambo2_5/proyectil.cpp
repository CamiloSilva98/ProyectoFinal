#include "proyectil.h"

Proyectil::Proyectil(float x, float y, float vx, float vy, int daño,
                     ModeloFisico *modelo, ModeloRebote *rebote, int rebotesMax)
    : entidad(x, y, 1), velocidadX(vx), velocidadY(vy), daño(daño),
    rebotesMax(rebotesMax), rebotesHechos(0), modelo(modelo), rebote(rebote), deRambo(false){}

void Proyectil::actualizar(float dt)
{
    mover(dt);
}

// avanza segun el modelo fisico asignado
void Proyectil::mover(float dt)
{
    if (modelo == nullptr) return;
    modelo->calcularMovimiento(x, y, velocidadX, velocidadY, dt);
}

// (nx, ny) es la normal unitaria de la superficie golpeada
void Proyectil::colisionar(float nx, float ny)
{
    if (rebote == nullptr || rebotesHechos >= rebotesMax)
    {
        salud = 0;   // el proyectil deja de estar vivo
        return;
    }
    rebote->calcularRebote(velocidadX, velocidadY, nx, ny);
    rebotesHechos++;
}

int Proyectil::getDaño() const { return daño; }
float Proyectil::getVelocidadX() const { return velocidadX; }
float Proyectil::getVelocidadY() const { return velocidadY; }

void Proyectil::setDeRambo(bool valor) { deRambo = valor; }
bool Proyectil::esDeRambo() const { return deRambo; }