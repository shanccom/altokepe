#pragma once
#include "EstadoPlato.h"

class EnEsperaState : public EstadoPlato {
public:
    QString nombre() const override { return "EN_ESPERA"; }

    void iniciar(PlatoInstancia& plato) override;
};
