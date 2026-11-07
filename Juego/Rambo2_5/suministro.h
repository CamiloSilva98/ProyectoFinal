#ifndef SUMINISTRO_H
#define SUMINISTRO_H

#include "entidad.h"

class Rambo;

enum class TipoSuministro { Energia, Municion };

class Suministro : public entidad
{
private:
    TipoSuministro tipo;
    int cantidad;

public:
    Suministro(float x, float y, TipoSuministro tipo, int cantidad);

    void actualizar(float dt) override;
    void aplicarA(Rambo &rambo);

    TipoSuministro getTipo() const;
    int getCantidad() const;
};

#endif