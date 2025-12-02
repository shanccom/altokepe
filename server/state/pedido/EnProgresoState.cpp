#include "EnProgresoState.h"
#include "ListoState.h"
#include "CanceladoState.h"
#include "common/models/PedidoMesa.h"

void EnProgresoState::finalizar(PedidoMesa& pedido) {
    pedido.cambiarEstado(new ListoState());
}

void EnProgresoState::cancelar(PedidoMesa& pedido) {
    pedido.cambiarEstado(new CanceladoState());
}
