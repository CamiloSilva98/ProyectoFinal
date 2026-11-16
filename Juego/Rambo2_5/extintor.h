#ifndef EXTINTOR_H
#define EXTINTOR_H

#include "entidad.h"

class Extintor : public entidad
{
public:
    Extintor(float x, float y);

    void actualizar(float dt) override;
};

#endif