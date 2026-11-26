#include "gestorjuego.h"
#include "itemfigura.h"
#include "itemguardia.h"
#include "itempersonaje.h"
#include "itemrambo.h"
#include "guardia.h"
#include "cadaver.h"
#include "extintor.h"
#include "nubehumo.h"
#include "ondaruido.h"
#include "piedra.h"
#include "suministro.h"
#include "policia.h"
#include "armaoscilante.h"
#include <QFont>
#include <QGraphicsPixmapItem>
#include <QPainter>
#include <algorithm>
#include <cmath>
#include <QGraphicsView>
#include <QTransform>

static const float PASO = 0.016f;
static const float GRADOS_POR_RADIAN = 57.29578f;
static const int LADO_RAMBO = 52;
static const int LADO_GUARDIA = 48;
static const int CUADROS_RAMBO = 4;
static const int CUADROS_GUARDIA = 6;
static const float DURACION_APUNTADO = 0.35f;
static const float ALTURA_SIMBOLO = 44.0f;
static const float DURACION_IMPACTO = 0.3f;
static const float ZOOM_NIVEL2 = 1.0f;
static const float ESCALA_RAMBO_N2 = 3.0f;
static const float ANCHO_NIVEL2 = 1916.0f;   // ancho del fondo
static const float ZONA_CAMARA = 200.0f;     // cuanto se aleja rambo del centro antes de que la camara lo siga
static const float ESCALA_TORRETA = 1.8f;
static const float PIVOTE_TORRETA_X = 37.0f;
static const float PIVOTE_TORRETA_Y = 15.0f;
static const int CUADROS_GRANADA = 3;
static const int CUADROS_EXPLOSION = 3;
static const float ESCALA_POLICIA = 3.0f;      // 40 px * 3 = 120, cerca de los 126 de la colisión
static const float ESCALA_GRANADA = 2.0f;
static const float ESCALA_BALA_TORRETA = 2.0f;
static const float ESCALA_ITEM = 1.5f;
static const float DURACION_POSE_DISPARO = 0.3f;
static const float DURACION_POSE_LANZAR = 0.5f;
static const float DURACION_ANIM_MUERTE = 0.8f;   // lo que tarda en caer
static const float DURACION_CUERPO = 1.2f;        // luego desaparece

// la dificultad escogida tambien se aplica al nivel 2
static float factorNivel2(Dificultad d)
{
    if (d == Dificultad::Facil) return 0.7f;
    if (d == Dificultad::Dificil) return 1.4f;
    return 1.0f;
}

GestorJuego::GestorJuego(QGraphicsScene *escena, QObject *parent)
    : QObject(parent), escena(escena), pantalla(Pantalla::Menu), nivelActual(1), dificultad(Dificultad::Normal),
    nivel1(nullptr), nivel2(nullptr), hud(nullptr),
    direccion(1.0f), tiempoAnim(0.0f), tiempoHerido(0.0f), tiempoItems(0.0f), saludAnterior(0),
    animRambo(0.0f), animGuardias(0.0f), tiempoApuntando(0.0f), anguloRambo(0.0f), camX(0.0f)
{
    spritesRambo.load(":/sprites/recursos/sprites/rambo_sprites.png");
    spriteEnergia.load(":/sprites/recursos/sprites/item.png");
    spriteBala.load(":/sprites/recursos/sprites/bala.png");
    fondoNivel1.load(":/sprites/recursos/sprites/fondo_nivel1.png");
    spriteCarroCampero.load(":/sprites/recursos/sprites/carro_campero.png");
    spriteCarroSedan.load(":/sprites/recursos/sprites/carro_sedan.png");
    spriteMuroVertical.load(":/sprites/recursos/sprites/muro_vertical.png");
    spriteMuroHorizontal.load(":/sprites/recursos/sprites/muro_horizontal.png");
    spriteExtintor.load(":/sprites/recursos/sprites/extintor.png");
    spritesRamboNivel1.load(":/sprites/recursos/sprites/rambo_cenital.png");
    spritesGuardia.load(":/sprites/recursos/sprites/guardia_cenital.png");
    spriteAlerta.load(":/sprites/recursos/sprites/simbolo_alerta.png");
    spriteDuda.load(":/sprites/recursos/sprites/simbolo_duda.png");
    QPixmap hojaCadaveres(":/sprites/recursos/sprites/cadaver_guardia.png");
    for (int i = 0; i < 2; ++i)
        cuerposGuardia[i] = hojaCadaveres.copy(i * hojaCadaveres.height(), 0, hojaCadaveres.height(),
                                               hojaCadaveres.height());
    spriteMunicion.load(":/sprites/recursos/sprites/municion.png");
    spriteTorreta.load(":/sprites/recursos/sprites/torreta.png");
    spriteFusilero.load(":/sprites/recursos/sprites/enemigoshoot.png");
    spriteGranada.load(":/sprites/recursos/sprites/granada.png");
    spriteExplosion.load(":/sprites/recursos/sprites/explosion.png");
    spriteBalaTorreta.load(":/sprites/recursos/sprites/balatorreta.png");
    cuadrosCorrer = cargarCuadros(":/sprites/recursos/sprites/enemigo", 7);
    cuadrosLanzar = cargarCuadros(":/sprites/recursos/sprites/granadero", 6);
    cuadrosMuerte = cargarCuadros(":/sprites/recursos/sprites/enemigosMuerte", 3);

    if (spriteMunicion.isNull()) qWarning("No se pudo cargar municion.png, revisar recursos.qrc");
    if (spriteTorreta.isNull()) qWarning("No se pudo cargar torreta.png, revisar recursos.qrc");
    if (spritesRambo.isNull()) qWarning("No se pudo cargar rambo_sprites.png, revisar recursos.qrc");
    if (spriteEnergia.isNull()) qWarning("No se pudo cargar item.png, revisar recursos.qrc");
    if (spriteBala.isNull()) qWarning("No se pudo cargar bala.png, revisar recursos.qrc");
    if (fondoNivel1.isNull() || spriteCarroCampero.isNull() || spriteCarroSedan.isNull()
        || spriteMuroVertical.isNull() || spriteMuroHorizontal.isNull() || spriteExtintor.isNull())
        qWarning("No se pudieron cargar las imagenes del nivel 1, revisar recursos.qrc");
    if (spritesRamboNivel1.isNull() || spritesGuardia.isNull() || spriteAlerta.isNull() || spriteDuda.isNull()
        || cuerposGuardia[0].isNull())
        qWarning("No se pudieron cargar los sprites de rambo, de los guardias o de sus avisos, revisar recursos.qrc");
    if (spriteFusilero.isNull()) qWarning("No se pudo cargar enemigoshoot.png, revisar recursos.qrc");
    if (spriteGranada.isNull()) qWarning("No se pudo cargar granada.png, revisar recursos.qrc");
    if (spriteExplosion.isNull()) qWarning("No se pudo cargar explosion.png, revisar recursos.qrc");
    if (spriteBalaTorreta.isNull()) qWarning("No se pudo cargar balatorreta.png, revisar recursos.qrc");


    escena->setSceneRect(0, 0, ANCHO_ESCENA, ALTO_ESCENA);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &GestorJuego::actualizar);
    timer->start(16);

    mostrarMenu();
}

GestorJuego::~GestorJuego()
{
    descartarNiveles();
}

void GestorJuego::teclaPresionada(int tecla)
{
    teclas.insert(tecla);
    bool enter = (tecla == Qt::Key_Return || tecla == Qt::Key_Enter);

    switch (pantalla)
    {
    case Pantalla::Menu:
        if (tecla == Qt::Key_1 || enter) iniciarNivel1();
        else if (tecla == Qt::Key_2) iniciarNivel2();
        break;
    case Pantalla::JugandoNivel1:
    case Pantalla::JugandoNivel2:
        if (tecla == Qt::Key_Escape) mostrarMenu();
        break;
    case Pantalla::Nivel1Completado:
        if (enter) iniciarNivel2();
        else if (tecla == Qt::Key_Escape) mostrarMenu();
        break;
    case Pantalla::Derrota:
        if (enter && nivelActual == 1) iniciarNivel1();
        else if (enter) iniciarNivel2();
        else if (tecla == Qt::Key_Escape) mostrarMenu();
        break;
    case Pantalla::FinJuego:
        if (enter || tecla == Qt::Key_Escape) mostrarMenu();
        break;
    }
}

void GestorJuego::teclaSoltada(int tecla)
{
    teclas.remove(tecla);
}

void GestorJuego::clic(QPointF punto, Qt::MouseButton boton)
{
    if (pantalla != Pantalla::JugandoNivel1) return;

    Rambo *rambo = nivel1->getRambo();
    float haciaElClic = std::atan2(punto.y() - rambo->getY(), punto.x() - rambo->getX()) * GRADOS_POR_RADIAN;
    if (boton == Qt::LeftButton)
    {
        int flechasAntes = rambo->getMunicion();
        nivel1->dispararFlecha(punto.x(), punto.y());
        // solo gira si la flecha salio de verdad (puede faltar municion o recarga)
        if (rambo->getMunicion() < flechasAntes)
        {
            anguloRambo = haciaElClic;
            tiempoApuntando = DURACION_APUNTADO;
        }
    }
    else if (boton == Qt::RightButton)
    {
        nivel1->lanzarPiedra(punto.x(), punto.y());
        anguloRambo = haciaElClic;
        tiempoApuntando = DURACION_APUNTADO;
    }
}

void GestorJuego::soltarTeclas()
{
    teclas.clear();
}

void GestorJuego::setDificultad(Dificultad nueva)
{
    if (nueva == dificultad) return;
    dificultad = nueva;

    if (pantalla == Pantalla::JugandoNivel1) iniciarNivel1();
    else if (pantalla == Pantalla::JugandoNivel2) nivel2->setDificultad(factorNivel2(dificultad));
    else if (pantalla == Pantalla::Menu) mostrarMenu();
}

Dificultad GestorJuego::getDificultad() const
{
    return dificultad;
}

QString GestorJuego::nombreDificultad(Dificultad dificultad)
{
    if (dificultad == Dificultad::Facil) return "Facil";
    if (dificultad == Dificultad::Dificil) return "Dificil";
    return "Normal";
}

void GestorJuego::actualizar()
{
    if (pantalla == Pantalla::JugandoNivel1) actualizarNivel1();
    else if (pantalla == Pantalla::JugandoNivel2) actualizarNivel2();
}

void GestorJuego::actualizarNivel1()
{
    float dirX = 0.0f;
    float dirY = 0.0f;
    if (teclas.contains(Qt::Key_A)) dirX -= 1.0f;
    if (teclas.contains(Qt::Key_D)) dirX += 1.0f;
    if (teclas.contains(Qt::Key_W)) dirY -= 1.0f;
    if (teclas.contains(Qt::Key_S)) dirY += 1.0f;

    bool corriendo = teclas.contains(Qt::Key_Shift);
    nivel1->correrRambo(corriendo);
    if (dirX != 0.0f || dirY != 0.0f)
    {
        nivel1->moverRambo(dirX, dirY, PASO);
        animRambo += corriendo ? PASO * 2 : PASO;
        if (tiempoApuntando <= 0.0f) anguloRambo = std::atan2(dirY, dirX) * GRADOS_POR_RADIAN;
    }
    else animRambo = 0.0f;
    if (tiempoApuntando > 0.0f) tiempoApuntando -= PASO;
    animGuardias += PASO;
    nivel1->actualizar(PASO);

    int saludActual = nivel1->getRambo()->getSalud();
    if (saludActual < saludAnterior) tiempoHerido = DURACION_IMPACTO;
    saludAnterior = saludActual;
    if (tiempoHerido > 0.0f) tiempoHerido -= PASO;

    if (nivel1->comprobarVictoria())
    {
        pantalla = Pantalla::Nivel1Completado;
        mostrarMensaje("NIVEL 1 COMPLETADO", "Enter: pasar al Nivel 2        Esc: menu");
        return;
    }
    if (nivel1->comprobarDerrota())
    {
        pantalla = Pantalla::Derrota;
        QString causa = nivel1->tiempoAgotado() ? "Se acabo el tiempo" : "Te descubrieron";
        mostrarMensaje("DERROTA", causa + "\nEnter: reintentar        Esc: menu");
        return;
    }

    dibujarNivel1();
}

void GestorJuego::actualizarNivel2()
{
    if (teclas.contains(Qt::Key_A)) { direccion = -1.0f; nivel2->moverRambo(-1.0f, PASO); }
    if (teclas.contains(Qt::Key_D)) { direccion = 1.0f; nivel2->moverRambo(1.0f, PASO); }
    if (teclas.contains(Qt::Key_Space)) nivel2->saltarRambo();
    if (teclas.contains(Qt::Key_J)) nivel2->dispararRambo(direccion);

    nivel2->actualizar(PASO);

    if (nivel2->comprobarVictoria())
    {
        pantalla = Pantalla::FinJuego;
        mostrarMensaje("JUEGO COMPLETADO", "Enter: volver al menu");
        return;
    }
    if (nivel2->comprobarDerrota())
    {
        pantalla = Pantalla::Derrota;
        mostrarMensaje("DERROTA", "Rambo cayo en la horda " + QString::number(nivel2->getNumeroHorda())
                                      + "\nEnter: reintentar        Esc: menu");
        return;
    }

    tiempoItems += PASO;
    if (teclas.contains(Qt::Key_A) || teclas.contains(Qt::Key_D)) tiempoAnim += PASO;
    else tiempoAnim = 0.0f;

    // un golpe deja el cuadro de herido un momento
    int saludActual = nivel2->getRambo()->getSalud();
    if (saludActual < saludAnterior) tiempoHerido = 0.25f;
    saludAnterior = saludActual;
    if (tiempoHerido > 0.0f) tiempoHerido -= PASO;

    dibujarNivel2();
    seguirConCamara();
}

void GestorJuego::mostrarMenu()
{
    descartarNiveles();
    limpiarEscena();
    pantalla = Pantalla::Menu;
    escena->setBackgroundBrush(QColor(20, 20, 30));

    agregarTexto("RAMBO", 80, 60, Qt::white);
    agregarTexto("1 o Enter: Nivel 1 (infiltracion)", 240, 24, Qt::white);
    agregarTexto("2: Nivel 2 (hordas)", 285, 24, Qt::white);
    agregarTexto("Dificultad: " + nombreDificultad(dificultad) + "  (se cambia en el menu de arriba, vale para los dos niveles)", 335, 20, Qt::yellow);
    agregarTexto("Nivel 1:  WASD mover   Shift correr   clic izquierdo flecha   clic derecho piedra", 400, 18, Qt::lightGray);
    agregarTexto("Llega a la zona verde sin que te vean. Los guardias oyen el ruido.", 430, 18, Qt::lightGray);
    agregarTexto("Nivel 2:  A y D mover   Espacio saltar   J disparar", 490, 18, Qt::lightGray);
    agregarTexto("Sobrevive las tres hordas. Esc vuelve al menu.", 520, 18, Qt::lightGray);
}

void GestorJuego::iniciarNivel1()
{
    descartarNiveles();
    limpiarEscena();
    nivelActual = 1;

    nivel1 = new Nivel1(ANCHO_ESCENA, ALTO_ESCENA, configurarDificultad(dificultad));
    nivel1->iniciar();

    animRambo = 0.0f;
    animGuardias = 0.0f;
    tiempoApuntando = 0.0f;
    anguloRambo = 0.0f;
    tiempoHerido = 0.0f;
    saludAnterior = nivel1->getRambo()->getSalud();

    escena->setBackgroundBrush(fondoNivel1.isNull() ? QBrush(QColor(45, 50, 45)) : QBrush(fondoNivel1));
    dibujarMapaNivel1();
    crearHud();
    pantalla = Pantalla::JugandoNivel1;
}

void GestorJuego::iniciarNivel2()
{
    descartarNiveles();
    limpiarEscena();
    nivelActual = 2;

    nivel2 = new Nivel2(ANCHO_NIVEL2, ALTO_ESCENA, 650);
    nivel2->setDificultad(factorNivel2(dificultad));
    nivel2->iniciar();

    direccion = 1.0f;
    tiempoAnim = 0.0f;
    tiempoHerido = 0.0f;
    tiempoItems = 0.0f;
    saludAnterior = nivel2->getRambo()->getSalud();

    // fondo del nivel, se estira a la escena y queda detras de las entidades
    QPixmap imagenFondo(":/sprites/recursos/sprites/fondo.png");
    QGraphicsPixmapItem *fondo = new QGraphicsPixmapItem(
        imagenFondo.scaled(ANCHO_NIVEL2, ALTO_ESCENA, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    fondo->setZValue(-10);
    agregarFijo(fondo, 0, 0);
    crearHud();
    activarCamara();
    seguirConCamara();
    pantalla = Pantalla::JugandoNivel2;
}

void GestorJuego::mostrarMensaje(const QString &titulo, const QString &detalle)
{
    limpiarEscena();
    agregarTexto(titulo, 250, 48, Qt::white);

    float y = 330;
    for (const QString &linea : detalle.split("\n"))
    {
        agregarTexto(linea, y, 22, Qt::lightGray);
        y += 40;
    }
}

void GestorJuego::descartarNiveles()
{
    delete nivel1;
    nivel1 = nullptr;
    delete nivel2;
    nivel2 = nullptr;
}

void GestorJuego::limpiarEscena()
{
    restablecerCamara();
    escena->clear();
    movibles.clear();
    hud = nullptr;
    muertes.clear();
    ultimosPolicias.clear();
}

void GestorJuego::borrarMovibles()
{
    qDeleteAll(movibles);
    movibles.clear();
}

void GestorJuego::agregarFijo(QGraphicsItem *item, float x, float y)
{
    item->setPos(x, y);
    escena->addItem(item);
}

QGraphicsItem *GestorJuego::agregarMovil(QGraphicsItem *item, float x, float y)
{
    agregarFijo(item, x, y);
    movibles.append(item);
    return item;
}

QGraphicsPixmapItem *GestorJuego::crearImagen(const QPixmap &imagen)
{
    QGraphicsPixmapItem *item = new QGraphicsPixmapItem(imagen);
    item->setOffset(-imagen.width() / 2.0, -imagen.height() / 2.0);
    return item;
}

void GestorJuego::agregarTexto(const QString &texto, float y, int tamano, const QColor &color)
{
    QGraphicsSimpleTextItem *item = escena->addSimpleText(texto, QFont("Arial", tamano));
    item->setBrush(color);
    item->setPos((ANCHO_ESCENA - item->boundingRect().width()) / 2, y);
}

void GestorJuego::crearHud()
{
    hud = escena->addSimpleText("", QFont("Arial", 12));
    hud->setBrush(Qt::white);
    hud->setPos(10, 6);
    hud->setZValue(100);
}

void GestorJuego::dibujarMapaNivel1()
{
    for (const Circulo &sombra : nivel1->getSombras())
        agregarFijo(new ItemFigura(Forma::Elipse, sombra.radio * 2, sombra.radio * 2, QColor(0, 0, 0, 110)),
                    sombra.x, sombra.y);

    const Circulo &meta = nivel1->getMeta();
    agregarFijo(new ItemFigura(Forma::Elipse, meta.radio * 2, meta.radio * 2, QColor(80, 220, 120, 90),
                               QPen(QColor(80, 220, 120), 2)),
                meta.x, meta.y);

    int carrosDibujados = 0;
    for (const Obstaculo &o : nivel1->getObstaculos())
    {
        if (o.tipo == TipoObstaculo::Columna)
        {
            agregarFijo(new ItemFigura(Forma::Elipse, o.radio * 2, o.radio * 2, QColor(130, 130, 130)),
                        o.x, o.y);
            continue;
        }

        float centroX = o.x + o.ancho / 2;
        float centroY = o.y + o.alto / 2;

        if (o.tipo == TipoObstaculo::Muro)
        {
            const QPixmap &imagen = (o.alto > o.ancho) ? spriteMuroVertical : spriteMuroHorizontal;
            QPixmap pared(static_cast<int>(o.ancho), static_cast<int>(o.alto));
            pared.fill(Qt::transparent);
            QPainter pintor(&pared);
            pintor.drawTiledPixmap(0, 0, pared.width(), pared.height(), imagen);
            pintor.end();
            agregarFijo(crearImagen(pared), centroX, centroY);
            continue;
        }

        const QPixmap &imagen = (carrosDibujados % 2 == 0) ? spriteCarroCampero : spriteCarroSedan;
        carrosDibujados++;
        float largo = std::max(o.ancho, o.alto);
        float corto = std::min(o.ancho, o.alto);
        QGraphicsPixmapItem *carro = crearImagen(imagen.scaled(static_cast<int>(largo), static_cast<int>(corto)));
        if (o.alto > o.ancho) carro->setRotation(90);
        agregarFijo(carro, centroX, centroY);
    }
}

void GestorJuego::dibujarNivel1()
{
    borrarMovibles();

    for (entidad *e : nivel1->getEntidades())
    {
        if (!e->estaViva()) continue;
        float x = e->getX();
        float y = e->getY();

        if (dynamic_cast<Rambo*>(e) != nullptr)
        {
            bool acechando = nivel1->getRambo()->estaAcechando();
            if (spritesRamboNivel1.isNull())
            {
                QColor color = acechando ? QColor(40, 120, 50) : QColor(90, 210, 90);
                agregarMovil(new ItemFigura(Forma::Elipse, e->getRadio() * 2, e->getRadio() * 2, color,
                                            QPen(Qt::black, 1)), x, y);
            }
            else
            {
                int cuadro = static_cast<int>(animRambo / 0.08f) % CUADROS_RAMBO;
                agregarMovil(new ItemPersonaje(spritesRamboNivel1, LADO_RAMBO, cuadro, anguloRambo,
                                               acechando ? 0.55f : 1.0f), x, y);
            }
        }
        else if (Guardia *g = dynamic_cast<Guardia*>(e))
        {
            if (spritesGuardia.isNull())
            {
                agregarMovil(new ItemFigura(Forma::Elipse, 24, 24, QColor(60, 90, 200)), x, y);
            }
            else
            {
                int cuadro = static_cast<int>(animGuardias / 0.15f) % CUADROS_GUARDIA;
                agregarMovil(new ItemPersonaje(spritesGuardia, LADO_GUARDIA, cuadro,
                                               g->getAnguloVision() * GRADOS_POR_RADIAN), x, y);
            }

            if (g->getDeteccion() > 0.0f) agregarMovil(new ItemGuardia(g->getDeteccion()), x, y);

            if (g->getEstado() == EstadoGuardia::Persiguiendo || g->estaAlarmado())
                agregarMovil(crearImagen(spriteAlerta), x, y - ALTURA_SIMBOLO);
            else if (g->getEstado() == EstadoGuardia::Investigando)
                agregarMovil(crearImagen(spriteDuda), x, y - ALTURA_SIMBOLO);
        }
        else if (dynamic_cast<Extintor*>(e) != nullptr)
        {
            agregarMovil(crearImagen(spriteExtintor), x, y);
        }
        else if (dynamic_cast<Cadaver*>(e) != nullptr)
        {
            const QPixmap &cuerpo = cuerposGuardia[static_cast<int>(x + y) % 2];
            if (cuerpo.isNull()) agregarMovil(new ItemFigura(Forma::Rectangulo, 22, 10, QColor(120, 30, 30)), x, y);
            else agregarMovil(crearImagen(cuerpo), x, y);
        }
        else if (dynamic_cast<NubeHumo*>(e) != nullptr)
        {
            agregarMovil(new ItemFigura(Forma::Elipse, e->getRadio() * 2, e->getRadio() * 2,
                                        QColor(170, 170, 170, 120)), x, y);
        }
        else if (dynamic_cast<OndaRuido*>(e) != nullptr)
        {
            agregarMovil(new ItemFigura(Forma::Elipse, e->getRadio() * 2, e->getRadio() * 2, Qt::NoBrush,
                                        QPen(QColor(255, 255, 255, 120), 2)), x, y);
        }
        else if (dynamic_cast<Piedra*>(e) != nullptr)
        {
            agregarMovil(new ItemFigura(Forma::Elipse, 8, 8, QColor(160, 160, 160)), x, y);
        }
        else if (Proyectil *flecha = dynamic_cast<Proyectil*>(e))
        {
            float grados = std::atan2(flecha->getVelocidadY(), flecha->getVelocidadX()) * GRADOS_POR_RADIAN;
            agregarMovil(new ItemFigura(Forma::Rectangulo, 16, 3, QColor(240, 220, 120)), x, y)
                ->setRotation(grados);
        }
    }

    if (tiempoHerido > 0.0f)
        agregarMovil(new ItemFigura(Forma::Rectangulo, ANCHO_ESCENA, ALTO_ESCENA, QColor(255, 0, 0, 70)),
                     ANCHO_ESCENA / 2, ALTO_ESCENA / 2);

    Rambo *rambo = nivel1->getRambo();
    hud->setText(QString("Salud: %1   Flechas: %2   Piedras: %3   Tiempo: %4   Dificultad: %5")
                     .arg(rambo->getSalud()).arg(rambo->getMunicion()).arg(rambo->getPiedras())
                     .arg(static_cast<int>(nivel1->getTiempoRestante())).arg(nombreDificultad(dificultad)));
}

static QPixmap cuadroEn(const QVector<QPixmap> &v, int i)
{
    if (v.isEmpty()) return QPixmap();
    return v.at(qBound(0, i, v.size() - 1));
}

void GestorJuego::dibujarNivel2()
{
    borrarMovibles();
    Rambo *rambo = nivel2->getRambo();
    QSet<const entidad*> vivos;

    for (entidad *e : nivel2->getEntidades())
    {
        if (!e->estaViva()) continue;
        float x = e->getX();
        float y = e->getY();

        if (dynamic_cast<Rambo*>(e) != nullptr)
        {
            agregarMovil(new ItemRambo(spritesRambo, cuadroRambo(), direccion < 0.0f, ESCALA_RAMBO_N2), x, y);
        }
        else if (Policia *pol = dynamic_cast<Policia*>(e))
        {
            bool haciaDerecha = rambo->getX() >= x;
            float t = pol->getTiempoAtaque();

            vivos.insert(pol);
            ultimosPolicias.insert(pol, QVector4D(x, y, haciaDerecha ? 1.0f : 0.0f, 0.0f));

            if (pol->getTipo() == TipoPolicia::Fusilero && t < DURACION_POSE_DISPARO)
            {
                // el sprite del fusil mira a la derecha
                agregarImagen(spriteFusilero, ESCALA_POLICIA, !haciaDerecha, x, y);
            }
            else if (pol->getTipo() == TipoPolicia::Granadero && t < DURACION_POSE_LANZAR)
            {
                int n = cuadrosLanzar.size();
                int i = qMin(n - 1, static_cast<int>(t / DURACION_POSE_LANZAR * n));
                // los demas sprites miran a la izquierda
                agregarImagen(cuadroEn(cuadrosLanzar, i), ESCALA_POLICIA, haciaDerecha, x, y);
            }
            else
            {
                int n = qMax(1, cuadrosCorrer.size());
                int i = pol->estaCaminando() ? static_cast<int>(tiempoItems / 0.1f) % n : 0;
                agregarImagen(cuadroEn(cuadrosCorrer, i), ESCALA_POLICIA, haciaDerecha, x, y);
            }
        }
        else if (Granada *g = dynamic_cast<Granada*>(e))
        {
            if (g->estaExplotando())
            {
                float progreso = qBound(0.0f, g->getProgresoExplosion(), 0.999f);
                int cuadro = static_cast<int>(progreso * CUADROS_EXPLOSION);
                float anchoCuadro = spriteExplosion.width() / static_cast<float>(CUADROS_EXPLOSION);
                float escala = 2.0f * g->getRadioExplosion() / anchoCuadro;   // diametro = 2 * radio
                // el ayudante pone los pies en y, se sube media altura para centrar la explosion
                agregarCuadro(spriteExplosion, cuadro, CUADROS_EXPLOSION, escala, false,
                              x, y + spriteExplosion.height() * escala / 2.0f);
            }
            else if (!spriteGranada.isNull())
            {
                int cuadro = static_cast<int>(tiempoItems / 0.08f) % CUADROS_GRANADA;
                agregarCuadro(spriteGranada, cuadro, CUADROS_GRANADA, ESCALA_GRANADA, false,
                              x, y + spriteGranada.height() * ESCALA_GRANADA / 2.0f);
            }
            else
            {
                agregarMovil(new ItemFigura(Forma::Elipse, 12, 12, QColor(255, 130, 40)), x, y);
            }
        }
        else if (Proyectil *p = dynamic_cast<Proyectil*>(e))
        {
            float grados = std::atan2(p->getVelocidadY(), p->getVelocidadX()) * GRADOS_POR_RADIAN;
            const QPixmap &sprite = p->esDeTorreta() ? spriteBalaTorreta : spriteBala;
            float escala = p->esDeTorreta() ? ESCALA_BALA_TORRETA : 2.0f;

            if (!sprite.isNull())
            {
                QGraphicsPixmapItem *bala = new QGraphicsPixmapItem(sprite);
                bala->setOffset(-sprite.width() / 2.0, -sprite.height() / 2.0);
                bala->setScale(escala);
                bala->setRotation(grados);
                agregarMovil(bala, x, y);
            }
            else
            {
                QColor color = p->esDeRambo() ? QColor(255, 230, 80) : QColor(255, 130, 40);
                agregarMovil(new ItemFigura(Forma::Elipse, 12, 12, color), x, y);
            }
        }
        else if (Suministro *s = dynamic_cast<Suministro*>(e))
        {
            const QPixmap &sprite = (s->getTipo() == TipoSuministro::Energia) ? spriteEnergia : spriteMunicion;
            if (!sprite.isNull())
            {
                QGraphicsPixmapItem *item = new QGraphicsPixmapItem(sprite);
                item->setOffset(-sprite.width() / 2.0, -sprite.height());   // pies en (x, y)
                item->setScale(ESCALA_ITEM);
                agregarMovil(item, x, y);
            }
            else
            {
                QColor color = (s->getTipo() == TipoSuministro::Energia) ? QColor(80, 220, 120)
                                                                         : QColor(220, 70, 70);
                agregarMovil(new ItemFigura(Forma::Elipse, 20, 20, color), x, y - 10);
            }
        }
        else if (ArmaOscilante *a = dynamic_cast<ArmaOscilante*>(e))
        {
            if (!spriteTorreta.isNull())
            {
                QGraphicsPixmapItem *torreta = new QGraphicsPixmapItem(spriteTorreta);
                torreta->setOffset(-PIVOTE_TORRETA_X, -PIVOTE_TORRETA_Y);
                torreta->setScale(ESCALA_TORRETA);
                torreta->setRotation(a->getAngulo() * GRADOS_POR_RADIAN - 180.0f);
                agregarMovil(torreta, x, y);
            }
        }
    }
    // los policias que ya no estan vivos empiezan su animacion de muerte
    for (auto it = ultimosPolicias.begin(); it != ultimosPolicias.end(); )
    {
        if (vivos.contains(it.key())) { ++it; continue; }
        muertes.append(QVector4D(it.value().x(), it.value().y(), 0.0f, it.value().z()));
        it = ultimosPolicias.erase(it);
    }

    for (int i = muertes.size() - 1; i >= 0; --i)
    {
        muertes[i].setZ(muertes[i].z() + PASO);
        float t = muertes[i].z();
        if (t >= DURACION_CUERPO) { muertes.removeAt(i); continue; }

        int n = cuadrosMuerte.size();
        int cuadro = qMin(n - 1, static_cast<int>(t / DURACION_ANIM_MUERTE * n));
        agregarImagen(cuadroEn(cuadrosMuerte, cuadro), ESCALA_POLICIA,
                      muertes[i].w() > 0.5f, muertes[i].x(), muertes[i].y());
    }
    hud->setText(QString("Salud: %1   Municion: %2   Fatiga: %3   Horda: %4   Dificultad: %5")
                     .arg(rambo->getSalud()).arg(rambo->getMunicion())
                     .arg(static_cast<int>(rambo->getFatiga())).arg(nivel2->getNumeroHorda())
                     .arg(nombreDificultad(dificultad)));
}

int GestorJuego::cuadroRambo() const
{
    Rambo *rambo = nivel2->getRambo();
    if (tiempoHerido > 0.0f) return 5;
    if (!rambo->estaEnSuelo()) return 3;
    if (teclas.contains(Qt::Key_J)) return 6;
    if (teclas.contains(Qt::Key_A) || teclas.contains(Qt::Key_D))
        return static_cast<int>(tiempoAnim / 0.12f) % 4;
    return 0;
}
void GestorJuego::activarCamara()
{
    QGraphicsView *vista = escena->views().value(0);
    if (vista == nullptr) return;

    escena->setSceneRect(0, 0, ANCHO_NIVEL2, ALTO_ESCENA);
    vista->resetTransform();
    vista->scale(ZOOM_NIVEL2, ZOOM_NIVEL2);
    hud->setScale(1.0f);
    camX = nivel2->getRambo()->getX();
}

// la camara solo se mueve cuando rambo se aleja ZONA_CAMARA de su centro
void GestorJuego::seguirConCamara()
{
    QGraphicsView *vista = escena->views().value(0);
    if (vista == nullptr) return;

    float x = nivel2->getRambo()->getX();
    if (x > camX + ZONA_CAMARA) camX = x - ZONA_CAMARA;
    else if (x < camX - ZONA_CAMARA) camX = x + ZONA_CAMARA;

    // el centro de la camara no se sale del fondo
    float mitad = vista->viewport()->width() / vista->transform().m11() / 2.0f;
    camX = qMax(mitad, qMin(camX, ANCHO_NIVEL2 - mitad));

    vista->centerOn(camX, ALTO_ESCENA / 2.0);
    hud->setPos(vista->mapToScene(10, 10));   // el hud queda fijo en la esquina de la pantalla
}

// vuelve a mostrar la escena completa, para el menu, el nivel 1 y los mensajes
void GestorJuego::restablecerCamara()
{
    QGraphicsView *vista = escena->views().value(0);
    if (vista == nullptr) return;

    escena->setSceneRect(0, 0, ANCHO_ESCENA, ALTO_ESCENA);   // el menu y el nivel 1 usan 1280 x 720
    vista->resetTransform();
    vista->centerOn(ANCHO_ESCENA / 2.0, ALTO_ESCENA / 2.0);
}

// dibuja un cuadro de una tira horizontal, con los pies (centro abajo) en (x, y)
void GestorJuego::agregarCuadro(const QPixmap &tira, int cuadro, int cuadros,
                                float escala, bool espejo, float x, float y)
{
    if (tira.isNull()) return;
    float anchoF = tira.width() / static_cast<float>(cuadros);
    int ancho = static_cast<int>(anchoF);
    QPixmap parte = tira.copy(static_cast<int>(cuadro * anchoF), 0, ancho, tira.height());
    if (espejo) parte = parte.transformed(QTransform().scale(-1, 1));
    QGraphicsPixmapItem *item = new QGraphicsPixmapItem(parte);
    item->setOffset(-ancho / 2.0, -parte.height());
    item->setScale(escala);
    agregarMovil(item, x, y);
}

QVector<QPixmap> GestorJuego::cargarCuadros(const QString &prefijo, int cantidad)
{
    QVector<QPixmap> cuadros;
    for (int i = 1; i <= cantidad; ++i)
    {
        QPixmap p(prefijo + QString::number(i) + ".png");
        if (p.isNull()) qWarning("No se pudo cargar %s%d.png", qPrintable(prefijo), i);
        cuadros.append(p);
    }
    return cuadros;
}

// una imagen suelta con los pies (centro abajo) en (x, y)
void GestorJuego::agregarImagen(const QPixmap &imagen, float escala, bool espejo, float x, float y)
{
    if (imagen.isNull()) return;
    QPixmap img = espejo ? imagen.transformed(QTransform().scale(-1, 1)) : imagen;
    QGraphicsPixmapItem *item = new QGraphicsPixmapItem(img);
    item->setOffset(-img.width() / 2.0, -img.height());
    item->setScale(escala);
    agregarMovil(item, x, y);
}
