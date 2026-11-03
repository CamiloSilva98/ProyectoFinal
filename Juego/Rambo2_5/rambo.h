#ifndef RAMBO_H
#define RAMBO_H

#include "entidad.h"

class Rambo : public entidad
{
private:
    int municion;
    float velocidad;

public:
    Rambo(float x, float y, int salud, int municion, float velocidad);

    void actualizar(float dt) override;
    void mover(float dirX, float dirY, float dt);
    bool disparar();
    void agregarMunicion(int cantidad);

    int getMunicion() const;
    float getVelocidad() const;
    void setVelocidad(float v);
};

#endif