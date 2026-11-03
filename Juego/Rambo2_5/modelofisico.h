#ifndef MODELOFISICO_H
#define MODELOFISICO_H

class ModeloFisico
{
public:
    virtual ~ModeloFisico() {}

    // avanza posicion y velocidad un paso dt
    virtual void calcularMovimiento(float &x, float &y, float &vx, float &vy, float dt) = 0;
};

#endif