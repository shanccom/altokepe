#include "EstrategiaCancelarPedido.h"
#include "LogicaNegocio.h"

void EstrategiaCancelarPedido::ejecutar(LogicaNegocio* logica,
                                        const QJsonObject& mensaje,
                                        ManejadorCliente* remitente) {
    logica->procesarCancelarPedido(mensaje, remitente);
}
