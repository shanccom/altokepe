#include "DevueltoState.h"
#include "EnProgresoState.h"
#include "common/models/PlatoInstancia.h"

void DevueltoState::iniciar(PlatoInstancia& plato) {
    plato.cambiarEstado(new EnProgresoState());
}
