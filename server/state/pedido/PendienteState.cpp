#include "PendienteState.h"
#include "EnProgresoState.h"
#include "CanceladoState.h"
#include "common/models/PedidoMesa.h"

void PendienteState::preparar(PedidoMesa& pedido) {
    pedido.cambiarEstado(new EnProgresoState());
}

void PendienteState::cancelar(PedidoMesa& pedido) {
    pedido.cambiarEstado(new CanceladoState());
}
