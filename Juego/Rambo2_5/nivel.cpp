#include "nivel.h"

Nivel::Nivel(float tiempoLimite)
    : tiempoRestante(tiempoLimite > 0.0f ? tiempoLimite : 0.0f),
    hayLimiteTiempo(tiempoLimite > 0.0f) {}

Nivel::~Nivel()
{
    for (entidad *e : entidades) delete e;
    entidades.clear();
}

void Nivel::actualizar(float dt)
{
    if (hayLimiteTiempo)
    {
        tiempoRestante -= dt;
        if (tiempoRestante < 0.0f) tiempoRestante = 0.0f;
    }

    // se recorre por indice y con el tamano de antes porque una entidad
    // puede agregar otras (ej. un proyectil) durante su actualizar();
    // las nuevas empiezan a actualizarse en el siguiente paso
    const std::size_t cantidad = entidades.size();
    for (std::size_t i = 0; i < cantidad; ++i)
    {
        if (entidades[i]->estaViva())
            entidades[i]->actualizar(dt);
    }

    eliminarMuertas();
}

void Nivel::eliminarMuertas()
{
    std::size_t j = 0;
    for (std::size_t i = 0; i < entidades.size(); ++i)
    {
        if (entidades[i]->estaViva())
            entidades[j++] = entidades[i];
        else
            delete entidades[i];
    }
    entidades.resize(j);
}

void Nivel::agregarEntidad(entidad *e)
{
    if (e != nullptr) entidades.push_back(e);
}

const std::vector<entidad*> &Nivel::getEntidades() const { return entidades; }
float Nivel::getTiempoRestante() const { return tiempoRestante; }
bool Nivel::tiempoAgotado() const { return hayLimiteTiempo && tiempoRestante <= 0.0f; }
