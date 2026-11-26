#ifndef ITEMPERSONAJE_H
#define ITEMPERSONAJE_H

#include <QGraphicsItem>
#include <QPixmap>

// un cuadro cuadrado de una hoja de sprites vista desde arriba; el dibujo mira a la derecha y el item lo gira
class ItemPersonaje : public QGraphicsItem
{
public:
    ItemPersonaje(const QPixmap &hoja, int lado, int cuadro, float anguloGrados, float opacidad = 1.0f);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    QPixmap hoja;
    int lado;
    int cuadro;
    float opacidad;
};

#endif // ITEMPERSONAJE_H
