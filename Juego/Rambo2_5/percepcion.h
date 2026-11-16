#ifndef PERCEPCION_H
#define PERCEPCION_H

#include <vector>
#include "estimulo.h"
#include "guardia.h"

class Nivel1;

class Percepcion
{
private:
    Nivel1 *nivel;

    bool puedeVer(const Guardia &guardia, float px, float py) const;

public:
    explicit Percepcion(Nivel1 *nivel);

    std::vector<Estimulo> detectarEstimulos(const Guardia &guardia);
};

#endif