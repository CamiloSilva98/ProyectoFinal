#include "aprendizaje.h"
#include <cmath>

static const float RADIO_PUNTO = 50.0f;

Aprendizaje::Aprendizaje(std::size_t capacidad)
    : capacidad(capacidad)
{
}

int Aprendizaje::buscarPunto(float x, float y) const
{
    for (std::size_t i = 0; i < memoria.size(); ++i)
    {
        float dx = memoria[i].x - x;
        float dy = memoria[i].y - y;
        if (std::sqrt(dx * dx + dy * dy) < RADIO_PUNTO) return static_cast<int>(i);
    }
    return -1;
}

void Aprendizaje::registrarResultado(float x, float y, bool encontroRambo)
{
    int posicion = buscarPunto(x, y);

    if (encontroRambo)
    {
        if (posicion >= 0) memoria[posicion].falsasAlarmas = 0;
        return;
    }

    if (posicion >= 0)
    {
        memoria[posicion].falsasAlarmas++;
        return;
    }

    if (memoria.size() >= capacidad) memoria.erase(memoria.begin());
    memoria.push_back({x, y, 1});
}

int Aprendizaje::consultarExperiencias(float x, float y) const
{
    int posicion = buscarPunto(x, y);
    if (posicion < 0) return 0;
    return memoria[posicion].falsasAlarmas;
}
