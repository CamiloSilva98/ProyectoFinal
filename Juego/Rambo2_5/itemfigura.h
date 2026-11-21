#ifndef ITEMFIGURA_H
#define ITEMFIGURA_H

#include <QGraphicsItem>
#include <QBrush>
#include <QPen>

enum class Forma { Rectangulo, Elipse };

// figura simple centrada en su posicion, sirve para casi todo lo que se dibuja con formas
class ItemFigura : public QGraphicsItem
{
public:
    ItemFigura(Forma forma, float ancho, float alto, const QBrush &relleno,
               const QPen &borde = QPen(Qt::NoPen));

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    Forma forma;
    float ancho;
    float alto;
    QBrush relleno;
    QPen borde;
};

#endif // ITEMFIGURA_H
