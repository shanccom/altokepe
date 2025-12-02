#pragma once
#include "EstadoPlato.h"

class EntregadoStatePlato : public EstadoPlato {
public:
    QString nombre() const override { return "ENTREGADO"; }

    void devolver(PlatoInstancia& plato) override;
};
