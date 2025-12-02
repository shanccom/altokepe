#include "ListoState.h"
#include "EntregadoState.h"
#include "common/models/PedidoMesa.h"

void ListoState::entregar(PedidoMesa& pedido) {
    pedido.cambiarEstado(new EntregadoState());
}
