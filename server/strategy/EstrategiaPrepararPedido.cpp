#include "EstrategiaPrepararPedido.h"
#include "LogicaNegocio.h"

void EstrategiaPrepararPedido::ejecutar(LogicaNegocio* logica,
                                        const QJsonObject& mensaje,
                                        ManejadorCliente* remitente) {
    logica->procesarPrepararPedido(mensaje, remitente);
}
