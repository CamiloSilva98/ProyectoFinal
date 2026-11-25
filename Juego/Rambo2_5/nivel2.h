#ifndef NIVEL2_H
#define NIVEL2_H

#include <vector>
#include "nivel.h"
#include "rambo.h"
#include "armaoscilante.h"
#include "policia.h"
#include "modeloparabolico.h"
#include "modelorebote.h"
#include "modelooscilatorio.h"
#include "granada.h"


// base donde se puede pararse: borde izquierdo, borde derecho y altura de la superficie
struct Plataforma
{
    float x0;
    float x1;
    float y;
};

// lugar por donde aparece un policia y hasta donde puede caminar en su base
struct PuntoAparicion
{
    float x;
    float y;
    float izq;
    float der;
};

// parametros de dificultad de una horda
struct ConfigHorda
{
    float duracion;          // segundos en que pueden aparecer policias
    float intervaloSpawn;    // segundos entre apariciones
    int maxEnPantalla;       // policias vivos al mismo tiempo
    int saludPolicia;
    float velocidadPolicia;
    float intervaloAtaque;   // segundos entre bombas
};

class Nivel2 : public Nivel
{
private:
    // mapa
    float anchoNivel;
    float altoNivel;
    float ySuelo;

    // los posee Nivel, aqui solo se apuntan
    Rambo *rambo;
    ArmaOscilante *arma;

    // modelos fisicos, los posee el nivel
    ModeloParabolico modeloSalto;
    ModeloParabolico modeloBala;
    ModeloParabolico modeloBomba;
    ModeloRebote reboteBala;
    ModeloRebote reboteGranada;
    ModeloOscilatorio oscilador;

    // hordas
    std::vector<ConfigHorda> hordas;
    std::vector<Plataforma> plataformas;
    std::vector<PuntoAparicion> puntos;
    int numeroHorda;       // empieza en 1
    float tiempoHorda;
    float tiempoSpawn;
    bool generando;
    bool ladoSpawn;        // alterna izquierda y derecha

    // arma fija
    float intervaloArma;
    float tiempoArma;

    int probSuministro;    // porcentaje de soltar suministro al morir un policia

    bool victoria;
    bool derrota;

    int contarPolicias() const;
    void generarPolicia();
    void soltarSuministro(float px, float py);
    void resolverColisiones();
    float superficieBajo(float px, float py) const;
    void resolverPlataformas(float dt);
    void limitesDeBase(float px, float superficie, float &izq, float &der) const;

public:
    Nivel2(float ancho, float alto, float ySuelo);

    void iniciar() override;
    void actualizar(float dt) override;
    bool comprobarVictoria() const override;
    bool comprobarDerrota() const override;

    void iniciarHorda();
    void actualizarHorda(float dt);
    bool comprobarHordaTerminada() const;

    // controles que usara la interfaz
    void moverRambo(float dir, float dt);   // dir: -1 izquierda, 1 derecha
    void saltarRambo();
    void dispararRambo(float dir);

    Rambo *getRambo() const;
    int getNumeroHorda() const;
    float getAncho() const;
    float getAlto() const;
    float getYSuelo() const;
    const std::vector<Plataforma> &getPlataformas() const;
};

#endif
