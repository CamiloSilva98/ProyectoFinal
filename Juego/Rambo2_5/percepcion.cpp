#include "percepcion.h"
#include "nivel1.h"
#include "cadaver.h"
#include "ondaruido.h"
#include <cmath>

static const float PI = 3.14159265f;
static const float FACTOR_ALCANCE_EN_HUMO = 0.4f;
static const float INTENSIDAD_MINIMA = 0.1f;

Percepcion::Percepcion(Nivel1 *nivel)
    : nivel(nivel)
{
}

bool Percepcion::puedeVer(const Guardia &guardia, float px, float py) const
{
    float alcance = guardia.getAlcance();
    if (nivel->estaEnHumo(guardia.getX(), guardia.getY())) alcance *= FACTOR_ALCANCE_EN_HUMO;

    float dx = px - guardia.getX();
    float dy = py - guardia.getY();
    if (dx * dx + dy * dy > alcance * alcance) return false;

    float diferencia = std::atan2(dy, dx) - guardia.getAnguloVision();
    while (diferencia > PI) diferencia -= 2.0f * PI;
    while (diferencia < -PI) diferencia += 2.0f * PI;
    if (std::fabs(diferencia) > guardia.getApertura() / 2.0f) return false;

    return nivel->hayLineaDeVista(guardia.getX(), guardia.getY(), px, py);
}

std::vector<Estimulo> Percepcion::detectarEstimulos(const Guardia &guardia)
{
    std::vector<Estimulo> estimulos;

    Rambo *rambo = nivel->getRambo();
    if (rambo->estaViva() && !nivel->estaEnHumo(rambo->getX(), rambo->getY())
        && puedeVer(guardia, rambo->getX(), rambo->getY()))
    {
        estimulos.push_back({TipoEstimulo::Visual, rambo->getX(), rambo->getY(), 1.0f});
    }

    for (Cadaver *cadaver : nivel->getCadaveres())
    {
        if (puedeVer(guardia, cadaver->getX(), cadaver->getY()) && cadaver->marcarVistoPor(&guardia))
            estimulos.push_back({TipoEstimulo::Cadaver, cadaver->getX(), cadaver->getY(), 1.0f});
    }

    for (OndaRuido *onda : nivel->getOndas())
    {
        float dx = onda->getX() - guardia.getX();
        float dy = onda->getY() - guardia.getY();
        float intensidad = 0.0f;
        if (onda->escuchadaPor(&guardia, std::sqrt(dx * dx + dy * dy), INTENSIDAD_MINIMA, intensidad))
            estimulos.push_back({TipoEstimulo::Ruido, onda->getX(), onda->getY(), intensidad});
    }

    return estimulos;
}
