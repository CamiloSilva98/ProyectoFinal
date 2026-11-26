#include "guardia.h"
#include "nivel1.h"
#include <cmath>

static const float RADIO_GUARDIA = 12.0f;
static const float FACTOR_PERSECUCION = 2.3f;
static const int DANO_DISPARO = 50;
static const float TIEMPO_RECARGA = 3.0f;
static const float TIEMPO_PERDER_RASTRO = 3.0f;
static const float ESPERA_INVESTIGANDO = 2.0f;
static const float DURACION_ALARMA = 6.0f;
static const float K_AMPLITUD = 1.5f;
static const float K_FRECUENCIA = 2.0f;
// un punto pegado a un muro no se alcanza a menos del radio del cuerpo
static const float DISTANCIA_LLEGADA = 16.0f;


Guardia::Guardia(float x, float y, float finX, float finY, float velocidad,
                 float alcance, float apertura, float amplitud, float frecuencia,
                 Nivel1 *nivel)
    : Enemigo(x, y, 10, velocidad), estado(EstadoGuardia::Patrullando),
    inicioX(x), inicioY(y), finX(finX), finY(finY), haciaFin(true),
    destinoX(x), destinoY(y), tiempoEspera(0.0f), tiempoSinVer(0.0f),
    thetaCentro(0.0f), oscilador(amplitud, frecuencia), amplitudBase(amplitud),
    frecuenciaBase(frecuencia), tiempoAlarma(0.0f), alcance(alcance), apertura(apertura),
    deteccion(0.0f), investigacionTerminada(false), encontroRambo(false),
    investigadoX(0.0f), investigadoY(0.0f), estatico(false), thetaPuesto(0.0f),
    recarga(0.0f), nivel(nivel)
{
    radio = RADIO_GUARDIA;
}

void Guardia::actualizar(float dt)
{
    if (!estaViva()) return;

    float a = 0.0f, b = 0.0f, c = 0.0f, d = 0.0f;
    oscilador.calcularMovimiento(a, b, c, d, dt);

    if (recarga > 0.0f) recarga -= dt;

    if (tiempoAlarma > 0.0f)
    {
        tiempoAlarma -= dt;
        if (tiempoAlarma <= 0.0f)
        {
            oscilador.setAmplitud(amplitudBase);
            oscilador.setFrecuencia(frecuenciaBase);
        }
    }

    Enemigo::actualizar(dt);
}

void Guardia::mover(float dt)
{
    float metaX = destinoX;
    float metaY = destinoY;
    float rapidez = velocidad;

    if (estado == EstadoGuardia::Patrullando)
    {
        metaX = haciaFin ? finX : inicioX;
        metaY = haciaFin ? finY : inicioY;
    }
    else if (estado == EstadoGuardia::Persiguiendo)
    {
        rapidez = velocidad * FACTOR_PERSECUCION;
        tiempoSinVer += dt;
        if (tiempoSinVer > TIEMPO_PERDER_RASTRO)
        {
            estado = EstadoGuardia::Patrullando;
            return;
        }
    }

    float dx = metaX - x;
    float dy = metaY - y;
    float distancia = std::sqrt(dx * dx + dy * dy);

    if (distancia > DISTANCIA_LLEGADA)
    {
        float rumboX = metaX;
        float rumboY = metaY;
        if (nivel != nullptr) nivel->siguientePunto(x, y, metaX, metaY, rumboX, rumboY);

        float rumboDx = rumboX - x;
        float rumboDy = rumboY - y;
        float largo = std::sqrt(rumboDx * rumboDx + rumboDy * rumboDy);
        thetaCentro = std::atan2(rumboDy, rumboDx);
        float paso = rapidez * dt;
        if (paso > largo) paso = largo;
        x += rumboDx / largo * paso;
        y += rumboDy / largo * paso;
        return;
    }

    if (estado == EstadoGuardia::Patrullando)
    {
        haciaFin = !haciaFin;
        if (estatico) thetaCentro = thetaPuesto;
    }
    else if (estado == EstadoGuardia::Investigando)
    {
        tiempoEspera += dt;
        if (tiempoEspera >= ESPERA_INVESTIGANDO)
        {
            estado = EstadoGuardia::Patrullando;
            investigacionTerminada = true;
            encontroRambo = false;
            investigadoX = destinoX;
            investigadoY = destinoY;
        }
    }
}

void Guardia::atacar(entidad &blanco)
{
    blanco.recibirDano(DANO_DISPARO);
    recarga = TIEMPO_RECARGA;
}

void Guardia::investigar(float x, float y)
{
    if (estado == EstadoGuardia::Persiguiendo) return;
    estado = EstadoGuardia::Investigando;
    destinoX = x;
    destinoY = y;
    tiempoEspera = 0.0f;
}

void Guardia::perseguir(float x, float y)
{
    if (estado == EstadoGuardia::Investigando)
    {
        investigacionTerminada = true;
        encontroRambo = true;
        investigadoX = destinoX;
        investigadoY = destinoY;
    }
    estado = EstadoGuardia::Persiguiendo;
    destinoX = x;
    destinoY = y;
    tiempoSinVer = 0.0f;
}

// al cambiar A y omega la fase sigue acumulada
void Guardia::alarmar()
{
    oscilador.setAmplitud(amplitudBase * K_AMPLITUD);
    oscilador.setFrecuencia(frecuenciaBase * K_FRECUENCIA);
    tiempoAlarma = DURACION_ALARMA;
}

void Guardia::fijarPuesto(float theta)
{
    estatico = true;
    thetaCentro = theta;
    thetaPuesto = theta;
}

bool Guardia::puedeDisparar() const { return recarga <= 0.0f; }

void Guardia::subirDeteccion(float cantidad)
{
    deteccion += cantidad;
    if (deteccion > 1.0f) deteccion = 1.0f;
}

void Guardia::bajarDeteccion(float cantidad)
{
    deteccion -= cantidad;
    if (deteccion < 0.0f) deteccion = 0.0f;
}

bool Guardia::consumirInvestigacion(float &x, float &y, bool &encontro)
{
    if (!investigacionTerminada) return false;
    investigacionTerminada = false;
    x = investigadoX;
    y = investigadoY;
    encontro = encontroRambo;
    return true;
}

EstadoGuardia Guardia::getEstado() const { return estado; }
float Guardia::getAnguloVision() const { return oscilador.calcularAngulo(thetaCentro); }
float Guardia::getAlcance() const { return alcance; }
float Guardia::getApertura() const { return apertura; }
float Guardia::getDeteccion() const { return deteccion; }
bool Guardia::estaAlarmado() const { return tiempoAlarma > 0.0f; }
float Guardia::getAmplitud() const { return oscilador.getAmplitud(); }
float Guardia::getFrecuencia() const { return oscilador.getFrecuencia(); }