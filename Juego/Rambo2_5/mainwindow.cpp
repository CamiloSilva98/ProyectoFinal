#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QKeyEvent>
#include <QPaintEvent>
#include <cstdlib>
#include <ctime>
#include "suministro.h"
#include "nivel2.h"
#include <QTransform>
#include <QDirIterator>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    std::srand(std::time(nullptr));

    setFixedSize(1500, 900);

    nivel = new Nivel2(1280, 720, 600);
    nivel->iniciar();
    direccion = 1.0f;

    spritesRambo.load(":/sprites/recursos/sprites/rambo_sprites.png");
    spriteEnergia.load(":/sprites/recursos/sprites/items.png");
    spriteBala.load(":/sprites/recursos/sprites/bala.png");
    tiempoItems = 0.0f;

    tiempoAnim = 0.0f;
    tiempoHerido = 0.0f;
    saludAnterior = nivel->getRambo()->getSalud();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::actualizarJuego);
    timer->start(16);
}

MainWindow::~MainWindow()
{
    delete nivel;
    delete ui;
}

// un paso del juego: lee las teclas, avanza el nivel y redibuja
void MainWindow::actualizarJuego()
{
    const float dt = 0.016f;

    if (teclas.contains(Qt::Key_A)) { direccion = -1.0f; nivel->moverRambo(-1.0f, dt); }
    if (teclas.contains(Qt::Key_D)) { direccion = 1.0f; nivel->moverRambo(1.0f, dt); }
    if (teclas.contains(Qt::Key_Space)) nivel->saltarRambo();
    if (teclas.contains(Qt::Key_J)) nivel->dispararRambo(direccion);

    nivel->actualizar(dt);
    tiempoItems += dt;
    // la animacion avanza mientras camina, y un golpe activa el cuadro de herido un momento
    bool camina = teclas.contains(Qt::Key_A) || teclas.contains(Qt::Key_D);
    if (camina) tiempoAnim += dt; else tiempoAnim = 0.0f;

    int saludActual = nivel->getRambo()->getSalud();
    if (saludActual < saludAnterior) tiempoHerido = 0.25f;
    saludAnterior = saludActual;
    if (tiempoHerido > 0.0f) tiempoHerido -= dt;
    update();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (!event->isAutoRepeat()) teclas.insert(event->key());
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (!event->isAutoRepeat()) teclas.remove(event->key());
}

// dibujo temporal con figuras simples, mientras llegan los sprites
void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.fillRect(rect(), QColor(30, 30, 40));

    int suelo = static_cast<int>(nivel->getYSuelo());
    p.setPen(Qt::white);
    p.drawLine(0, suelo, width(), suelo);

    for (entidad *e : nivel->getEntidades())
    {
        if (!e->estaViva()) continue;
        int x = static_cast<int>(e->getX());
        int y = static_cast<int>(e->getY());

        if (dynamic_cast<Rambo*>(e) != nullptr)
        {
            // se recorta el cuadro de la tira y se voltea si mira a la izquierda
            const int CW = 32, CH = 42;
            const float ESCALA = 1.5f;
            QPixmap cuadro = spritesRambo.copy(frameRambo() * CW, 0, CW, CH);
            if (direccion < 0.0f) cuadro = cuadro.transformed(QTransform().scale(-1, 1));
            int dw = static_cast<int>(CW * ESCALA);
            int dh = static_cast<int>(CH * ESCALA);
            p.drawPixmap(x - dw / 2, y - dh, dw, dh, cuadro);
        }
        else if (dynamic_cast<Policia*>(e) != nullptr)
        {
            p.fillRect(x - 15, y - 60, 30, 60, QColor(70, 110, 220));
        } else if (Proyectil *pr = dynamic_cast<Proyectil*>(e)) {
            if (pr->esDeRambo() && !spriteBala.isNull()) {
                // la bala se gira segun hacia donde va
                float ang = std::atan2(pr->getVelocidadY(), pr->getVelocidadX()) * 180.0f / 3.14159265f;
                int bw = spriteBala.width() * 2;
                int bh = spriteBala.height() * 2;
                p.save();
                p.translate(x, y);
                p.rotate(ang);
                p.drawPixmap(-bw / 2, -bh / 2, bw, bh, spriteBala);
                p.restore();
            } else {
                p.setBrush(pr->esDeRambo() ? QColor(255, 230, 80) : QColor(255, 130, 40));
                p.setPen(Qt::NoPen);
                p.drawEllipse(QPoint(x, y), 6, 6);
            }
        }
        else if (Suministro *s = dynamic_cast<Suministro*>(e)) {
            if (s->getTipo() == TipoSuministro::Energia && !spriteEnergia.isNull()) {
                // si la imagen es una tira de varios cuadros se anima, con 1 se dibuja fija
                const int CUADROS = 1;
                int fw = spriteEnergia.width() / CUADROS;
                int fh = spriteEnergia.height();
                int cuadro = static_cast<int>(tiempoItems / 0.1f) % CUADROS;
                int dh = 30;
                int dw = static_cast<int>(fw * (dh / static_cast<float>(fh)));
                p.drawPixmap(x - dw / 2, y - dh, dw, dh, spriteEnergia.copy(cuadro * fw, 0, fw, fh));
            } else {
                p.setBrush(s->getTipo() == TipoSuministro::Energia ? QColor(80, 220, 120) : QColor(220, 70, 70));
                p.setPen(Qt::NoPen);
                p.drawEllipse(QPoint(x, y - 10), 10, 10);
            }
        }
        else if (ArmaOscilante *a = dynamic_cast<ArmaOscilante*>(e))
        {
            p.setBrush(Qt::gray);
            p.setPen(QPen(Qt::gray, 4));
            p.drawEllipse(QPoint(x, y), 14, 14);
            p.drawLine(x, y, x + static_cast<int>(40 * std::cos(a->getAngulo())),
                       y + static_cast<int>(40 * std::sin(a->getAngulo())));
        }
    }

    // datos en pantalla
    Rambo *r = nivel->getRambo();
    p.setPen(Qt::white);
    p.drawText(10, 20, QString("Salud: %1   Municion: %2   Fatiga: %3   Horda: %4")
                           .arg(r->getSalud()).arg(r->getMunicion())
                           .arg(static_cast<int>(r->getFatiga())).arg(nivel->getNumeroHorda()));
    if (nivel->comprobarVictoria()) p.drawText(width() / 2 - 30, 60, "VICTORIA");
    if (nivel->comprobarDerrota()) p.drawText(width() / 2 - 30, 60, "DERROTA");
}

// decide que cuadro de la tira se dibuja segun lo que esta haciendo rambo
int MainWindow::frameRambo() const
{
    Rambo *r = nivel->getRambo();
    if (tiempoHerido > 0.0f) return 5;
    if (!r->estaEnSuelo()) return 3;
    if (teclas.contains(Qt::Key_J)) return 6;
    if (teclas.contains(Qt::Key_A) || teclas.contains(Qt::Key_D))
        return static_cast<int>(tiempoAnim / 0.12f) % 4;
    return 0;
}