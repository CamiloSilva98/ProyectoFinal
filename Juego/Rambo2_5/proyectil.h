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
    int daño;
    int rebotesMax;
    int rebotesHechos;
    ModeloFisico *modelo;   // movimiento, no es dueño
    ModeloRebote *rebote;   // puede ser nullptr si no rebota, no es dueño
    bool deRambo;

public:
    Proyectil(float x, float y, float vx, float vy, int dano,
              ModeloFisico *modelo, ModeloRebote *rebote = nullptr, int rebotesMax = 0);

    void actualizar(float dt) override;
    void mover(float dt);
    void colisionar(float nx, float ny);

    int getDaño() const;
    void setDeRambo(bool valor);
    bool esDeRambo() const;
    float getVelocidadX() const;
    float getVelocidadY() const;
};

#endif