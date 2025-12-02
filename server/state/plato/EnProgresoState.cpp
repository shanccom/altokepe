#include "EnProgresoState.h"
#include "PreparandoState.h"
#include "common/models/PlatoInstancia.h"

void EnProgresoState::pasarAPreparando(PlatoInstancia& plato) {
    plato.cambiarEstado(new PreparandoState());
}
