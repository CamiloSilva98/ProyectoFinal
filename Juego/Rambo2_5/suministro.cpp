#include "suministro.h"
#include "rambo.h"

static const float FATIGA_ENERGIA = 50.0f;

Suministro::Suministro(float x, float y, TipoSuministro tipo, int cantidad)
    : entidad(x, y, 1), tipo(tipo), cantidad(cantidad)
{
}

// el suministro se queda quieto esperando a que lo recojan
void Suministro::actualizar(float dt)
{
    (void)dt;
}

// energia reduce la fatiga y cura, municion suma disparos
void Suministro::aplicarA(Rambo &rambo)
{
    if (tipo == TipoSuministro::Energia)
    {
        rambo.reducirFatiga(FATIGA_ENERGIA);
        rambo.curar(cantidad);   // para la energia, cantidad es la vida que cura
    }
    else
    {
        rambo.agregarMunicion(cantidad);
    }
    salud = 0;
}

TipoSuministro Suministro::getTipo() const { return tipo; }
int Suministro::getCantidad() const { return cantidad; }