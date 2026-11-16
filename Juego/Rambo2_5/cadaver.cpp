#include "cadaver.h"
#include <algorithm>

Cadaver::Cadaver(float x, float y)
    : entidad(x, y, 1, 10.0f)
{
}

void Cadaver::actualizar(float dt)
{
    (void)dt;
}

bool Cadaver::marcarVistoPor(const void *guardia)
{
    if (std::find(vistoPor.begin(), vistoPor.end(), guardia) != vistoPor.end()) return false;
    vistoPor.push_back(guardia);
    return true;
}
