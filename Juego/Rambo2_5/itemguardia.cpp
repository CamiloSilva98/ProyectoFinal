#include "itemguardia.h"
#include <QPainter>

static const float RADIO_CUERPO = 12.0f;
static const float GRADOS_POR_RADIAN = 57.29578f;

ItemGuardia::ItemGuardia(float alcance, float apertura, float angulo, float deteccion, const QColor &colorCono)
    : alcance(alcance), apertura(apertura), angulo(angulo), deteccion(deteccion), colorCono(colorCono)
{
}

QRectF ItemGuardia::boundingRect() const
{
    return QRectF(-alcance, -alcance, 2 * alcance, 2 * alcance);
}

void ItemGuardia::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    // drawPie mide los angulos al reves que el juego (antihorario, en 1/16 de grado)
    // porque en la escena la y crece hacia abajo, por eso el angulo va con signo negativo
    int inicio = static_cast<int>(-(angulo + apertura / 2) * GRADOS_POR_RADIAN * 16);
    int recorrido = static_cast<int>(apertura * GRADOS_POR_RADIAN * 16);

    painter->setPen(Qt::NoPen);
    painter->setBrush(colorCono);
    painter->drawPie(boundingRect(), inicio, recorrido);

    painter->setBrush(QColor(60, 90, 200));
    painter->drawEllipse(QPointF(0, 0), RADIO_CUERPO, RADIO_CUERPO);

    if (deteccion > 0.0f)
    {
        painter->setBrush(Qt::red);
        painter->drawRect(QRectF(-RADIO_CUERPO, -RADIO_CUERPO - 14, 2 * RADIO_CUERPO * deteccion, 4));
    }
}
