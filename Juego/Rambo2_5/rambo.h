#ifndef RAMBO_H
#define RAMBO_H

#include "entidad.h"
#include "suministro.h"

class Suministro;

class Rambo : public entidad

{
private:
    int municion;
    float velocidad;
    float fatiga;

public:
    Rambo(float x, float y, int salud, int municion, float velocidad);

    void actualizar(float dt) override;
    void mover(float dirX, float dirY, float dt);
    bool disparar();
    void agregarMunicion(int cantidad);
    bool recogerSuministro(Suministro &suministro);
    void reducirFatiga(float cantidad);
    float getFatiga() const;

    int getMunicion() const;
    float getVelocidad() const;
    void setVelocidad(float v);
};

#endif