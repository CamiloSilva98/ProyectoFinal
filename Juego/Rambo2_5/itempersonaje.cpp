#include "itempersonaje.h"
#include <QPainter>

ItemPersonaje::ItemPersonaje(const QPixmap &hoja, int lado, int cuadro, float anguloGrados, float opacidad)
    : hoja(hoja), lado(lado), cuadro(cuadro), opacidad(opacidad)
{
    setRotation(anguloGrados);
}

QRectF ItemPersonaje::boundingRect() const
{
    return QRectF(-lado / 2.0, -lado / 2.0, lado, lado);
}

void ItemPersonaje::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setOpacity(opacidad);
    painter->setRenderHint(QPainter::SmoothPixmapTransform);

    QRectF origen(cuadro * lado, 0, lado, lado);
    painter->drawPixmap(boundingRect(), hoja, origen);
}
