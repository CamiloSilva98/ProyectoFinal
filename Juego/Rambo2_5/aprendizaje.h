#ifndef APRENDIZAJE_H
#define APRENDIZAJE_H

#include <vector>
#include "puntosospecha.h"

class Aprendizaje
{
private:
    std::vector<PuntoSospecha> memoria;
    std::size_t capacidad;

    int buscarPunto(float x, float y) const;

public:
    explicit Aprendizaje(std::size_t capacidad);

    void registrarResultado(float x, float y, bool encontroRambo);

    // cuantas veces fue falsa la alarma cerca de ese punto
    int consultarExperiencias(float x, float y) const;
};

#endif
