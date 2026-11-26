#ifndef POLICIA_H
#define POLICIA_H

#include "enemigo.h"
#include "modelofisico.h"
#include "modelorebote.h"

enum class TipoPolicia { Fusilero, Granadero };

class Nivel;

class Policia : public Enemigo
{
private:
    entidad *objetivo;          // no es dueño
    Nivel *nivel;               // no es dueño, aqui se agregan las bombas
    ModeloFisico *modeloBomba;  // no es dueño

    float rangoAtaque;
    float intervaloAtaque;
    float tiempoAtaque;
    float velocidadBombaX;
    float velocidadBombaY;
    int danoBomba;
    TipoPolicia tipo;
    ModeloFisico *modeloBala;     // no es dueño, sin gravedad
    ModeloRebote *reboteGranada;  // no es dueño
    float limiteIzq;
    float limiteDer;
    float gravedadGranada;   // la misma del modelo de la granada, para calcular el angulo
    float velocidadCaida;    // mayor que cero mientras cae
    bool caminando;

public:
    Policia(float x, float y, int salud, float velocidad,
            entidad *objetivo, Nivel *nivel, ModeloFisico *modeloBomba);

    void actualizar(float dt) override;
    void mover(float dt) override;
    void atacar(entidad &blanco) override;

    void setParametrosAtaque(float rango, float intervalo, float velX, float velY, int dano);

    void setTipo(TipoPolicia nuevoTipo);
    TipoPolicia getTipo() const;
    void setModelos(ModeloFisico *bala, ModeloRebote *rebote);
    void setLimites(float izq, float der);
    void setGravedadGranada(float g);
    void caer(float dt, float superficie);
    float getTiempoAtaque() const;
    bool estaCaminando() const;
};

#endif
