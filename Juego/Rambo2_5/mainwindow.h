#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSet>
#include <QTimer>
#include "nivel2.h"
#include <QPixmap>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    Ui::MainWindow *ui;
    Nivel2 *nivel;
    QTimer *timer;
    QSet<int> teclas;
    float direccion;
    QPixmap spritesRambo;
    float tiempoAnim;
    float tiempoHerido;
    int saludAnterior;
    QPixmap spriteEnergia;
    QPixmap spriteBala;
    float tiempoItems;
    int frameRambo() const;
    void actualizarJuego();
};
#endif // MAINWINDOW_H
