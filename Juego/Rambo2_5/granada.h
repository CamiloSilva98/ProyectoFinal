#ifndef GRANADA_H
#define GRANADA_H

#include "proyectil.h"

// proyectil que no hace dano al golpear, solo al explotar tras tocar el suelo
class Granada : public Proyectil
{
private:
    int golpesParaExplotar;     // 1 o 2, al azar
    int golpesSuelo;
    bool explotada;
    bool explosionPendiente;    // true un solo instante para que el nivel aplique el dano
    float tiempoExplosion;
    float radioExplosion;

public:
    Granada(float x, float y, float vx, float vy, int danoExplosion, float radioExplosion,
            ModeloFisico *modelo, ModeloRebote *rebote);

    void actualizar(float dt) override;
    void golpearSuelo();            // cuenta el golpe: el primero puede rebotar, luego explota
    bool estaExplotando() const;
    bool consumirExplosion();       // devuelve true una sola vez, al explotar
    float getRadioExplosion() const;
    float getProgresoExplosion() const;   // 0 al explotar, 1 al terminar
};

#endif
