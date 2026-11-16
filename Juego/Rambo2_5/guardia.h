#ifndef GUARDIA_H
#define GUARDIA_H

#include "enemigo.h"
#include "modelooscilatorio.h"

enum class EstadoGuardia { Patrullando, Investigando, Persiguiendo };

class Guardia : public Enemigo
{
private:
    EstadoGuardia estado;

    float inicioX, inicioY;
    float finX, finY;
    bool haciaFin;
    float destinoX, destinoY;
    float tiempoEspera;
    float tiempoSinVer;

    // el cono barre como una linterna: theta = thetaCentro + A sin(fase)
    float thetaCentro;
    ModeloOscilatorio oscilador;
    float amplitudBase;
    float frecuenciaBase;
    float tiempoAlarma;
    float alcance;
    float apertura;

    float deteccion;

    bool investigacionTerminada;
    bool encontroRambo;
    float investigadoX, investigadoY;

public:
    Guardia(float x, float y, float finX, float finY, float velocidad,
            float alcance, float apertura, float amplitud, float frecuencia);

    void actualizar(float dt) override;
    void mover(float dt) override;
    void atacar(entidad &blanco) override;

    void investigar(float x, float y);
    void perseguir(float x, float y);
    void alarmar();

    void subirDeteccion(float cantidad);
    void bajarDeteccion(float cantidad);

    // devuelve true una sola vez cuando termino una investigacion
    bool consumirInvestigacion(float &x, float &y, bool &encontro);

    EstadoGuardia getEstado() const;
    float getAnguloVision() const;
    float getAlcance() const;
    float getApertura() const;
    float getDeteccion() const;
    bool estaAlarmado() const;
    float getAmplitud() const;
    float getFrecuencia() const;
};

#endif
