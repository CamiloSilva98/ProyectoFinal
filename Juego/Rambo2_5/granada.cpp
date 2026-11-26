#include "granada.h"
#include <cstdlib>

static const float DURACION_EXPLOSION = 0.35f;

Granada::Granada(float x, float y, float vx, float vy, int danoExplosion, float radioExplosion,
                 ModeloFisico *modelo, ModeloRebote *rebote)
    : Proyectil(x, y, vx, vy, danoExplosion, modelo, rebote, 1),
    golpesParaExplotar(1 + std::rand() % 2), golpesSuelo(0), explotada(false),
    explosionPendiente(false), tiempoExplosion(DURACION_EXPLOSION), radioExplosion(radioExplosion)
{
}

void Granada::actualizar(float dt)
{
    // ya explotada no se mueve, solo espera a que termine la explosion
    if (explotada)
    {
        tiempoExplosion -= dt;
        if (tiempoExplosion <= 0.0f) salud = 0;
        return;
    }
    Proyectil::actualizar(dt);
}

void Granada::golpearSuelo()
{
    if (explotada) return;
    golpesSuelo++;
    if (golpesSuelo >= golpesParaExplotar)
    {
        explotada = true;
        explosionPendiente = true;
    }
    else
    {
        colisionar(0.0f, -1.0f);   // el primer golpe rebota
    }
}

bool Granada::estaExplotando() const { return explotada; }

bool Granada::consumirExplosion()
{
    bool hay = explosionPendiente;
    explosionPendiente = false;
    return hay;
}

float Granada::getRadioExplosion() const { return radioExplosion; }

float Granada::getProgresoExplosion() const
{
    return 1.0f - tiempoExplosion / DURACION_EXPLOSION;
}