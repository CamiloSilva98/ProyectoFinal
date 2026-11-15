#include "nubehumo.h"

NubeHumo::NubeHumo(float x, float y, float radio, float duracion)
    : entidad(x, y, 1, radio), tiempoVida(duracion)
{
}

void NubeHumo::actualizar(float dt)
{
    tiempoVida -= dt;
    if (tiempoVida <= 0.0f) salud = 0;
}

float NubeHumo::getTiempoVida() const { return tiempoVida; }