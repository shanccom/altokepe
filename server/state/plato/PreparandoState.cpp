#include "PreparandoState.h"
#include "FinalizadoState.h"
#include "common/models/PlatoInstancia.h"

void PreparandoState::terminar(PlatoInstancia& plato) {
    plato.cambiarEstado(new FinalizadoState());
}
