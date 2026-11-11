#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QKeyEvent>
#include <QPaintEvent>
#include <cstdlib>
#include <ctime>
#include "suministro.h"
#include "nivel2.h"

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
    if (teclas.contains(Qt::Key_W) || teclas.contains(Qt::Key_Space)) nivel->saltarRambo();
    if (teclas.contains(Qt::Key_J)) nivel->dispararRambo(direccion);

    nivel->actualizar(dt);
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
            p.fillRect(x - 15, y - 60, 30, 60, QColor(60, 180, 75));
        }
        else if (dynamic_cast<Policia*>(e) != nullptr)
        {
            p.fillRect(x - 15, y - 60, 30, 60, QColor(70, 110, 220));
        }
        else if (Proyectil *pr = dynamic_cast<Proyectil*>(e))
        {
            p.setBrush(pr->esDeRambo() ? QColor(255, 230, 80) : QColor(255, 130, 40));
            p.setPen(Qt::NoPen);
            p.drawEllipse(QPoint(x, y), 6, 6);
        }
        else if (Suministro *s = dynamic_cast<Suministro*>(e))
        {
            p.setBrush(s->getTipo() == TipoSuministro::Energia ? QColor(80, 220, 120) : QColor(220, 70, 70));
            p.setPen(Qt::NoPen);
            p.drawEllipse(QPoint(x, y - 10), 10, 10);
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