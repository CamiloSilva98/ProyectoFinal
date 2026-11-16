#ifndef ESTIMULO_H
#define ESTIMULO_H

enum class TipoEstimulo { Ruido, Visual, Cadaver };

struct Estimulo
{
    TipoEstimulo tipo;
    float x;
    float y;
    float intensidad;
};

#endif
