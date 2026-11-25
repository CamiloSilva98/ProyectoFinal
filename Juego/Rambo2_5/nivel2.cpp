#include "nivel2.h"
#include "suministro.h"
#include "proyectil.h"
#include <cstdlib>
#include <cmath>


// tamano del cuerpo de los personajes en pantalla, igual al sprite de rambo escalado
static const float ANCHO_CUERPO = 45.0f;
static const float ALTO_CUERPO = 126.0f;
static const float RADIO_PROYECTIL = 6.0f;

// el cuerpo es un rectangulo con los pies en (x, y) y el proyectil un punto con un radio pequeno
static bool golpea(const entidad &proyectil, const entidad &cuerpo)
{
    float dx = std::fabs(proyectil.getX() - cuerpo.getX());
    float dy = proyectil.getY() - cuerpo.getY();
    return dx <= ANCHO_CUERPO / 2.0f + RADIO_PROYECTIL
           && dy >= -(ALTO_CUERPO + RADIO_PROYECTIL)
           && dy <= RADIO_PROYECTIL;
}

// la explosion alcanza al cuerpo si su centro queda dentro del radio
static bool alcanzaExplosion(const Granada &g, const entidad &cuerpo)
{
    float dx = g.getX() - cuerpo.getX();
    float dy = g.getY() - (cuerpo.getY() - ALTO_CUERPO / 2.0f);
    float r = g.getRadioExplosion();
    return dx * dx + dy * dy <= r * r;
}

Nivel2::Nivel2(float ancho, float alto, float ySuelo)
    : Nivel(0.0f), anchoNivel(ancho), altoNivel(alto), ySuelo(ySuelo),
    rambo(nullptr), arma(nullptr),
    modeloSalto(700.0f), modeloBala(0.0f), modeloBomba(500.0f),
    reboteBala(0.5f), reboteGranada(0.45f), oscilador(0.6f, 1.5f),
    numeroHorda(1), tiempoHorda(0.0f), tiempoSpawn(0.0f),
    generando(false),
    intervaloArma(2.0f), tiempoArma(0.0f), probSuministro(45),
    victoria(false), derrota(false)
{
    // duracion, intervalo de aparicion, maximo en pantalla, salud, velocidad, intervalo de ataque
    hordas.push_back({20.0f, 2.5f, 3, 30, 80.0f, 2.5f});//3,4,6
    hordas.push_back({25.0f, 2.0f, 4, 40, 100.0f, 2.0f});
    hordas.push_back({30.0f, 1.5f, 6, 50, 120.0f, 1.5f});
    // plataformas, medidas segun el fondo actual: balcones arriba y dos bases intermedias
    plataformas.push_back({0.0f, 478.0f, 175.0f});
    plataformas.push_back({1437.0f, 1916.0f, 175.0f});
    plataformas.push_back({172.0f, 660.0f, 375.0f});
    plataformas.push_back({1270.0f, 1745.0f, 375.0f});

    puntos.clear();
    puntos.push_back({   0.0f, 745.0f,    0.0f, 1916.0f});   // suelo, lado izquierdo
    puntos.push_back({1916.0f, 745.0f,    0.0f, 1916.0f});   // suelo, lado derecho
    puntos.push_back({  40.0f, 175.0f,    0.0f,  478.0f});   // balcon izquierdo
    puntos.push_back({1876.0f, 175.0f, 1437.0f, 1916.0f});   // balcon derecho
}

void Nivel2::iniciar()
{
    rambo = new Rambo(anchoNivel * 0.1f, ySuelo, 100, 30, 300.0f);
    rambo->setSuelo(ySuelo);
    rambo->setModeloSalto(&modeloSalto);
    rambo->setVelocidadSalto(640.0f);
    agregarEntidad(rambo);

    // el arma cuelga arriba al centro apuntando hacia abajo (pi/2)
    arma = new ArmaOscilante(anchoNivel * 0.5f, 60.0f, 1.5708f, 300.0f, 10, &oscilador, &modeloBomba);
    agregarEntidad(arma);
    protegerDeLimpieza(rambo);

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
    rambo->topeTecho(0, ALTO_CUERPO);
    resolverPlataformas(dt);
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
    const ConfigHorda &cfg = hordas.at(numeroHorda - 1);
    const PuntoAparicion &pt = puntos[std::rand() % puntos.size()];
    bool granadero = (std::rand() % 2 == 0);

    Policia *p = new Policia(pt.x, pt.y, cfg.saludPolicia, cfg.velocidadPolicia,
                             rambo, this, &modeloBomba);
    p->setLimites(pt.izq, pt.der);
    p->setModelos(&modeloBala, &reboteGranada);
    p->setGravedadGranada(modeloBomba.getGravedad());

    if (granadero)
    {
        p->setTipo(TipoPolicia::Granadero);
        p->setParametrosAtaque(600.0f, cfg.intervaloAtaque * 1.5f, 400.0f, 300.0f, 25);
    }
    else
    {
        p->setTipo(TipoPolicia::Fusilero);
        p->setParametrosAtaque(500.0f, cfg.intervaloAtaque * 0.5f, 500.0f, 0.0f, 8);
    }
    agregarEntidad(p);
}

void Nivel2::soltarSuministro(float px, float py)
{
    if (std::rand() % 100 >= probSuministro) return;

    TipoSuministro tipo = (std::rand() % 2 == 0) ? TipoSuministro::Energia
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
            Granada *g = dynamic_cast<Granada*>(e);
            if (g != nullptr)
            {
                // la granada solo hace dano al explotar, y explota al tocar el suelo o una base
                if (g->estaExplotando())
                {
                    if (g->consumirExplosion() && alcanzaExplosion(*g, *rambo))
                        rambo->recibirDano(g->getDano());
                }
                else if (g->getVelocidadY() > 0.0f && g->getY() >= superficieBajo(g->getX(), g->getY()))
                {
                    g->golpearSuelo();
                }
                continue;
            }

            if (p->esDeRambo())
            {
                // las balas de rambo danan a los policias
                for (std::size_t j = 0; j < cantidad && p->estaViva(); ++j)
                {
                    Policia *pol = dynamic_cast<Policia*>(entidades[j]);
                    if (pol == nullptr || !pol->estaViva()) continue;
                    if (golpea(*p, *pol))
                    {
                        pol->recibirDano(p->getDano());
                        p->recibirDano(1);
                        if (!pol->estaViva()) soltarSuministro(pol->getX(), pol->getY());
                    }
                }
            }
            else if (golpea(*p, *rambo))
            {
                // los disparos de los fusileros y del arma fija danan a rambo
                rambo->recibirDano(p->getDano());
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
        if (s != nullptr && golpea(*s, *rambo))
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

    Proyectil *bala = new Proyectil(rambo->getX() + dir * 45.0f, rambo->getY() - 75.0f, dir * 700.0f, 0.0f,
                                    10, &modeloBala, &reboteBala, 1);
    bala->setDeRambo(true);
    agregarEntidad(bala);
}

Rambo *Nivel2::getRambo() const { return rambo; }
int Nivel2::getNumeroHorda() const { return numeroHorda; }
float Nivel2::getAncho() const { return anchoNivel; }
float Nivel2::getAlto() const { return altoNivel; }
float Nivel2::getYSuelo() const { return ySuelo; }

// altura de la superficie mas alta que queda debajo (o a los pies) del punto, suelo incluido
float Nivel2::superficieBajo(float px, float py) const
{
    float superficie = ySuelo;
    for (const Plataforma &pl : plataformas)
    {
        if (px >= pl.x0 && px <= pl.x1 && pl.y >= py - 4.0f && pl.y < superficie)
            superficie = pl.y;
    }
    return superficie;
}

// rambo aterriza en las plataformas desde arriba y las atraviesa al subir,
// los policias caen por gravedad hasta la base que tengan debajo
void Nivel2::resolverPlataformas(float dt)
{
    float suelo = superficieBajo(rambo->getX(), rambo->getY());
    rambo->setSuelo(suelo);

    // si estaba parado y ya no hay apoyo debajo, empieza a caer
    if (rambo->estaEnSuelo() && rambo->getY() < suelo - 1.0f)
        rambo->soltarSuelo();

    for (entidad *e : entidades)
    {
        Policia *pol = dynamic_cast<Policia*>(e);
        if (pol == nullptr || !pol->estaViva()) continue;

        float superficie = superficieBajo(pol->getX(), pol->getY());
        pol->caer(dt, superficie);

        // al apoyarse en una base, sus limites pasan a ser los de esa base
        if (pol->getY() >= superficie - 0.5f)
        {
            float izq, der;
            limitesDeBase(pol->getX(), superficie, izq, der);
            pol->setLimites(izq, der);
        }
    }
}

// extremos de la base en la que esta parado un policia, el suelo ocupa todo el ancho
void Nivel2::limitesDeBase(float px, float superficie, float &izq, float &der) const
{
    izq = 0.0f;
    der = anchoNivel;
    for (const Plataforma &pl : plataformas)
    {
        if (px >= pl.x0 && px <= pl.x1 && std::fabs(pl.y - superficie) < 0.5f)
        {
            izq = pl.x0;
            der = pl.x1;
        }
    }
}
const std::vector<Plataforma> &Nivel2::getPlataformas() const { return plataformas; }