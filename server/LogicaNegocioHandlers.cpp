#include "LogicaNegocioHandlers.h"
#include "LogicaNegocio.h"
#include "ManejadorCliente.h"
#include "common/network/Protocolo.h"
#include "common/models/Estados.h"
#include <QDebug>

OperacionHandler* OperacionHandler::setNext(const Ptr& next) {
  m_next = next;
  return m_next.get();
}

bool OperacionHandler::process(OperacionContext& ctx) {
  if (!handle(ctx)) {
    return false;
  }
  if (m_next) {
    return m_next->process(ctx);
  }
  return true;
}

DataExtractionHandler::DataExtractionHandler(const QString& dataKey)
  : m_key(dataKey) {}

bool DataExtractionHandler::handle(OperacionContext& ctx) {
  if (!ctx.mensaje.contains(m_key) || !ctx.mensaje[m_key].isObject()) {
    qWarning() << "Handler: mensaje sin data válida para key" << m_key;
    return false;
  }
  ctx.data = ctx.mensaje[m_key].toObject();
  return true;
}

bool PedidoLookupHandler::handle(OperacionContext& ctx) {
  return true;
}

bool InstanciaLookupHandler::handle(OperacionContext& ctx) {
  return true;
}

bool EstacionAutorizadaHandler::handle(OperacionContext& ctx) {
  return true;
}
