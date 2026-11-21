#include "itemfigura.h"
#include <QPainter>

ItemFigura::ItemFigura(Forma forma, float ancho, float alto, const QBrush &relleno, const QPen &borde)
    : forma(forma), ancho(ancho), alto(alto), relleno(relleno), borde(borde)
{
}

QRectF ItemFigura::boundingRect() const
{
    float mitadBorde = borde.widthF() / 2.0f;
    return QRectF(-ancho / 2 - mitadBorde, -alto / 2 - mitadBorde,
                  ancho + borde.widthF(), alto + borde.widthF());
}

void ItemFigura::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setPen(borde);
    painter->setBrush(relleno);

    QRectF caja(-ancho / 2, -alto / 2, ancho, alto);
    if (forma == Forma::Elipse) painter->drawEllipse(caja);
    else painter->drawRect(caja);
}
