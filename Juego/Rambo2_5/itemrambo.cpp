#include "itemrambo.h"
#include <QPainter>

static const int ANCHO_CUADRO = 32;
static const int ALTO_CUADRO = 42;
static const float ESCALA = 1.5f;

ItemRambo::ItemRambo(const QPixmap &tira, int cuadro, bool mirandoIzquierda)
    : tira(tira), cuadro(cuadro), mirandoIzquierda(mirandoIzquierda)
{
}

QRectF ItemRambo::boundingRect() const
{
    float ancho = ANCHO_CUADRO * ESCALA;
    float alto = ALTO_CUADRO * ESCALA;
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
