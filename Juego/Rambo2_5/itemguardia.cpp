#include "itemguardia.h"
#include <QPainter>

static const float MEDIO_ANCHO_BARRA = 12.0f;
static const float ALTURA_BARRA = 26.0f;
static const float GROSOR_BARRA = 4.0f;

ItemGuardia::ItemGuardia(float deteccion)
    : deteccion(deteccion)
{
}

QRectF ItemGuardia::boundingRect() const
{
    return QRectF(-MEDIO_ANCHO_BARRA, -ALTURA_BARRA, 2 * MEDIO_ANCHO_BARRA, GROSOR_BARRA);
}

void ItemGuardia::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::red);
    painter->drawRect(QRectF(-MEDIO_ANCHO_BARRA, -ALTURA_BARRA, 2 * MEDIO_ANCHO_BARRA * deteccion, GROSOR_BARRA));
}
