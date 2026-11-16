#ifndef ONDARUIDO_H
#define ONDARUIDO_H


#include <vector>
#include "entidad.h"
#include "modeloonda.h"

class OndaRuido : public entidad
{
private:
    ModeloOnda modelo;
    std::vector<const void*> oyentes;

public:
    OndaRuido(float x, float y, float velocidadSonido, float radioMax, float intensidadInicial);

    void actualizar(float dt) override;

    // cada onda la escucha una sola vez cada oyente
    bool escuchadaPor(const void *oyente, float distancia, float intensidadMin, float &intensidad);
};

#endif // ONDARUIDO_H
