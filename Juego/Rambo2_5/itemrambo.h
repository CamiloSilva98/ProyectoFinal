#ifndef ITEMRAMBO_H
#define ITEMRAMBO_H

#include <QGraphicsItem>
#include <QPixmap>

// dibuja un cuadro de la tira de sprites de rambo, con los pies en la posicion del item
class ItemRambo : public QGraphicsItem
{
public:
    ItemRambo(const QPixmap &tira, int cuadro, bool mirandoIzquierda, float escala = 1.5f);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    QPixmap tira;
    int cuadro;
    bool mirandoIzquierda;
    float escala;
};

#endif // ITEMRAMBO_H
