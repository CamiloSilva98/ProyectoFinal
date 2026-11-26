#include "policia.h"
#include "nivel.h"
#include "proyectil.h"
#include <cmath>
#include "granada.h"

static const float GRAVEDAD_CAIDA = 1200.0f;
static const float TOLERANCIA_NIVEL = 60.0f;

Policia::Policia(float x, float y, int salud, float velocidad,
                 entidad *objetivo, Nivel *nivel, ModeloFisico *modeloBomba)
    : Enemigo(x, y, salud, velocidad), objetivo(objetivo), nivel(nivel),
    modeloBomba(modeloBomba), rangoAtaque(600.0f), intervaloAtaque(2.0f),
    tiempoAtaque(0.0f), velocidadBombaX(250.0f), velocidadBombaY(300.0f), danoBomba(15),
    tipo(TipoPolicia::Granadero), modeloBala(nullptr), reboteGranada(nullptr),
    limiteIzq(0.0f), limiteDer(100000.0f), gravedadGranada(500.0f), velocidadCaida(0.0f), caminando(false)
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
    caminando = false;
    if (objetivo == nullptr) return;
    if (velocidadCaida > 0.0f) return;   // mientras cae no camina

    float dx = objetivo->getX() - x;
    float dir = (dx > 0.0f) ? 1.0f : -1.0f;
    float dy = objetivo->getY() - y;   // positivo si rambo esta mas abajo

    if (tipo == TipoPolicia::Fusilero)
    {
        if (dy > TOLERANCIA_NIVEL)
        {
            // rambo esta abajo: camina hacia el borde para caer, sin limites
            float d = dir;
            if (std::fabs(dx) < 20.0f) d = (x - limiteIzq < limiteDer - x) ? -1.0f : 1.0f;
            caminando = true;
            x += d * velocidad * dt;
            return;
        }
        if (dy < -TOLERANCIA_NIVEL)
        {
            // rambo esta arriba: se pone debajo de el y espera
            if (std::fabs(dx) > 20.0f)
            {
                caminando = true;
                x += dir * velocidad * dt;
                if (x < limiteIzq) x = limiteIzq;
                if (x > limiteDer) x = limiteDer;
            }
            return;
        }
    }

    if (std::fabs(dx) <= rangoAtaque * 0.5f) return;
    caminando = true;
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
        // el fusilero solo dispara al frente, en horizontal
        // en el fusilero velocidadBombaX es la rapidez de la bala
        if (std::fabs(blanco.getY() - y) > TOLERANCIA_NIVEL) return;   // solo si lo tiene enfrente
        nivel->agregarEntidad(new Proyectil(x + dir * 45.0f, y - 75.0f,
                                            dir * velocidadBombaX, 0.0f, danoBomba, modeloBala));
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

float Policia::getTiempoAtaque() const { return tiempoAtaque; }
bool Policia::estaCaminando() const { return caminando; }