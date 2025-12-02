#pragma once
#include "EstadoPedido.h"

class EnProgresoState : public EstadoPedido {
public:
    QString nombre() const override { return "EN_PROGRESO"; }

    void finalizar(PedidoMesa& pedido) override;
    void cancelar(PedidoMesa& pedido) override;
};
