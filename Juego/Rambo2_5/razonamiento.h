#ifndef RAZONAMIENTO_H
#define RAZONAMIENTO_H

#include <vector>
#include "estimulo.h"
#include "decision.h"
#include "aprendizaje.h"
#include "guardia.h"

class Razonamiento
{
public:
    // devuelve el indice del estimulo mas importante, o -1 si ninguno vale la pena
    int evaluarEstimulos(const std::vector<Estimulo> &estimulos, const Aprendizaje &aprendizaje) const;

    Decision decidirAccion(const std::vector<Estimulo> &estimulos, const Aprendizaje &aprendizaje,
                           const Guardia &guardia) const;
};

#endif

