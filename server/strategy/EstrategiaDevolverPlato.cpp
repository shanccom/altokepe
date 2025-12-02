#include "EstrategiaDevolverPlato.h"
#include "LogicaNegocio.h"

void EstrategiaDevolverPlato::ejecutar(LogicaNegocio* logica,
                                       const QJsonObject& mensaje,
                                       ManejadorCliente* remitente) {
    logica->procesarDevolverPlato(mensaje, remitente);
}
