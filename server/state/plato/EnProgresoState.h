#pragma once
#include "EstadoPlato.h"

class EnProgresoState : public EstadoPlato {
public:
    QString nombre() const override { return "EN_PROGRESO"; }

    void pasarAPreparando(PlatoInstancia& plato) override;
};
