#pragma once
#include "EstadoPedido.h"

class ListoState : public EstadoPedido {
public:
    QString nombre() const override { return "LISTO"; }

    void entregar(PedidoMesa& pedido) override;
};
