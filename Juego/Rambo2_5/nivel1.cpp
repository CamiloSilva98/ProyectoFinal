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
static const float LADO_CELDA = 20.0f;
// el guardia mide 12 de radio, los 2 de mas evitan que roce las esquinas
static const float RADIO_PASO = 14.0f;

// rutas de patrulla: x inicial, y inicial, x final, y final
// el orden importa: en Facil solo salen los primeros 6 guardias
static const float RUTAS[10][4] = {
    {540, 95, 820, 95},      // habitacion de control, junto a la salida a la meta
    {555, 200, 555, 470},    // pasillo izquierdo de monitoreo
    {170, 250, 420, 250},    // parqueadero, fila de arriba
    {680, 560, 1000, 560},   // pasillo bajo el complejo
    {905, 200, 905, 470},    // pasillo derecho de monitoreo
    {200, 440, 440, 440},    // parqueadero, fila de abajo
    {700, 670, 1000, 670},   // pasillo sur
    {690, 340, 770, 340},    // dentro de la habitacion interna
    {100, 20, 330, 20},      // franja norte, sobre el parqueadero
    {1130, 260, 1130, 480}   // zona este
};

// un guardia en su puesto no se distrae: detecta mucho mas rapido que uno que patrulla
static const float FACTOR_DETECCION_ESTATICO = 5.0f;
static const float FACTOR_ALCANCE_ESTATICO = 1.4f;
static const float FACTOR_APERTURA_ESTATICO = 1.3f;

// guardias que no patrullan: x, y y hacia donde miran (radianes)
// el orden importa: Facil usa el primero, Normal los dos primeros y Dificil los tres
static const float PUESTOS[3][3] = {
    {1000, 85, 3.1416f},     // entrada a la zona de la meta
    {440, 28, 0.0f},         // paso entre el parqueadero y el complejo, por el norte
    {1245, 540, 1.5708f}     // paso del sur: la abertura junto al borde derecho, bajo el muro inferior
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
        config.estaticos = 1;
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
        config.estaticos = 3;
        config.alcance = 250.0f;
        config.apertura = 1.2f;
        config.amplitud = 1.0f;
        config.frecuencia = 1.6f;
        config.memoria = 12;
        config.velocidadDeteccion = 1.0f;
    }
    else config.estaticos = 2;
    return config;
}

Nivel1::Nivel1(float ancho, float alto, ConfigNivel1 config)
    : Nivel(config.tiempo), anchoNivel(ancho), altoNivel(alto), config(config),
    rambo(nullptr), meta({1200.0f, 60.0f, 55.0f}), columnas(0), filas(0),
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
    construirRejilla();

    rambo = new Rambo(60.0f, 660.0f, 100, config.flechas, VELOCIDAD_RAMBO, RADIO_RAMBO);
    rambo->setParametrosFatiga(100.0f, 0.0f, 0.0f, 0.5f);
    rambo->agregarPiedras(config.piedras);
    agregarEntidad(rambo);
    protegerDeLimpieza(rambo);

    crearGuardias();
}

void Nivel1::crearMapa()
{
    // complejo central: pared izquierda, pared inferior y pared derecha (salida a la meta arriba)
    obstaculos.push_back({TipoObstaculo::Muro, 480, 45, 0, 20, 555});
    obstaculos.push_back({TipoObstaculo::Muro, 480, 600, 0, 740, 20});
    obstaculos.push_back({TipoObstaculo::Muro, 960, 155, 0, 20, 365});

    // division entre control y monitoreo, con puerta de 110 a la derecha
    obstaculos.push_back({TipoObstaculo::Muro, 500, 145, 0, 350, 20});

    // habitacion interna, con puerta de 110 abajo
    obstaculos.push_back({TipoObstaculo::Muro, 610, 255, 0, 20, 185});
    obstaculos.push_back({TipoObstaculo::Muro, 610, 255, 0, 240, 20});
    obstaculos.push_back({TipoObstaculo::Muro, 830, 255, 0, 20, 185});
    obstaculos.push_back({TipoObstaculo::Muro, 610, 420, 0, 65, 20});
    obstaculos.push_back({TipoObstaculo::Muro, 785, 420, 0, 65, 20});

    // entrada al complejo de 120 entre x=500 y x=620
    obstaculos.push_back({TipoObstaculo::Muro, 620, 500, 0, 360, 20});

    // la meta solo se alcanza pasando por las habitaciones
    obstaculos.push_back({TipoObstaculo::Muro, 960, 210, 0, 320, 20});

    // parqueadero
    obstaculos.push_back({TipoObstaculo::Carro, 90, 95, 0, 32, 70});
    obstaculos.push_back({TipoObstaculo::Carro, 225, 95, 0, 32, 70});
    obstaculos.push_back({TipoObstaculo::Carro, 360, 95, 0, 32, 70});
    obstaculos.push_back({TipoObstaculo::Carro, 165, 340, 0, 120, 40});
    obstaculos.push_back({TipoObstaculo::Carro, 90, 495, 0, 32, 70});
    obstaculos.push_back({TipoObstaculo::Carro, 360, 495, 0, 32, 70});

    obstaculos.push_back({TipoObstaculo::Columna, 106, 55, 18, 0, 0});
    obstaculos.push_back({TipoObstaculo::Columna, 241, 55, 18, 0, 0});
    obstaculos.push_back({TipoObstaculo::Columna, 376, 55, 18, 0, 0});

    // zonas oscuras: un respiro cerca del inicio, uno en cada tramo largo y uno antes de la meta
    sombras.push_back({200, 650, 55});
    sombras.push_back({330, 320, 60});
    sombras.push_back({560, 555, 45});
    sombras.push_back({555, 330, 50});
    sombras.push_back({905, 330, 50});
    sombras.push_back({910, 60, 50});
    sombras.push_back({1050, 110, 50});

    // extintores donde el humo sirve para cegar a un guardia que cuida un paso obligado
    agregarEntidad(new Extintor(260, 560));
    agregarEntidad(new Extintor(535, 470));
    agregarEntidad(new Extintor(905, 440));
    agregarEntidad(new Extintor(520, 100));
    agregarEntidad(new Extintor(1010, 70));
}

void Nivel1::crearGuardias()
{
    int cantidad = config.guardias;
    if (cantidad > 10) cantidad = 10;

    for (int i = 0; i < cantidad; ++i)
    {
        Guardia *guardia = new Guardia(RUTAS[i][0], RUTAS[i][1], RUTAS[i][2], RUTAS[i][3], 60.0f,
                                       config.alcance, config.apertura, config.amplitud, config.frecuencia, this);
        agregarEntidad(guardia);
        agentes.push_back(new AgenteGuardia(guardia, this, config.memoria, config.velocidadDeteccion));
    }

    for (int i = 0; i < config.estaticos && i < 3; ++i)
    {
        Guardia *guardia = new Guardia(PUESTOS[i][0], PUESTOS[i][1], PUESTOS[i][0], PUESTOS[i][1], 60.0f,
                                       config.alcance * FACTOR_ALCANCE_ESTATICO, config.apertura * FACTOR_APERTURA_ESTATICO,
                                       config.amplitud, config.frecuencia, this);
        guardia->fijarPuesto(PUESTOS[i][2]);
        agregarEntidad(guardia);
        agentes.push_back(new AgenteGuardia(guardia, this, config.memoria,
                                            config.velocidadDeteccion * FACTOR_DETECCION_ESTATICO));
    }
}

void Nivel1::actualizar(float dt)
{
    if (rambo == nullptr || victoria || derrota) return;

    retirarGuardiasMuertos();
    rambo->setEnCobertura(estaEnSombra(rambo->getX(), rambo->getY())
                          || estaEnHumo(rambo->getX(), rambo->getY()));

    Nivel::actualizar(dt);
    separarGuardiasDeObstaculos();

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

// la rejilla marca las celdas por las que un guardia no cabe
void Nivel1::construirRejilla()
{
    columnas = static_cast<int>(std::ceil(anchoNivel / LADO_CELDA));
    filas = static_cast<int>(std::ceil(altoNivel / LADO_CELDA));
    celdaBloqueada.assign(columnas * filas, false);

    for (int celda = 0; celda < columnas * filas; ++celda)
    {
        float cx = (celda % columnas + 0.5f) * LADO_CELDA;
        float cy = (celda / columnas + 0.5f) * LADO_CELDA;
        float nx, ny, penetracion;
        celdaBloqueada[celda] = chocaConObstaculo(cx, cy, RADIO_PASO, nx, ny, penetracion);
    }
}

// punto al que debe apuntar un guardia para llegar al destino sin chocar con los muros:
// si hay paso libre va derecho, si no busca el camino por la rejilla
void Nivel1::siguientePunto(float x, float y, float destinoX, float destinoY, float &puntoX, float &puntoY) const
{
    puntoX = destinoX;
    puntoY = destinoY;
    if (hayPasoLibre(x, y, destinoX, destinoY, RADIO_PASO)) return;

    int inicio = static_cast<int>(limitar(y / LADO_CELDA, 0, filas - 1)) * columnas
                 + static_cast<int>(limitar(x / LADO_CELDA, 0, columnas - 1));
    int meta = static_cast<int>(limitar(destinoY / LADO_CELDA, 0, filas - 1)) * columnas
               + static_cast<int>(limitar(destinoX / LADO_CELDA, 0, columnas - 1));
    if (inicio == meta) return;

    std::vector<int> anterior(columnas * filas, -1);
    std::vector<int> cola;
    cola.push_back(inicio);
    anterior[inicio] = inicio;

    const int saltoColumna[4] = {1, -1, 0, 0};
    const int saltoFila[4] = {0, 0, 1, -1};
    for (std::size_t i = 0; i < cola.size() && anterior[meta] == -1; ++i)
    {
        for (int k = 0; k < 4; ++k)
        {
            int columna = cola[i] % columnas + saltoColumna[k];
            int fila = cola[i] / columnas + saltoFila[k];
            if (columna < 0 || columna >= columnas || fila < 0 || fila >= filas) continue;

            int vecina = fila * columnas + columna;
            // la meta puede quedar pegada a un muro, a esa celda se le permite entrar
            if (anterior[vecina] != -1 || (celdaBloqueada[vecina] && vecina != meta)) continue;
            anterior[vecina] = cola[i];
            cola.push_back(vecina);
        }
    }
    if (anterior[meta] == -1) return;

    std::vector<int> camino;
    for (int celda = meta; celda != inicio; celda = anterior[celda])
        camino.insert(camino.begin(), celda);

    // se apunta a la celda mas lejana que se ve en linea recta para que no camine en zigzag
    std::size_t elegida = 0;
    for (std::size_t i = 1; i < camino.size(); ++i)
    {
        float cx = (camino[i] % columnas + 0.5f) * LADO_CELDA;
        float cy = (camino[i] / columnas + 0.5f) * LADO_CELDA;
        if (!hayPasoLibre(x, y, cx, cy, RADIO_PASO)) break;
        elegida = i;
    }
    puntoX = (camino[elegida] % columnas + 0.5f) * LADO_CELDA;
    puntoY = (camino[elegida] / columnas + 0.5f) * LADO_CELDA;
}

// por si un guardia roza una esquina: lo saca del obstaculo
void Nivel1::separarGuardiasDeObstaculos()
{
    for (AgenteGuardia *agente : agentes)
    {
        Guardia *guardia = agente->getGuardia();
        float nx, ny, penetracion;
        if (chocaConObstaculo(guardia->getX(), guardia->getY(), guardia->getRadio(), nx, ny, penetracion))
            guardia->setPosicion(guardia->getX() + nx * penetracion, guardia->getY() + ny * penetracion);
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

bool Nivel1::hayLineaDeVista(float x1, float y1, float x2, float y2) const
{
    return hayPasoLibre(x1, y1, x2, y2, 0.0f);
}

// se revisan puntos cada 5 px sobre la linea, es suficiente para obstaculos de mas de 15 px
bool Nivel1::hayPasoLibre(float x1, float y1, float x2, float y2, float radio) const
{
    float largo = distancia(x1, y1, x2, y2);
    int pasos = static_cast<int>(largo / 5.0f);
    float nx, ny, penetracion;

    for (int i = 1; i < pasos; ++i)
    {
        float t = static_cast<float>(i) / pasos;
        if (chocaConObstaculo(x1 + (x2 - x1) * t, y1 + (y2 - y1) * t, radio, nx, ny, penetracion))
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
