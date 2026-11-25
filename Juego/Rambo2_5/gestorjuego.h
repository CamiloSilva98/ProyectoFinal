#ifndef GESTORJUEGO_H
#define GESTORJUEGO_H

#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsSimpleTextItem>
#include <QList>
#include <QPixmap>
#include <QSet>
#include <QTimer>
#include "nivel1.h"
#include "nivel2.h"

const int ANCHO_ESCENA = 1280;
const int ALTO_ESCENA = 720;

enum class Pantalla { Menu, JugandoNivel1, JugandoNivel2, Nivel1Completado, Derrota, FinJuego };

class GestorJuego : public QObject
{
    Q_OBJECT

public:
    explicit GestorJuego(QGraphicsScene *escena, QObject *parent = nullptr);
    ~GestorJuego() override;

    void teclaPresionada(int tecla);
    void teclaSoltada(int tecla);
    void clic(QPointF punto, Qt::MouseButton boton);
    void soltarTeclas();

    // solo afecta al nivel 1; si se esta jugando, se reinicia con la nueva
    void setDificultad(Dificultad nueva);
    Dificultad getDificultad() const;
    static QString nombreDificultad(Dificultad dificultad);

private:
    QGraphicsScene *escena;
    QTimer *timer;
    Pantalla pantalla;
    int nivelActual;
    Dificultad dificultad;

    Nivel1 *nivel1;
    Nivel2 *nivel2;

    QSet<int> teclas;
    QList<QGraphicsItem*> movibles;
    QGraphicsSimpleTextItem *hud;

    QPixmap spritesRambo;
    QPixmap spriteEnergia;
    QPixmap spriteBala;
    float direccion;
    float tiempoAnim;
    float tiempoHerido;
    float tiempoItems;
    int saludAnterior;

    void actualizar();
    void actualizarNivel1();
    void actualizarNivel2();

    void mostrarMenu();
    void iniciarNivel1();
    void iniciarNivel2();
    void mostrarMensaje(const QString &titulo, const QString &detalle);
    void descartarNiveles();

    void limpiarEscena();
    void borrarMovibles();
    void agregarFijo(QGraphicsItem *item, float x, float y);
    QGraphicsItem *agregarMovil(QGraphicsItem *item, float x, float y);
    void agregarTexto(const QString &texto, float y, int tamano, const QColor &color);
    void crearHud();

    void dibujarMapaNivel1();
    void dibujarNivel1();
    void dibujarNivel2();
    int cuadroRambo() const;
    void activarCamara();
    void seguirConCamara();
    void restablecerCamara();
};

#endif // GESTORJUEGO_H
