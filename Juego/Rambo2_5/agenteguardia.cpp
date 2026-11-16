#include "agenteguardia.h"
#include "nivel1.h"

static const float VELOCIDAD_DETECCION = 0.7f;
static const float VELOCIDAD_OLVIDO = 0.3f;
static const float FACTOR_ACECHADOR = 0.4f;

AgenteGuardia::AgenteGuardia(Guardia *guardia, Nivel1 *nivel, std::size_t memoria)
    : guardia(guardia), nivel(nivel), percepcion(nivel), aprendizaje(memoria)
{
}

void AgenteGuardia::actualizar(float dt)
{
    if (!guardia->estaViva()) return;

    std::vector<Estimulo> estimulos = percepcion.detectarEstimulos(*guardia);

    bool veARambo = false;
    for (const Estimulo &e : estimulos)
        if (e.tipo == TipoEstimulo::Visual) veARambo = true;

    if (veARambo)
    {
        float velocidad = VELOCIDAD_DETECCION;
        if (nivel->getRambo()->estaAcechando()) velocidad *= FACTOR_ACECHADOR;
        guardia->subirDeteccion(velocidad * dt);
    }
    else
    {
        guardia->bajarDeteccion(VELOCIDAD_OLVIDO * dt);
    }

    Decision decision = razonamiento.decidirAccion(estimulos, aprendizaje, *guardia);
    accion.ejecutarDecision(decision, *guardia);

    float x = 0.0f, y = 0.0f;
    bool encontro = false;
    if (guardia->consumirInvestigacion(x, y, encontro))
        aprendizaje.registrarResultado(x, y, encontro);

    if (guardia->getDeteccion() >= 1.0f)
        guardia->atacar(*nivel->getRambo());
}

Guardia *AgenteGuardia::getGuardia() const { return guardia; }
const Aprendizaje &AgenteGuardia::getAprendizaje() const { return aprendizaje; }
