#include "EnEsperaState.h"
#include "EnProgresoState.h"
#include "common/models/PlatoInstancia.h"

void EnEsperaState::iniciar(PlatoInstancia& plato) {
    plato.cambiarEstado(new EnProgresoState());
}
