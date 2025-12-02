#include "EntregadoStatePlato.h"
#include "DevueltoState.h"
#include "common/models/PlatoInstancia.h"

void EntregadoStatePlato::devolver(PlatoInstancia& plato) {
    plato.cambiarEstado(new DevueltoState());
}
