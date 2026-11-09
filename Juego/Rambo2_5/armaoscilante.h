#ifndef ARMAOSCILANTE_H
#define ARMAOSCILANTE_H

#include "entidad.h"
#include "modelooscilatorio.h"
#include "modelofisico.h"
#include "modelorebote.h"
#include "proyectil.h"

class ArmaOscilante : public entidad
{
private:
    float thetaCentro;          // angulo de equilibrio en radianes
    float anguloActual;
    float velocidadProyectil;
    int danoProyectil;
    ModeloOscilatorio *oscilador;   // no es dueno
    ModeloFisico *modeloProyectil;  // no es dueno
    ModeloRebote *rebote;           // puede ser nullptr, no es dueno

public:
    ArmaOscilante(float x, float y, float thetaCentro, float velocidadProyectil, int danoProyectil,
                  ModeloOscilatorio *oscilador, ModeloFisico *modeloProyectil,
                  ModeloRebote *rebote = nullptr);

    void actualizar(float dt) override;
    void actualizarAngulo(float dt);
    Proyectil *disparar();   // el que llama se encarga de liberar el proyectil

    float getAngulo() const;
};

#endif
