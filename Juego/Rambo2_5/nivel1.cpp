#include "nivel1.h"
#include "agenteguardia.h"
#include "guardia.h"
#include "cadaver.h"
#include "extintor.h"
#include "nubehumo.h"
#include "ondaruido.h"
#include "piedra.h"
#include <cmath>

static const float VELOCIDAD_SONIDO = 300.0f;
static const float RADIO_PROYECTIL = 3.0f;
static const float VELOCIDAD_FLECHA = 500.0f;
static const float VELOCIDAD_MINIMA_FLECHA = 40.0f;
static const float ARRASTRE_FLECHA = 1.0f;
static const float ARRASTRE_PIEDRA = 2.0f;
static const float VELOCIDAD_MINIMA_PIEDRA = 25.0f;
static const float RADIO_HUMO = 60.0f;
static const float DURACION_HUMO = 8.0f;
static const float RADIO_RAMBO = 10.0f;
static const float VELOCIDAD_RAMBO = 110.0f;
static const int DANO_FLECHA = 10;

// rutas de patrulla: x inicial, y inicial, x final, y final
static const float RUTAS[10][4] = {
    {360, 100, 560, 100},
    {400, 320, 400, 480},
    {650, 60, 950, 60},
    {760, 300, 760, 500},
    {1000, 100, 1250, 100},
    {950, 350, 950, 650},
    {1150, 450, 1250, 650},
    {100, 200, 250, 200},
    {500, 500, 650, 650},
    {900, 50, 900, 150}
};

static float distancia(float x1, float y1, float x2, float y2)
{
    float dx = x2 - x1;
    float dy = y2 - y1;
    return std::sqrt(dx * dx + dy * dy);
}

static float limitar(float valor, float minimo, float maximo)
{
    if (valor < minimo) return minimo;
    if (valor > maximo) return maximo;
    return valor;
}

ConfigNivel1 configurarDificultad(Dificultad dificultad)
{
    ConfigNivel1 config;

    if (dificultad == Dificultad::Facil)
    {
        config.tiempo = 240.0f;
        config.guardias = 6;
        config.alcance = 150.0f;
        config.apertura = 0.8f;
        config.amplitud = 0.5f;
        config.frecuencia = 0.8f;
        config.memoria = 3;
        config.velocidadDeteccion = 0.5f;
    }
    else if (dificultad == Dificultad::Dificil)
    {
        config.tiempo = 120.0f;
        config.alcance = 250.0f;
        config.apertura = 1.2f;
        config.amplitud = 1.0f;
        config.frecuencia = 1.6f;
        config.memoria = 12;
        config.velocidadDeteccion = 1.0f;
    }
    return config;
}

Nivel1::Nivel1(float ancho, float alto, ConfigNivel1 config)
    : Nivel(config.tiempo), anchoNivel(ancho), altoNivel(alto), config(config),
    rambo(nullptr), meta({1200.0f, 60.0f, 55.0f}),
    modeloFlecha(ARRASTRE_FLECHA, 1.0f), modeloPiedra(ARRASTRE_PIEDRA, 1.0f), reboteFlecha(0.7f),
    victoria(false), derrota(false)
{
}

Nivel1::~Nivel1()
{
    for (AgenteGuardia *agente : agentes) delete agente;
}

void Nivel1::iniciar()
{
    crearMapa();

    rambo = new Rambo(60.0f, 660.0f, 100, config.flechas, VELOCIDAD_RAMBO, RADIO_RAMBO);
    rambo->setParametrosFatiga(100.0f, 0.0f, 0.0f, 0.5f);
    rambo->agregarPiedras(config.piedras);
    agregarEntidad(rambo);
    protegerDeLimpieza(rambo);

    crearGuardias();
}

void Nivel1::crearMapa()
{
    obstaculos.push_back({TipoObstaculo::Muro, 300, 0, 0, 20, 300});
    obstaculos.push_back({TipoObstaculo::Muro, 700, 420, 0, 20, 300});
    obstaculos.push_back({TipoObstaculo::Muro, 1000, 200, 0, 200, 20});

    obstaculos.push_back({TipoObstaculo::Carro, 150, 520, 0, 70, 32});
    obstaculos.push_back({TipoObstaculo::Carro, 420, 600, 0, 70, 32});
    obstaculos.push_back({TipoObstaculo::Carro, 520, 300, 0, 32, 70});
    obstaculos.push_back({TipoObstaculo::Carro, 820, 250, 0, 70, 32});
    obstaculos.push_back({TipoObstaculo::Carro, 850, 560, 0, 70, 32});
    obstaculos.push_back({TipoObstaculo::Carro, 1050, 420, 0, 32, 70});

    obstaculos.push_back({TipoObstaculo::Columna, 400, 200, 18, 0, 0});
    obstaculos.push_back({TipoObstaculo::Columna, 600, 450, 18, 0, 0});
    obstaculos.push_back({TipoObstaculo::Columna, 600, 150, 18, 0, 0});
    obstaculos.push_back({TipoObstaculo::Columna, 900, 400, 18, 0, 0});
    obstaculos.push_back({TipoObstaculo::Columna, 1100, 330, 18, 0, 0});
    obstaculos.push_back({TipoObstaculo::Columna, 250, 380, 18, 0, 0});

    sombras.push_back({200, 420, 70});
    sombras.push_back({640, 560, 80});
    sombras.push_back({1000, 520, 70});
    sombras.push_back({780, 100, 60});

    agregarEntidad(new Extintor(220, 620));
    agregarEntidad(new Extintor(760, 330));
    agregarEntidad(new Extintor(1100, 150));
}

void Nivel1::crearGuardias()
{
    int cantidad = config.guardias;
    if (cantidad > 10) cantidad = 10;

    for (int i = 0; i < cantidad; ++i)
    {
        Guardia *guardia = new Guardia(RUTAS[i][0], RUTAS[i][1], RUTAS[i][2], RUTAS[i][3], 60.0f,
                                       config.alcance, config.apertura, config.amplitud, config.frecuencia);
        agregarEntidad(guardia);
        agentes.push_back(new AgenteGuardia(guardia, this, config.memoria, config.velocidadDeteccion));
    }
}

void Nivel1::actualizar(float dt)
{
    if (rambo == nullptr || victoria || derrota) return;

    retirarGuardiasMuertos();
    rambo->setEnCobertura(estaEnSombra(rambo->getX(), rambo->getY())
                          || estaEnHumo(rambo->getX(), rambo->getY()));

    Nivel::actualizar(dt);

    if (rambo->consumirRuido())
        generarEstimulo(rambo->getX(), rambo->getY(), 300.0f, 1.0f);

    actualizarGuardias(dt);
    resolverColisiones();

    if (!rambo->estaViva() || tiempoAgotado())
    {
        derrota = true;
        return;
    }

    if (distancia(rambo->getX(), rambo->getY(), meta.x, meta.y) < meta.radio)
        victoria = true;
}

bool Nivel1::comprobarVictoria() const { return victoria; }
bool Nivel1::comprobarDerrota() const { return derrota; }

// el guardia muerto sigue en la lista hasta el final del paso, por eso se saca aqui su posicion
void Nivel1::retirarGuardiasMuertos()
{
    std::size_t i = 0;
    while (i < agentes.size())
    {
        Guardia *guardia = agentes[i]->getGuardia();
        if (guardia->estaViva())
        {
            i++;
            continue;
        }
        agregarEntidad(new Cadaver(guardia->getX(), guardia->getY()));
        delete agentes[i];
        agentes.erase(agentes.begin() + i);
    }
}

void Nivel1::actualizarGuardias(float dt)
{
    for (AgenteGuardia *agente : agentes)
        agente->actualizar(dt);
}

void Nivel1::generarEstimulo(float x, float y, float alcance, float intensidad)
{
    agregarEntidad(new OndaRuido(x, y, VELOCIDAD_SONIDO, alcance, intensidad));
}

void Nivel1::resolverColisiones()
{
    const std::size_t cantidad = entidades.size();
    for (std::size_t i = 0; i < cantidad; ++i)
    {
        Proyectil *proyectil = dynamic_cast<Proyectil*>(entidades[i]);
        if (proyectil == nullptr || !proyectil->estaViva()) continue;

        float px = proyectil->getX();
        float py = proyectil->getY();
        if (px < 0.0f || px > anchoNivel || py < 0.0f || py > altoNivel)
        {
            proyectil->recibirDano(1);
            continue;
        }

        Piedra *piedra = dynamic_cast<Piedra*>(proyectil);

        float nx, ny, penetracion;
        if (chocaConObstaculo(px, py, RADIO_PROYECTIL, nx, ny, penetracion))
        {
            if (piedra != nullptr) generarEstimulo(px, py, 350.0f, 1.0f);
            else generarEstimulo(px, py, 150.0f, 0.4f);

            proyectil->setPosicion(px + nx * (penetracion + 0.5f), py + ny * (penetracion + 0.5f));
            proyectil->colisionar(nx, ny);
            continue;
        }

        if (piedra != nullptr)
        {
            if (piedra->estaDetenida(VELOCIDAD_MINIMA_PIEDRA))
            {
                generarEstimulo(px, py, 350.0f, 1.0f);
                piedra->recibirDano(1);
            }
            continue;
        }

        float vx = proyectil->getVelocidadX();
        float vy = proyectil->getVelocidadY();
        if (std::sqrt(vx * vx + vy * vy) < VELOCIDAD_MINIMA_FLECHA)
        {
            proyectil->recibirDano(1);
            continue;
        }

        for (std::size_t j = 0; j < cantidad && proyectil->estaViva(); ++j)
        {
            Guardia *guardia = dynamic_cast<Guardia*>(entidades[j]);
            if (guardia == nullptr || !guardia->estaViva()) continue;
            if (distancia(px, py, guardia->getX(), guardia->getY()) < guardia->getRadio() + RADIO_PROYECTIL)
            {
                guardia->recibirDano(proyectil->getDano());
                proyectil->recibirDano(1);
                generarEstimulo(px, py, 150.0f, 0.4f);
            }
        }

        for (std::size_t j = 0; j < cantidad && proyectil->estaViva(); ++j)
        {
            Extintor *extintor = dynamic_cast<Extintor*>(entidades[j]);
            if (extintor == nullptr || !extintor->estaViva()) continue;
            if (distancia(px, py, extintor->getX(), extintor->getY()) < extintor->getRadio() + RADIO_PROYECTIL)
            {
                extintor->recibirDano(1);
                proyectil->recibirDano(1);
                agregarEntidad(new NubeHumo(extintor->getX(), extintor->getY(), RADIO_HUMO, DURACION_HUMO));
            }
        }
    }
}

bool Nivel1::chocaConObstaculo(float px, float py, float radio, float &nx, float &ny, float &penetracion) const
{
    for (const Obstaculo &o : obstaculos)
    {
        if (o.tipo == TipoObstaculo::Columna)
        {
            float d = distancia(px, py, o.x, o.y);
            if (d >= o.radio + radio) continue;
            nx = d > 0.0f ? (px - o.x) / d : 1.0f;
            ny = d > 0.0f ? (py - o.y) / d : 0.0f;
            penetracion = o.radio + radio - d;
            return true;
        }

        float cercanoX = limitar(px, o.x, o.x + o.ancho);
        float cercanoY = limitar(py, o.y, o.y + o.alto);
        float d = distancia(px, py, cercanoX, cercanoY);
        if (d > radio) continue;

        if (d > 0.0f)
        {
            nx = (px - cercanoX) / d;
            ny = (py - cercanoY) / d;
            penetracion = radio - d;
            return true;
        }

        // el centro quedo dentro del rectangulo: se saca por el lado mas cercano
        float izquierda = px - o.x;
        float derecha = o.x + o.ancho - px;
        float arriba = py - o.y;
        float abajo = o.y + o.alto - py;
        float menor = izquierda;
        nx = -1.0f; ny = 0.0f;
        if (derecha < menor) { menor = derecha; nx = 1.0f; ny = 0.0f; }
        if (arriba < menor) { menor = arriba; nx = 0.0f; ny = -1.0f; }
        if (abajo < menor) { menor = abajo; nx = 0.0f; ny = 1.0f; }
        penetracion = menor + radio;
        return true;
    }
    return false;
}

// se revisan puntos cada 5 px sobre la linea, es suficiente para obstaculos de mas de 15 px
bool Nivel1::hayLineaDeVista(float x1, float y1, float x2, float y2) const
{
    float largo = distancia(x1, y1, x2, y2);
    int pasos = static_cast<int>(largo / 5.0f);
    float nx, ny, penetracion;

    for (int i = 1; i < pasos; ++i)
    {
        float t = static_cast<float>(i) / pasos;
        if (chocaConObstaculo(x1 + (x2 - x1) * t, y1 + (y2 - y1) * t, 0.0f, nx, ny, penetracion))
            return false;
    }
    return true;
}

bool Nivel1::estaEnHumo(float px, float py) const
{
    for (entidad *e : entidades)
    {
        NubeHumo *nube = dynamic_cast<NubeHumo*>(e);
        if (nube != nullptr && nube->estaViva()
            && distancia(px, py, nube->getX(), nube->getY()) < nube->getRadio())
            return true;
    }
    return false;
}

bool Nivel1::estaEnSombra(float px, float py) const
{
    for (const Circulo &sombra : sombras)
        if (distancia(px, py, sombra.x, sombra.y) < sombra.radio) return true;
    return false;
}

bool Nivel1::ramboChoca() const
{
    float nx, ny, penetracion;
    float x = rambo->getX();
    float y = rambo->getY();
    float r = rambo->getRadio();

    if (x < r || x > anchoNivel - r || y < r || y > altoNivel - r) return true;
    return chocaConObstaculo(x, y, r, nx, ny, penetracion);
}

// se mueve primero en x y luego en y para que pueda deslizarse por los muros
void Nivel1::moverRambo(float dirX, float dirY, float dt)
{
    if (!rambo->estaViva()) return;

    if (dirX != 0.0f && dirY != 0.0f)
    {
        dirX *= 0.7071f;
        dirY *= 0.7071f;
    }

    float xAntes = rambo->getX();
    float yAntes = rambo->getY();

    rambo->mover(dirX, 0.0f, dt);
    if (ramboChoca()) rambo->setPosicion(xAntes, rambo->getY());

    rambo->mover(0.0f, dirY, dt);
    if (ramboChoca()) rambo->setPosicion(rambo->getX(), yAntes);
}

void Nivel1::correrRambo(bool correr)
{
    rambo->setCorriendo(correr);
}

void Nivel1::dispararFlecha(float destinoX, float destinoY)
{
    if (!rambo->estaViva()) return;

    float dx = destinoX - rambo->getX();
    float dy = destinoY - rambo->getY();
    float largo = std::sqrt(dx * dx + dy * dy);
    if (largo < 1.0f) return;
    if (!rambo->disparar()) return;

    Proyectil *flecha = new Proyectil(rambo->getX(), rambo->getY(),
                                      dx / largo * VELOCIDAD_FLECHA, dy / largo * VELOCIDAD_FLECHA,
                                      DANO_FLECHA, &modeloFlecha, &reboteFlecha, 3);
    agregarEntidad(flecha);
}

// la velocidad inicial se calcula para que el arrastre la frene cerca del destino
void Nivel1::lanzarPiedra(float destinoX, float destinoY)
{
    if (!rambo->estaViva()) return;

    float dx = destinoX - rambo->getX();
    float dy = destinoY - rambo->getY();
    float largo = std::sqrt(dx * dx + dy * dy);
    if (largo < 1.0f) return;
    if (!rambo->lanzarPiedra()) return;

    float rapidez = largo * ARRASTRE_PIEDRA + VELOCIDAD_MINIMA_PIEDRA;
    agregarEntidad(new Piedra(rambo->getX(), rambo->getY(), dx / largo * rapidez, dy / largo * rapidez,
                              &modeloPiedra));
}

std::vector<Cadaver*> Nivel1::getCadaveres() const
{
    std::vector<Cadaver*> lista;
    for (entidad *e : entidades)
    {
        Cadaver *cadaver = dynamic_cast<Cadaver*>(e);
        if (cadaver != nullptr && cadaver->estaViva()) lista.push_back(cadaver);
    }
    return lista;
}

std::vector<OndaRuido*> Nivel1::getOndas() const
{
    std::vector<OndaRuido*> lista;
    for (entidad *e : entidades)
    {
        OndaRuido *onda = dynamic_cast<OndaRuido*>(e);
        if (onda != nullptr && onda->estaViva()) lista.push_back(onda);
    }
    return lista;
}

Rambo *Nivel1::getRambo() const { return rambo; }
const std::vector<AgenteGuardia*> &Nivel1::getAgentes() const { return agentes; }
const std::vector<Obstaculo> &Nivel1::getObstaculos() const { return obstaculos; }
const std::vector<Circulo> &Nivel1::getSombras() const { return sombras; }
const Circulo &Nivel1::getMeta() const { return meta; }
float Nivel1::getAncho() const { return anchoNivel; }
float Nivel1::getAlto() const { return altoNivel; }
