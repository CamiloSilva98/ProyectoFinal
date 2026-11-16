#ifndef PIEDRA_H
#define PIEDRA_H

#include "proyectil.h"

class Piedra : public Proyectil
{
public:
    Piedra(float x, float y, float vx, float vy, ModeloFisico *modelo);

    bool estaDetenida(float velocidadMinima) const;
};

#endif