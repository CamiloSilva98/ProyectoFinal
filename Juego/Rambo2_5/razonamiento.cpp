#include "razonamiento.h"

static const float UMBRAL_INTERES = 0.2f;
static const float PENALIZACION_FALSA_ALARMA = 0.25f;

int Razonamiento::evaluarEstimulos(const std::vector<Estimulo> &estimulos, const Aprendizaje &aprendizaje) const
{
    int mejor = -1;
    float mejorPuntaje = UMBRAL_INTERES;

    for (std::size_t i = 0; i < estimulos.size(); ++i)
    {
        const Estimulo &e = estimulos[i];
        float puntaje = e.intensidad;

        if (e.tipo == TipoEstimulo::Visual) puntaje = 100.0f;
        else if (e.tipo == TipoEstimulo::Cadaver) puntaje = 10.0f;
        else puntaje -= PENALIZACION_FALSA_ALARMA * aprendizaje.consultarExperiencias(e.x, e.y);

        if (puntaje > mejorPuntaje)
        {
            mejorPuntaje = puntaje;
            mejor = static_cast<int>(i);
        }
    }
    return mejor;
}

Decision Razonamiento::decidirAccion(const std::vector<Estimulo> &estimulos, const Aprendizaje &aprendizaje,
                                     const Guardia &guardia) const
{
    Decision decision = {TipoDecision::Continuar, 0.0f, 0.0f};

    int indice = evaluarEstimulos(estimulos, aprendizaje);
    if (indice < 0) return decision;

    const Estimulo &e = estimulos[indice];
    decision.destinoX = e.x;
    decision.destinoY = e.y;

    if (e.tipo == TipoEstimulo::Visual)
    {
        decision.tipo = TipoDecision::Perseguir;
    }
    else if (guardia.getEstado() != EstadoGuardia::Persiguiendo)
    {
        decision.tipo = (e.tipo == TipoEstimulo::Cadaver) ? TipoDecision::RevisarCadaver
                                                          : TipoDecision::Investigar;
    }
    return decision;
}
