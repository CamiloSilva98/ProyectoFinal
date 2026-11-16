#ifndef AGENTEINTELIGENTE_H
#define AGENTEINTELIGENTE_H

class AgenteInteligente
{
public:
    virtual ~AgenteInteligente() {}
    virtual void actualizar(float dt) = 0;
};

#endif
