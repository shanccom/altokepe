#include "CommandFactory.h"
#include "ServerCommands.h"
#include "common/network/Protocolo.h"
#include <QDebug>

std::unique_ptr<ICommand> CommandFactory::create(const QJsonObject& mensaje, ManejadorCliente* remitente) {
  QString comando = mensaje[Protocolo::COMANDO].toString();

  if (comando == Protocolo::NUEVO_PEDIDO) {
    return std::make_unique<CmdNuevoPedido>(remitente, mensaje);
  }
  else if (comando == Protocolo::PREPARAR_PEDIDO) {
    return std::make_unique<CmdPrepararPedido>(remitente, mensaje);
  }
  else if (comando == Protocolo::CANCELAR_PEDIDO) {
    return std::make_unique<CmdCancelarPedido>(remitente, mensaje);
  }
  else if (comando == Protocolo::MARCAR_PLATO_TERMINADO) {
    return std::make_unique<CmdMarcarPlatoTerminado>(remitente, mensaje);
  }
  else if (comando == Protocolo::CONFIRMAR_ENTREGA) {
    return std::make_unique<CmdConfirmarEntrega>(remitente, mensaje);
  }
  else if (comando == Protocolo::DEVOLVER_PLATO) {
    return std::make_unique<CmdDevolverPlato>(remitente, mensaje);
  }
  else if (comando == Protocolo::SOLICITAR_ESTADO) {
    return std::make_unique<CmdSolicitarEstado>(remitente, mensaje);
  }
  else if (comando == Protocolo::SIMULAR_VENTA) {
    return std::make_unique<CmdSimularVenta>(remitente, mensaje);
  }

  qWarning() << "CommandFactory: Comando desconocido o no soportado:" << comando;
  return nullptr;
}
