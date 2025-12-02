#include "FinalizadoState.h"
#include "EntregadoStatePlato.h"
#include "common/models/PlatoInstancia.h"

void FinalizadoState::entregar(PlatoInstancia& plato) {
    plato.cambiarEstado(new EntregadoStatePlato());
}
