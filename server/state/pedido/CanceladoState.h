#pragma once
#include "EstadoPedido.h"

class CanceladoState : public EstadoPedido {
public:
    QString nombre() const override { return "CANCELADO"; }
};
