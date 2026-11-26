#ifndef RAMBO_H
#define RAMBO_H

#include "entidad.h"
#include "suministro.h"

class Suministro;
class ModeloParabolico;
class Rambo : public entidad

{
private:
    int municion;
    float velocidad;
    float fatiga;
    float velocidadY;
    float ySuelo;
    float velocidadSalto;
    bool enSuelo;
    ModeloParabolico *modeloSalto;

    float fatigaMax;
    float tasaFatiga;       // fatiga por segundo de combate
    float costoDisparo;     // fatiga que suma cada disparo
    float intervaloBase;    // segundos entre disparos sin fatiga
    float tiempoDisparo;    // tiempo desde el ultimo disparo
    float factorFatiga;     // entre 0 y 1, cuanto afecta la fatiga

    //para el nivel 1 :D
    int piedras;
    bool corriendo;
    bool acechador;
    bool enCobertura;
    bool movio;
    float tiempoQuieto;
    float tiempoRuido;
    bool ruidoPendiente;

public:
    Rambo(float x, float y, int salud, int municion, float velocidad, float radio = 0.0f);

    void actualizar(float dt) override;
    void mover(float dirX, float dirY, float dt);
    bool disparar();
    void agregarMunicion(int cantidad);
    bool recogerSuministro(Suministro &suministro);
    void reducirFatiga(float cantidad);
    float getFatiga() const;
    void saltar();
    void setModeloSalto(ModeloParabolico *modelo);
    void setSuelo(float yPiso);
    bool estaEnSuelo() const;
    float getVelocidadEfectiva() const;
    float getIntervaloDisparo() const;
    void setParametrosFatiga(float maxima, float tasa, float costo, float factor);
    void setCorriendo(bool valor);
    bool estaCorriendo() const;
    void setEnCobertura(bool valor);
    bool estaAcechando() const;
    bool consumirRuido();
    bool lanzarPiedra();
    void agregarPiedras(int cantidad);
    int getPiedras() const;

    int getMunicion() const;
    float getVelocidad() const;
    void setVelocidad(float v);
};

#endif