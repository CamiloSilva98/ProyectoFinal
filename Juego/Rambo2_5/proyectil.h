#ifndef PROYECTIL_H
#define PROYECTIL_H

#include "entidad.h"
#include "modelofisico.h"
#include "modelorebote.h"

class Proyectil : public entidad
{
private:
    float velocidadX;
    float velocidadY;
    int dano;
    int rebotesMax;
    int rebotesHechos;
    ModeloFisico *modelo;   // movimiento, no es dueno
    ModeloRebote *rebote;   // puede ser nullptr si no rebota, no es dueno

public:
    Proyectil(float x, float y, float vx, float vy, int dano,
              ModeloFisico *modelo, ModeloRebote *rebote = nullptr, int rebotesMax = 0);

    void actualizar(float dt) override;
    void mover(float dt);
    void colisionar(float nx, float ny);

    int getDano() const;
    float getVelocidadX() const;
    float getVelocidadY() const;
};

#endif