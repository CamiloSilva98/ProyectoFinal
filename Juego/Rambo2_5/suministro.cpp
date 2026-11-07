#include "suministro.h"
#include "rambo.h"

Suministro::Suministro(float x, float y, TipoSuministro tipo, int cantidad)
    : entidad(x, y, 1), tipo(tipo), cantidad(cantidad)
{
}

// el suministro se queda quieto esperando a que lo recojan
void Suministro::actualizar(float dt)
{
    (void)dt;
}

// energia reduce la fatiga, municion suma disparos
void Suministro::aplicarA(Rambo &rambo)
{
    if (tipo == TipoSuministro::Energia)
        rambo.reducirFatiga(cantidad);
    else
        rambo.agregarMunicion(cantidad);

    salud = 0;   // ya fue recogido
}

TipoSuministro Suministro::getTipo() const { return tipo; }
int Suministro::getCantidad() const { return cantidad; }