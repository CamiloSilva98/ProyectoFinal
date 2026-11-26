#ifndef ITEMGUARDIA_H
#define ITEMGUARDIA_H

#include <QGraphicsItem>

// la barra que se llena sobre el guardia mientras ve a Rambo; deteccion va de 0 a 1
class ItemGuardia : public QGraphicsItem
{
public:
    ItemGuardia(float deteccion);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    float deteccion;
};

#endif // ITEMGUARDIA_H
