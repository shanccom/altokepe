#pragma once
#include "EstadoPedido.h"

class PendienteState : public EstadoPedido {
public:
    QString nombre() const override { return "PENDIENTE"; }

    void preparar(PedidoMesa& pedido) override;
    void cancelar(PedidoMesa& pedido) override;
};
