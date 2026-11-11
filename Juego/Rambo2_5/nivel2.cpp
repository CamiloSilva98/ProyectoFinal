#include "nivel2.h"
#include "suministro.h"
#include "proyectil.h"
#include <cstdlib>
#include <cmath>

static const float RADIO_IMPACTO = 40.0f;

// dos entidades estan cerca si la distancia entre centros es menor al radio de impacto
static bool cerca(const entidad &a, const entidad &b)
{
    float dx = a.getX() - b.getX();
    float dy = a.getY() - b.getY();
    return dx * dx + dy * dy <= RADIO_IMPACTO * RADIO_IMPACTO;
}

Nivel2::Nivel2(float ancho, float alto, float ySuelo)
    : Nivel(0.0f), anchoNivel(ancho), altoNivel(alto), ySuelo(ySuelo),
    rambo(nullptr), arma(nullptr),
    modeloSalto(600.0f), modeloBala(150.0f), modeloBomba(500.0f),
    reboteBala(0.5f), oscilador(0.6f, 1.5f),
    numeroHorda(1), tiempoHorda(0.0f), tiempoSpawn(0.0f),
    generando(false), ladoSpawn(false),
    intervaloArma(2.0f), tiempoArma(0.0f), probSuministro(35),
    victoria(false), derrota(false)
{
    // duracion, intervalo de aparicion, maximo en pantalla, salud, velocidad, intervalo de ataque
    hordas.push_back({20.0f, 2.5f, 3, 30, 80.0f, 2.5f});
    hordas.push_back({25.0f, 2.0f, 4, 40, 100.0f, 2.0f});
    hordas.push_back({30.0f, 1.5f, 6, 50, 120.0f, 1.5f});
}

void Nivel2::iniciar()
{
    rambo = new Rambo(anchoNivel * 0.1f, ySuelo, 100, 30, 200.0f);
    rambo->setSuelo(ySuelo);
    rambo->setModeloSalto(&modeloSalto);
    agregarEntidad(rambo);

    // el arma cuelga arriba al centro apuntando hacia abajo (pi/2)
    arma = new ArmaOscilante(anchoNivel * 0.5f, 60.0f, 1.5708f, 300.0f, 10, &oscilador, &modeloBomba);
    agregarEntidad(arma);

    numeroHorda = 1;
    victoria = false;
    derrota = false;
    iniciarHorda();
}

void Nivel2::actualizar(float dt)
{
    if (rambo == nullptr || victoria || derrota) return;

    Nivel::actualizar(dt);

    // el arma fija dispara cada cierto tiempo
    tiempoArma += dt;
    if (tiempoArma >= intervaloArma)
    {
        agregarEntidad(arma->disparar());
        tiempoArma = 0.0f;
    }

    actualizarHorda(dt);
    resolverColisiones();

    // rambo no se borra de memoria hasta destruir el nivel, por eso es seguro preguntar
    if (!rambo->estaViva())
    {
        derrota = true;
        return;
    }

    if (comprobarHordaTerminada())
    {
        if (numeroHorda < static_cast<int>(hordas.size()))
        {
            numeroHorda++;
            iniciarHorda();
        }
        else
        {
            victoria = true;
        }
    }
}

bool Nivel2::comprobarVictoria() const { return victoria; }
bool Nivel2::comprobarDerrota() const { return derrota; }

void Nivel2::iniciarHorda()
{
    const ConfigHorda &cfg = hordas[numeroHorda - 1];
    tiempoHorda = cfg.duracion;
    tiempoSpawn = cfg.intervaloSpawn;   // el primer policia aparece enseguida
    generando = true;
}

// mientras dura la horda aparecen policias, al acabar el tiempo ya no salen mas
void Nivel2::actualizarHorda(float dt)
{
    if (!generando) return;

    const ConfigHorda &cfg = hordas[numeroHorda - 1];
    tiempoHorda -= dt;
    if (tiempoHorda <= 0.0f)
    {
        tiempoHorda = 0.0f;
        generando = false;
        return;
    }

    tiempoSpawn += dt;
    if (tiempoSpawn >= cfg.intervaloSpawn && contarPolicias() < cfg.maxEnPantalla)
    {
        generarPolicia();
        tiempoSpawn = 0.0f;
    }
}

// la horda termina cuando ya no aparecen mas y no queda ninguno vivo
bool Nivel2::comprobarHordaTerminada() const
{
    return !generando && contarPolicias() == 0;
}

int Nivel2::contarPolicias() const
{
    int n = 0;
    for (entidad *e : entidades)
        if (e->estaViva() && dynamic_cast<Policia*>(e) != nullptr) n++;
    return n;
}

void Nivel2::generarPolicia()
{
    const ConfigHorda &cfg = hordas[numeroHorda - 1];
    float px = ladoSpawn ? anchoNivel : 0.0f;
    ladoSpawn = !ladoSpawn;

    Policia *p = new Policia(px, ySuelo, cfg.saludPolicia, cfg.velocidadPolicia,
                             rambo, this, &modeloBomba);
    p->setParametrosAtaque(600.0f, cfg.intervaloAtaque, 250.0f, 300.0f, 15);
    agregarEntidad(p);
}

void Nivel2::soltarSuministro(float px, float py)
{
    if (std::rand() % 100 >= probSuministro) return;

    TipoSuministro tipo = (std::rand() % 3 == 0) ? TipoSuministro::Energia
                                                 : TipoSuministro::Municion;
    int cantidad = (tipo == TipoSuministro::Energia) ? 50 : 25;
    agregarEntidad(new Suministro(px, py, tipo, cantidad));
}

void Nivel2::resolverColisiones()
{
    // se usa el tamano de antes porque al matar un policia se puede agregar un suministro
    const std::size_t cantidad = entidades.size();
    for (std::size_t i = 0; i < cantidad; ++i)
    {
        entidad *e = entidades[i];
        if (!e->estaViva()) continue;

        Proyectil *p = dynamic_cast<Proyectil*>(e);
        if (p != nullptr)
        {
            if (p->esDeRambo())
            {
                // las balas de rambo dañan a los policias
                for (std::size_t j = 0; j < cantidad && p->estaViva(); ++j)
                {
                    Policia *pol = dynamic_cast<Policia*>(entidades[j]);
                    if (pol == nullptr || !pol->estaViva()) continue;
                    if (cerca(*p, *pol)) {
                        pol->recibirDano(p->getDaño());
                        p->recibirDano(1);
                        if (!pol->estaViva()) soltarSuministro(pol->getX(), pol->getY());
                    }
                }
            }
            else if (cerca(*p, *rambo))
            {
                // bombas y disparos del arma dañan a rambo
                rambo->recibirDano(p->getDaño());
                p->recibirDano(1);
            }

            // suelo y limites del mapa
            if (p->estaViva() && p->getY() >= ySuelo && p->getVelocidadY() > 0.0f)
                p->colisionar(0.0f, -1.0f);
            if (p->estaViva() && (p->getX() < -100.0f || p->getX() > anchoNivel + 100.0f
                                  || p->getY() < -300.0f))
                p->recibirDano(1);
            continue;
        }

        Suministro *s = dynamic_cast<Suministro*>(e);
        if (s != nullptr && cerca(*s, *rambo))
            rambo->recogerSuministro(*s);
    }
}

void Nivel2::moverRambo(float dir, float dt)
{
    if (rambo == nullptr || !rambo->estaViva()) return;

    rambo->mover(dir, 0.0f, dt);

    // si se sale del mapa se devuelve el mismo paso
    float nx = rambo->getX();
    if (nx < 0.0f || nx > anchoNivel)
        rambo->mover(-dir, 0.0f, dt);
}

void Nivel2::saltarRambo()
{
    if (rambo == nullptr || !rambo->estaViva()) return;
    rambo->saltar();
}

void Nivel2::dispararRambo(float dir)
{
    if (rambo == nullptr || !rambo->estaViva()) return;
    if (!rambo->disparar()) return;

    Proyectil *bala = new Proyectil(rambo->getX(), rambo->getY() - 30.0f, dir * 600.0f, 0.0f,
                                    10, &modeloBala, &reboteBala, 1);
    bala->setDeRambo(true);
    agregarEntidad(bala);
}

Rambo *Nivel2::getRambo() const { return rambo; }
int Nivel2::getNumeroHorda() const { return numeroHorda; }
float Nivel2::getAncho() const { return anchoNivel; }
float Nivel2::getAlto() const { return altoNivel; }
float Nivel2::getYSuelo() const { return ySuelo; }