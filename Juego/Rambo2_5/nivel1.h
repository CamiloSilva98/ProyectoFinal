#ifndef NIVEL1_H
#define NIVEL1_H

#include <vector>
#include "nivel.h"
#include "rambo.h"
#include "modeloarrastre.h"
#include "modelorebote.h"

class AgenteGuardia;
class Cadaver;
class OndaRuido;

enum class TipoObstaculo { Columna, Muro, Carro };

// las columnas usan x, y y radio (centro); muros y carros usan x, y como esquina superior izquierda
struct Obstaculo
{
    TipoObstaculo tipo;
    float x;
    float y;
    float radio;
    float ancho;
    float alto;
};

struct Circulo
{
    float x;
    float y;
    float radio;
};

// parametros de dificultad
struct ConfigNivel1
{
    float tiempo = 180.0f;
    int guardias = 10;
    float alcance = 200.0f;
    float apertura = 1.0f;
    float amplitud = 0.8f;
    float frecuencia = 1.2f;
    int memoria = 8;
    int flechas = 6;
    int piedras = 4;
};

class Nivel1 : public Nivel
{
private:
    float anchoNivel;
    float altoNivel;
    ConfigNivel1 config;

    Rambo *rambo;
    std::vector<AgenteGuardia*> agentes;
    std::vector<Obstaculo> obstaculos;
    std::vector<Circulo> sombras;
    Circulo meta;

    ModeloArrastre modeloFlecha;
    ModeloArrastre modeloPiedra;
    ModeloRebote reboteFlecha;

    bool victoria;
    bool derrota;

    void crearMapa();
    void crearGuardias();
    void retirarGuardiasMuertos();
    void actualizarGuardias(float dt);
    void resolverColisiones();
    bool ramboChoca() const;

public:
    Nivel1(float ancho, float alto, ConfigNivel1 config = ConfigNivel1());
    ~Nivel1() override;

    void iniciar() override;
    void actualizar(float dt) override;
    bool comprobarVictoria() const override;
    bool comprobarDerrota() const override;

    void generarEstimulo(float x, float y, float alcance, float intensidad);

    void moverRambo(float dirX, float dirY, float dt);
    void correrRambo(bool correr);
    void dispararFlecha(float destinoX, float destinoY);
    void lanzarPiedra(float destinoX, float destinoY);

    bool chocaConObstaculo(float px, float py, float radio, float &nx, float &ny, float &penetracion) const;
    bool hayLineaDeVista(float x1, float y1, float x2, float y2) const;
    bool estaEnHumo(float px, float py) const;
    bool estaEnSombra(float px, float py) const;

    std::vector<Cadaver*> getCadaveres() const;
    std::vector<OndaRuido*> getOndas() const;

    Rambo *getRambo() const;
    const std::vector<AgenteGuardia*> &getAgentes() const;
    const std::vector<Obstaculo> &getObstaculos() const;
    const std::vector<Circulo> &getSombras() const;
    const Circulo &getMeta() const;
    float getAncho() const;
    float getAlto() const;
};

#endif
