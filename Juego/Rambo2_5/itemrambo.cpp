#include "itemrambo.h"
#include <QPainter>

static const int ANCHO_CUADRO = 32;
static const int ALTO_CUADRO = 42;

ItemRambo::ItemRambo(const QPixmap &tira, int cuadro, bool mirandoIzquierda, float escala)
    : tira(tira), cuadro(cuadro), mirandoIzquierda(mirandoIzquierda), escala(escala)
{}

QRectF ItemRambo::boundingRect() const
{
    float ancho = ANCHO_CUADRO * escala;
    float alto = ALTO_CUADRO * escala;
    return QRectF(-ancho / 2, -alto, ancho, alto);
}

void ItemRambo::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if (tira.isNull())
    {
        painter->fillRect(boundingRect(), Qt::darkGreen);
        return;
    }

    if (mirandoIzquierda) painter->scale(-1, 1);
    painter->drawPixmap(boundingRect(), tira, QRectF(cuadro * ANCHO_CUADRO, 0, ANCHO_CUADRO, ALTO_CUADRO));
}
