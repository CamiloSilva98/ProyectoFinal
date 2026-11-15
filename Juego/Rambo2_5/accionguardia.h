#ifndef ACCIONGUARDIA_H
#define ACCIONGUARDIA_H

#include "decision.h"
#include "guardia.h"

class AccionGuardia
{
public:
    void ejecutarDecision(const Decision &decision, Guardia &guardia);
};

#endif
