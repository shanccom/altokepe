#include "EstrategiaNuevoPedido.h"
#include "LogicaNegocio.h"

void EstrategiaNuevoPedido::ejecutar(LogicaNegocio* logica,
                                     const QJsonObject& mensaje,
                                     ManejadorCliente* remitente) {
    logica->procesarNuevoPedido(mensaje, remitente);
}
