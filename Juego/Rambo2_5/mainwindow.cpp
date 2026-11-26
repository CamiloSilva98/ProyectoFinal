#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QActionGroup>
#include <QKeyEvent>
#include <QMenuBar>
#include <QMouseEvent>
#include <cstdlib>
#include <ctime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    std::srand(std::time(nullptr));

    escena = new QGraphicsScene(this);
    ui->graphicsView->setScene(escena);
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // sin foco para que las teclas lleguen a la ventana y no muevan el scroll de la vista
    ui->graphicsView->setFocusPolicy(Qt::NoFocus);

    gestor = new GestorJuego(escena, this);

    QMenu *menuDificultad = menuBar()->addMenu("Dificultad");
    QActionGroup *grupo = new QActionGroup(this);
    const Dificultad opciones[3] = {Dificultad::Facil, Dificultad::Normal, Dificultad::Dificil};
    for (Dificultad opcion : opciones)
    {
        QAction *accion = new QAction(GestorJuego::nombreDificultad(opcion), this);
        accion->setCheckable(true);
        accion->setChecked(opcion == gestor->getDificultad());
        accion->setData(static_cast<int>(opcion));
        grupo->addAction(accion);
        menuDificultad->addAction(accion);
    }
    connect(grupo, &QActionGroup::triggered, this, &MainWindow::cambiarDificultad);

    // la barra de menu le quita alto a la zona central
    setFixedSize(ANCHO_ESCENA, ALTO_ESCENA + menuBar()->sizeHint().height());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::cambiarDificultad(QAction *accion)
{
    gestor->soltarTeclas();
    gestor->setDificultad(static_cast<Dificultad>(accion->data().toInt()));
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (!event->isAutoRepeat()) gestor->teclaPresionada(event->key());
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (!event->isAutoRepeat()) gestor->teclaSoltada(event->key());
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    QPoint enLaVista = ui->graphicsView->mapFromGlobal(event->globalPos());
    gestor->clic(ui->graphicsView->mapToScene(enLaVista), event->button());
}
