#include "ondaruido.h"
#include <algorithm>

OndaRuido::OndaRuido(float x, float y, float velocidadSonido, float radioMax, float intensidadInicial)
    : entidad(x, y, 1), modelo(velocidadSonido, radioMax, intensidadInicial)
{
}

void OndaRuido::actualizar(float dt)
{
    float vx = 0.0f, vy = 0.0f;
    modelo.calcularMovimiento(x, y, vx, vy, dt);
    radio = modelo.calcularRadio();
    if (modelo.haTerminado()) salud = 0;
}

bool OndaRuido::escuchadaPor(const void *oyente, float distancia, float intensidadMin, float &intensidad)
{
    if (std::find(oyentes.begin(), oyentes.end(), oyente) != oyentes.end()) return false;
    if (!modelo.haAlcanzado(distancia)) return false;

    intensidad = modelo.calcularIntensidad(distancia);
    if (intensidad < intensidadMin) return false;

    oyentes.push_back(oyente);
    return true;
}
