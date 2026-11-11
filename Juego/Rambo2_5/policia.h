#ifndef POLICIA_H
#define POLICIA_H

#include "enemigo.h"
#include "modelofisico.h"

class Nivel;

class Policia : public Enemigo
{
private:
    entidad *objetivo;          // no es dueño
    Nivel *nivel;               // no es dueño, aqui se agregan las bombas
    ModeloFisico *modeloBomba;  // no es dueño

    float rangoAtaque;
    float intervaloAtaque;
    float tiempoAtaque;
    float velocidadBombaX;
    float velocidadBombaY;
    int dañoBomba;

public:
    Policia(float x, float y, int salud, float velocidad,
            entidad *objetivo, Nivel *nivel, ModeloFisico *modeloBomba);

    void actualizar(float dt) override;
    void mover(float dt) override;
    void atacar(entidad &blanco) override;

    void setParametrosAtaque(float rango, float intervalo, float velX, float velY, int dano);
};

#endif
