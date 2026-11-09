#include "rambo.h"
#include "modeloparabolico.h"

Rambo::Rambo(float x, float y, int salud, int municion, float velocidad)
    : entidad(x, y, salud), municion(municion), velocidad(velocidad), fatiga(0.0f),
    velocidadY(0.0f), ySuelo(y), velocidadSalto(300.0f), enSuelo(true), modeloSalto(nullptr),
    fatigaMax(100.0f), tasaFatiga(1.0f), costoDisparo(2.0f),
    intervaloBase(0.3f), tiempoDisparo(10.0f), factorFatiga(0.5f){}

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
}

void Rambo::mover(float dirX, float dirY, float dt)
{
    float v = getVelocidadEfectiva();
    x += dirX * v * dt;
    y += dirY * v * dt;
}

// devuelve true si pudo disparar, gasta una municion
// devuelve true si pudo disparar: tiene municion y ya paso el intervalo
bool Rambo::disparar()
{
    if (municion <= 0) return false;
    if (tiempoDisparo < getIntervaloDisparo()) return false;

    municion--;
    tiempoDisparo = 0.0f;
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
    return velocidad * (1.0f - factorFatiga * fatiga / fatigaMax);
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

