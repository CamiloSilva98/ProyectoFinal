#include "accionguardia.h"

void AccionGuardia::ejecutarDecision(const Decision &decision, Guardia &guardia)
{
    switch (decision.tipo)
    {
    case TipoDecision::Perseguir:
        guardia.perseguir(decision.destinoX, decision.destinoY);
        break;
    case TipoDecision::Investigar:
        guardia.investigar(decision.destinoX, decision.destinoY);
        break;
    case TipoDecision::RevisarCadaver:
        guardia.alarmar();
        guardia.investigar(decision.destinoX, decision.destinoY);
        break;
    case TipoDecision::Continuar:
        break;
    }
}
