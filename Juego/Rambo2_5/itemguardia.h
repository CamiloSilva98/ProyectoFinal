#ifndef ITEMGUARDIA_H
#define ITEMGUARDIA_H

#include <QGraphicsItem>
#include <QColor>

// el guardia con su cono de vision; el angulo y la apertura vienen en radianes
class ItemGuardia : public QGraphicsItem
{
public:
    ItemGuardia(float alcance, float apertura, float angulo, float deteccion, const QColor &colorCono);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    float alcance;
    float apertura;
    float angulo;
    float deteccion;
    QColor colorCono;
};

#endif // ITEMGUARDIA_H