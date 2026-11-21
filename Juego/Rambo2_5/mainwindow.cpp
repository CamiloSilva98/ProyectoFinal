#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QKeyEvent>
#include <QMouseEvent>
#include <cstdlib>
#include <ctime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    std::srand(std::time(nullptr));

    setFixedSize(ANCHO_ESCENA, ALTO_ESCENA);

    escena = new QGraphicsScene(this);
    ui->graphicsView->setScene(escena);
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // sin foco para que las teclas lleguen a la ventana y no muevan el scroll de la vista
    ui->graphicsView->setFocusPolicy(Qt::NoFocus);

    gestor = new GestorJuego(escena, this);
}

MainWindow::~MainWindow()
{
    delete ui;
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
