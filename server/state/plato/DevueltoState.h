#pragma once
#include "EstadoPlato.h"

class DevueltoState : public EstadoPlato {
public:
    QString nombre() const override { return "DEVUELTO"; }

    void iniciar(PlatoInstancia& plato) override;
}