#include "policia.h"
#include "nivel.h"
#include "proyectil.h"
#include <cmath>

Policia::Policia(float x, float y, int salud, float velocidad,
                 entidad *objetivo, Nivel *nivel, ModeloFisico *modeloBomba)
    : Enemigo(x, y, salud, velocidad), objetivo(objetivo), nivel(nivel),
    modeloBomba(modeloBomba), rangoAtaque(600.0f), intervaloAtaque(2.0f),
    tiempoAtaque(0.0f), velocidadBombaX(250.0f), velocidadBombaY(300.0f), dañoBomba(15)
{
}

void Policia::actualizar(float dt)
{
    if (!estaViva()) return;

    tiempoAtaque += dt;
    Enemigo::actualizar(dt);   // se mueve

    if (objetivo != nullptr && std::fabs(objetivo->getX() - x) <= rangoAtaque)
        atacar(*objetivo);
}

// se acerca a rambo y se detiene a la mitad del rango de ataque
void Policia::mover(float dt)
{
    if (objetivo == nullptr) return;

    float dx = objetivo->getX() - x;
    if (std::fabs(dx) <= rangoAtaque * 0.5f) return;

    x += (dx > 0.0f ? 1.0f : -1.0f) * velocidad * dt;
}

// lanza una bomba en parabola hacia donde esta el objetivo
void Policia::atacar(entidad &blanco)
{
    if (nivel == nullptr || modeloBomba == nullptr) return;
    if (tiempoAtaque < intervaloAtaque) return;

    float dir = (blanco.getX() >= x) ? 1.0f : -1.0f;
    nivel->agregarEntidad(new Proyectil(x, y, dir * velocidadBombaX, -velocidadBombaY,
                                        dañoBomba, modeloBomba));
    tiempoAtaque = 0.0f;
}

void Policia::setParametrosAtaque(float rango, float intervalo, float velX, float velY, int dano)
{
    rangoAtaque = rango;
    intervaloAtaque = intervalo;
    velocidadBombaX = velX;
    velocidadBombaY = velY;
    dañoBomba = dano;
}