#ifndef AGENTEGUARDIA_H
#define AGENTEGUARDIA_H

#include "agenteinteligente.h"
#include "percepcion.h"
#include "razonamiento.h"
#include "accionguardia.h"
#include "aprendizaje.h"

class Nivel1;

class AgenteGuardia : public AgenteInteligente
{
private:
    Guardia *guardia;
    Nivel1 *nivel;
    Percepcion percepcion;
    Razonamiento razonamiento;
    AccionGuardia accion;
    Aprendizaje aprendizaje;
    float velocidadDeteccion;

public:
    AgenteGuardia(Guardia *guardia, Nivel1 *nivel, std::size_t memoria, float velocidadDeteccion);

    void actualizar(float dt) override;

    Guardia *getGuardia() const;
    const Aprendizaje &getAprendizaje() const;
};

#endif
