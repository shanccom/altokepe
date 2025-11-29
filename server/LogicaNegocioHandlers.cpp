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
  if (!ctx.data.contains("id_pedido")) {
    qWarning() << "Handler: data sin id_pedido";
    return false;
  }

  long long idPedido = ctx.data["id_pedido"].toInt();
  PedidoMesa* pedido = ctx.logica->obtenerPedido(idPedido);
  if (!pedido) {
    qWarning() << "Handler: pedido no encontrado" << idPedido;
    return false;
  }
  ctx.pedido = pedido;
  return true;
}

bool InstanciaLookupHandler::handle(OperacionContext& ctx) {
  if (!ctx.data.contains("id_instancia")) {
    qWarning() << "Handler: data sin id_instancia";
    return false;
  }

  if (!ctx.pedido) {
    qWarning() << "Handler: no hay pedido en contexto";
    return false;
  }

  long long idInstancia = ctx.data["id_instancia"].toInt();
  PlatoInstancia* instancia = ctx.logica->obtenerInstancia(*ctx.pedido, idInstancia);
  if (!instancia) {
    qWarning() << "Handler: instancia no encontrada en pedido" << idInstancia;
    return false;
  }
  ctx.instancia = instancia;

  ctx.definicion = ctx.logica->obtenerDefinicionPlato(instancia->id_plato_definicion);
  return true;
}

bool EstacionAutorizadaHandler::handle(OperacionContext& ctx) {
  if (!ctx.definicion || !ctx.remitente) {
    return false;
  }

  if (ctx.remitente->getTipoActor() != TipoActor::ESTACION_COCINA) {
    return true;
  }

  if (ctx.definicion->estacion != ctx.remitente->getNombreEstacion().toStdString()) {
    qWarning() << "Handler: estación no autorizada para la instancia";
    return false;
  }
  return true;
}
