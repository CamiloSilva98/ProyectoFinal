#include "policia.h"
#include "nivel.h"
#include "proyectil.h"
#include <cmath>
#include "granada.h"

static const float GRAVEDAD_CAIDA = 1200.0f;

Policia::Policia(float x, float y, int salud, float velocidad,
                 entidad *objetivo, Nivel *nivel, ModeloFisico *modeloBomba)
    : Enemigo(x, y, salud, velocidad), objetivo(objetivo), nivel(nivel),
    modeloBomba(modeloBomba), rangoAtaque(600.0f), intervaloAtaque(2.0f),
    tiempoAtaque(0.0f), velocidadBombaX(250.0f), velocidadBombaY(300.0f), danoBomba(15),
    tipo(TipoPolicia::Granadero), modeloBala(nullptr), reboteGranada(nullptr),
    limiteIzq(0.0f), limiteDer(100000.0f), gravedadGranada(500.0f), velocidadCaida(0.0f)
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

// se acerca a rambo, el fusilero ademas baja de su base si rambo esta mas abajo
void Policia::mover(float dt)
{
    if (objetivo == nullptr) return;
    if (velocidadCaida > 0.0f) return;   // mientras cae no camina

    float dx = objetivo->getX() - x;
    float dir = (dx > 0.0f) ? 1.0f : -1.0f;

    // rambo esta en una base mas baja y fuera del ancho de esta base
    bool baja = (tipo == TipoPolicia::Fusilero && objetivo->getY() - y > 60.0f
                 && (objetivo->getX() < limiteIzq || objetivo->getX() > limiteDer));
    if (baja)
    {
        x += dir * velocidad * dt;   // sin limites, para salirse de la base y caer
        return;
    }

    if (std::fabs(dx) <= rangoAtaque * 0.5f) return;
    x += dir * velocidad * dt;
    if (x < limiteIzq) x = limiteIzq;
    if (x > limiteDer) x = limiteDer;
}

// el granadero lanza una granada en parabola, el fusilero dispara recto hacia rambo
void Policia::atacar(entidad &blanco)
{
    if (nivel == nullptr || modeloBomba == nullptr) return;
    if (tiempoAtaque < intervaloAtaque) return;

    float dir = (blanco.getX() >= x) ? 1.0f : -1.0f;

    if (tipo == TipoPolicia::Granadero)
    {
        // calcula el angulo para que la granada caiga donde esta rambo, aunque este en una base mas alta
        float origenX = x + dir * 30.0f;
        float origenY = y - 100.0f;
        float d = std::fabs(blanco.getX() - origenX);
        float dy = blanco.getY() - origenY;   // positivo si rambo esta mas abajo
        float v = std::sqrt(velocidadBombaX * velocidadBombaX + velocidadBombaY * velocidadBombaY);

        float u = 1.0f;   // tangente del angulo, 1 es 45 grados: si no llega, lanza igual
        if (d > 1.0f && gravedadGranada > 0.0f)
        {
            float a = gravedadGranada * d * d / (2.0f * v * v);
            float disc = d * d - 4.0f * a * (a - dy);
            if (disc >= 0.0f)
            {
                float raiz = std::sqrt(disc);
                // arco alto si rambo esta mas arriba, arco bajo en los demas casos
                u = (dy < -20.0f) ? (d + raiz) / (2.0f * a) : (d - raiz) / (2.0f * a);
            }
        }
        float c = 1.0f / std::sqrt(1.0f + u * u);

        nivel->agregarEntidad(new Granada(origenX, origenY, dir * v * c, -v * u * c,
                                          danoBomba, 120.0f, modeloBomba, reboteGranada));
    }
    else if (modeloBala != nullptr)
    {
        // apunta al centro del cuerpo de rambo, aunque este en otra base
        float origenX = x + dir * 45.0f;
        float origenY = y - 75.0f;
        float dx = blanco.getX() - origenX;
        float dy = (blanco.getY() - 63.0f) - origenY;
        float d = std::sqrt(dx * dx + dy * dy);
        if (d < 1.0f) d = 1.0f;

        // en el fusilero velocidadBombaX es la rapidez de la bala
        nivel->agregarEntidad(new Proyectil(origenX, origenY, velocidadBombaX * dx / d,
                                            velocidadBombaX * dy / d, danoBomba, modeloBala));
    }
    tiempoAtaque = 0.0f;
}

void Policia::setParametrosAtaque(float rango, float intervalo, float velX, float velY, int dano)
{
    rangoAtaque = rango;
    intervaloAtaque = intervalo;
    velocidadBombaX = velX;
    velocidadBombaY = velY;
    danoBomba = dano;
}
void Policia::setTipo(TipoPolicia nuevoTipo) { tipo = nuevoTipo; }
TipoPolicia Policia::getTipo() const { return tipo; }

void Policia::setModelos(ModeloFisico *bala, ModeloRebote *rebote)
{
    modeloBala = bala;
    reboteGranada = rebote;
}

void Policia::setLimites(float izq, float der)
{
    limiteIzq = izq;
    limiteDer = der;
}
// cae por gravedad hasta la superficie que tiene debajo
void Policia::caer(float dt, float superficie)
{
    if (y >= superficie - 0.5f)
    {
        y = superficie;
        velocidadCaida = 0.0f;
        return;
    }
    velocidadCaida += GRAVEDAD_CAIDA * dt;
    y += velocidadCaida * dt;
    if (y >= superficie)
    {
        y = superficie;
        velocidadCaida = 0.0f;
    }
}

void Policia::setGravedadGranada(float g) { gravedadGranada = g; }