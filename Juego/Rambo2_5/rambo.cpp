#include "rambo.h"
#include "modeloparabolico.h"

static const float FACTOR_CORRER = 1.6f;
static const float FACTOR_ACECHADOR = 1.25f;
static const float TIEMPO_PARA_ACECHAR = 3.0f;
static const float PERIODO_RUIDO = 0.4f;


Rambo::Rambo(float x, float y, int salud, int municion, float velocidad, float radio)
    : entidad(x, y, salud, radio), municion(municion), velocidad(velocidad), fatiga(0.0f),
    velocidadY(0.0f), ySuelo(y), velocidadSalto(300.0f), enSuelo(true), modeloSalto(nullptr),
    fatigaMax(100.0f), tasaFatiga(1.0f), costoDisparo(2.0f),
    intervaloBase(0.3f), tiempoDisparo(10.0f), factorFatiga(0.5f),
    piedras(0), corriendo(false), acechador(false), enCobertura(false), movio(false),
    tiempoQuieto(0.0f), tiempoRuido(0.0f), ruidoPendiente(false){}


void Rambo::actualizar(float dt)
{
    // la fatiga sube con el tiempo de combate
    fatiga += tasaFatiga * dt;
    if (fatiga > fatigaMax) fatiga = fatigaMax;

    tiempoDisparo += dt;

    // salto con el modelo parabolico, el movimiento horizontal lo maneja mover()
    if (!enSuelo && modeloSalto != nullptr) {
        float vx = 0.0f;
        modeloSalto->calcularMovimiento(x, y, vx, velocidadY, dt);
        if (y >= ySuelo) {
            y = ySuelo;
            velocidadY = 0.0f;
            enSuelo = true;
        }
    }

    if (movio) tiempoQuieto = 0.0f;
    else tiempoQuieto += dt;

    if (acechador)
    {
        if (!enCobertura) acechador = false;
    }
    else if (enCobertura && tiempoQuieto >= TIEMPO_PARA_ACECHAR)
    {
        acechador = true;
    }

    if (corriendo && movio)
    {
        tiempoRuido += dt;
        if (tiempoRuido >= PERIODO_RUIDO)
        {
            ruidoPendiente = true;
            tiempoRuido = 0.0f;
        }
    }
    else
    {
        tiempoRuido = 0.0f;
    }
    movio = false;
}

void Rambo::mover(float dirX, float dirY, float dt)
{
    float v = getVelocidadEfectiva();
    x += dirX * v * dt;
    y += dirY * v * dt;
    if (dirX != 0.0f || dirY != 0.0f) movio = true;
}


// devuelve true si pudo disparar, gasta una municion
// devuelve true si pudo disparar: tiene municion y ya paso el intervalo
bool Rambo::disparar()
{
    if (municion <= 0) return false;
    if (tiempoDisparo < getIntervaloDisparo()) return false;

    municion--;
    tiempoDisparo = 0.0f;
    acechador = false;
    tiempoQuieto = 0.0f;
    fatiga += costoDisparo;
    if (fatiga > fatigaMax) fatiga = fatigaMax;
    return true;
}

void Rambo::agregarMunicion(int cantidad)
{
    municion += cantidad;
}

void Rambo::reducirFatiga(float cantidad)
{
    fatiga -= cantidad;
    if (fatiga < 0.0f) fatiga = 0.0f;
}

// solo salta si esta en el suelo y tiene modelo asignado
void Rambo::saltar()
{
    if (!enSuelo || modeloSalto == nullptr) return;
    velocidadY = -velocidadSalto;   // el eje vertical es positivo hacia abajo
    enSuelo = false;
}

void Rambo::setModeloSalto(ModeloParabolico *modelo) { modeloSalto = modelo; }
void Rambo::setSuelo(float yPiso) { ySuelo = yPiso; }
bool Rambo::estaEnSuelo() const { return enSuelo; }

// la fatiga baja la velocidad hasta un factorFatiga de su valor
float Rambo::getVelocidadEfectiva() const
{
    float v = velocidad * (1.0f - factorFatiga * fatiga / fatigaMax);
    if (corriendo) v *= FACTOR_CORRER;
    if (acechador) v *= FACTOR_ACECHADOR;
    return v;
}

// la fatiga alarga el intervalo entre disparos
float Rambo::getIntervaloDisparo() const
{
    return intervaloBase * (1.0f + factorFatiga * fatiga / fatigaMax);
}

void Rambo::setParametrosFatiga(float maxima, float tasa, float costo, float factor)
{
    fatigaMax = maxima;
    tasaFatiga = tasa;
    costoDisparo = costo;
    factorFatiga = factor;
}

float Rambo::getFatiga() const { return fatiga; }
int Rambo::getMunicion() const { return municion; }
float Rambo::getVelocidad() const { return velocidad; }
void Rambo::setVelocidad(float v) { velocidad = v; }

// devuelve true si el suministro todavia estaba disponible y se aplico
bool Rambo::recogerSuministro(Suministro &suministro)
{
    if (!suministro.estaViva()) return false;
    suministro.aplicarA(*this);
    return true;
}

void Rambo::setCorriendo(bool valor) { corriendo = valor; }
bool Rambo::estaCorriendo() const { return corriendo; }
void Rambo::setEnCobertura(bool valor) { enCobertura = valor; }
bool Rambo::estaAcechando() const { return acechador; }

bool Rambo::consumirRuido()
{
    bool hayRuido = ruidoPendiente;
    ruidoPendiente = false;
    return hayRuido;
}

bool Rambo::lanzarPiedra()
{
    if (piedras <= 0) return false;
    piedras--;
    return true;
}

void Rambo::agregarPiedras(int cantidad) { piedras += cantidad; }
int Rambo::getPiedras() const { return piedras; }

