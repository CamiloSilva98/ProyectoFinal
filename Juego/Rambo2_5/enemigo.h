#ifndef ENEMIGO_H
#define ENEMIGO_H

#include "entidad.h"

// clase base abstracta de los enemigos (Guardia y Policia heredan de aqui)
class Enemigo : public entidad
{
protected:
    float velocidad;   // rapidez de desplazamiento

public:
    Enemigo(float x, float y, int salud, float velocidad);
    ~Enemigo() override;

    // por defecto un enemigo solo se mueve, las subclases pueden ampliarlo
    void actualizar(float dt) override;

    // cada tipo de enemigo decide como se desplaza y como ataca
    virtual void mover(float dt) = 0;
    virtual void atacar(entidad &objetivo) = 0;

    float getVelocidad() const;
    void setVelocidad(float v);
};

#endif
