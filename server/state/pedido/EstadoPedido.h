#pragma once
#include <QString>

class PedidoMesa;
class ManejadorCliente;

class EstadoPedido {
public:
    virtual ~EstadoPedido() {}

    virtual QString nombre() const = 0;

    virtual void preparar(PedidoMesa& pedido) { }
    virtual void cancelar(PedidoMesa& pedido) { }
    virtual void finalizar(PedidoMesa& pedido) { }
    virtual void entregar(PedidoMesa& pedido) { }
};
