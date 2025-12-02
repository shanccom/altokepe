#pragma once
#include "EstadoPlato.h"

class FinalizadoState : public EstadoPlato {
public:
    QString nombre() const override { return "FINALIZADO"; }

    void entregar(PlatoInstancia& plato) override;
};
