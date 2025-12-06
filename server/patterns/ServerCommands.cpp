#include "ServerCommands.h"
#include "../LogicaNegocio.h"
#include "../ManejadorCliente.h"
#include <QDebug>

BaseServerCommand::BaseServerCommand(ManejadorCliente* remitente, const QJsonObject& payload)
    : m_remitente(remitente), m_payload(payload) {}

void CmdNuevoPedido::execute() {
  LogicaNegocio::instance()->procesarNuevoPedido(m_payload, m_remitente);
}

void CmdPrepararPedido::execute() {
  LogicaNegocio::instance()->procesarPrepararPedido(m_payload, m_remitente);
}

void CmdCancelarPedido::execute() {
  LogicaNegocio::instance()->procesarCancelarPedido(m_payload, m_remitente);
}

void CmdMarcarPlatoTerminado::execute() {
  LogicaNegocio::instance()->procesarMarcarPlatoTerminado(m_payload, m_remitente);
}

void CmdConfirmarEntrega::execute() {
  LogicaNegocio::instance()->procesarConfirmarEntrega(m_payload, m_remitente);
}

void CmdDevolverPlato::execute() {
  LogicaNegocio::instance()->procesarDevolverPlato(m_payload, m_remitente);
}

void CmdSolicitarEstado::execute() {
  // Llama directamente al envío de estado inicial existente
  LogicaNegocio::instance()->enviarEstadoInicial(m_remitente);
}

void CmdSimularVenta::execute() {
  int idPlato = m_payload["id_plato"].toInt(1);
  qDebug() << "CMD: Simulando venta del plato ID:" << idPlato;
  LogicaNegocio::instance()->registrarVenta(idPlato);
}
