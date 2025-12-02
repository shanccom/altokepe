#pragma once
#include <QString>

class PlatoInstancia;

class EstadoPlato {
public:
    virtual ~EstadoPlato() {}

    virtual QString nombre() const = 0;

    virtual void iniciar(PlatoInstancia& plato) {}
    virtual void pasarAPreparando(PlatoInstancia& plato) {}
    virtual void terminar(PlatoInstancia& plato) {}
    virtual void entregar(PlatoInstancia& plato) {}
    virtual void devolver(PlatoInstancia& plato) {}
};
