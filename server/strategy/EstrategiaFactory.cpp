#include "EstrategiaFactory.h"
#include "EstrategiaNuevoPedido.h"
#include "EstrategiaPrepararPedido.h"
#include "EstrategiaCancelarPedido.h"
#include "EstrategiaConfirmarEntrega.h"
#include "EstrategiaMarcarPlatoTerminado.h"
#include "EstrategiaDevolverPlato.h"
#include "common/network/Protocolo.h"

IEstrategia* EstrategiaFactory::obtener(const QString& evento) {

    if (evento == Protocolo::NUEVO_PEDIDO)
        return new EstrategiaNuevoPedido();

    if (evento == Protocolo::PREPARAR_PEDIDO)
        return new EstrategiaPrepararPedido();

    if (evento == Protocolo::CANCELAR_PEDIDO)
        return new EstrategiaCancelarPedido();

    if (evento == Protocolo::CONFIRMAR_ENTREGA)
        return new EstrategiaConfirmarEntrega();

    if (evento == Protocolo::PLATO_TERMINADO)
        return new EstrategiaMarcarPlatoTerminado();

    if (evento == Protocolo::DEVOLVER_PLATO)
        return new EstrategiaDevolverPlato();

    return nullptr;
}
