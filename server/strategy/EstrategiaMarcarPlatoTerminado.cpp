#include "EstrategiaMarcarPlatoTerminado.h"
#include "LogicaNegocio.h"

void EstrategiaMarcarPlatoTerminado::ejecutar(LogicaNegocio* logica,
                                              const QJsonObject& mensaje,
                                              ManejadorCliente* remitente) {
    logica->procesarMarcarPlatoTerminado(mensaje, remitente);
}
