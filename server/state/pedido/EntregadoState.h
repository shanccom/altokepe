#pragma once
#include "EstadoPedido.h"

class EntregadoState : public EstadoPedido {
public:
    QString nombre() const override { return "ENTREGADO"; }
};
