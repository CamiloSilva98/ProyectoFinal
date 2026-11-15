#ifndef ENTIDAD_H
#define ENTIDAD_H

class entidad
{
protected:
    float x;
    float y;

    int salud;
    float radio;

public:
    entidad(float x, float y, int salud, float radio = 0.0f);
    virtual ~entidad();
    // el dt se agrega para actualizar por paso de tiempo
    virtual void actualizar(float dt) = 0;
    virtual void recibirDano(int dano);


    float getX() const;
    float getY() const;
    float getRadio() const;
    void setPosicion(float nuevoX, float nuevoY);

    int getSalud() const;

    bool estaViva() const;
};

#endif