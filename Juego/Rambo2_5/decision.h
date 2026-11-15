#ifndef DECISION_H
#define DECISION_H

enum class TipoDecision { Continuar, Investigar, Perseguir, RevisarCadaver };

struct Decision
{
    TipoDecision tipo;
    float destinoX;
    float destinoY;
};

#endif