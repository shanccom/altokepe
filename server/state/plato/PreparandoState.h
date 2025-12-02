#pragma once
#include "EstadoPlato.h"

class PreparandoState : public EstadoPlato {
public:
    QString nombre() const override { return "PREPARANDO"; }

    void terminar(PlatoInstancia& plato) override;
};
