#ifndef CADAVER_H
#define CADAVER_H

#include <vector>
#include "entidad.h"

class Cadaver : public entidad
{
private:
    std::vector<const void*> vistoPor;

public:
    Cadaver(float x, float y);

    void actualizar(float dt) override;

    // true solo la primera vez que lo ve ese guardia
    bool marcarVistoPor(const void *guardia);
};

#endif