#ifndef NIVEL_H
#define NIVEL_H

#include <vector>
#include "entidad.h"

// clase base abstracta de los niveles (Nivel1 y Nivel2 heredan de aqui)
class Nivel
{
protected:
    float tiempoRestante;   // segundos, solo cuenta si hay limite
    bool hayLimiteTiempo;
    std::vector<entidad*> entidades;   // el nivel es dueno de estas entidades

    // borra de memoria las entidades que ya no estan vivas
    void eliminarMuertas();

public:
    // tiempoLimite <= 0 significa que el nivel no se controla por tiempo
    explicit Nivel(float tiempoLimite = 0.0f);
    virtual ~Nivel();

    // un nivel tiene punteros, no se debe copiar
    Nivel(const Nivel &) = delete;
    Nivel &operator=(const Nivel &) = delete;

    // arma el escenario y deja el nivel listo para jugar
    virtual void iniciar() = 0;

    // avanza el nivel un paso dt: descuenta tiempo, actualiza entidades
    // y limpia las muertas. Las subclases la llaman y agregan su logica.
    virtual void actualizar(float dt);

    virtual bool comprobarVictoria() const = 0;
    virtual bool comprobarDerrota() const = 0;

    // el nivel pasa a ser dueno de e (se borra al destruir el nivel)
    void agregarEntidad(entidad *e);

    const std::vector<entidad*> &getEntidades() const;
    float getTiempoRestante() const;
    bool tiempoAgotado() const;
};

#endif
