#ifndef NUBEHUMO_H
#define NUBEHUMO_H

#include "entidad.h"

class NubeHumo : public entidad
{
private:
    float tiempoVida;

public:
    NubeHumo(float x, float y, float radio, float duracion);

    void actualizar(float dt) override;
    float getTiempoVida() const;
};

#endif

