#include "EstrategiaConfirmarEntrega.h"
#include "LogicaNegocio.h"

void EstrategiaConfirmarEntrega::ejecutar(LogicaNegocio* logica,
                                          const QJsonObject& mensaje,
                                          ManejadorCliente* remitente) {
    logica->procesarConfirmarEntrega(mensaje, remitente);
}
