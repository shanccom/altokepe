#include "LogicaNegocio.h"
#include "ManejadorCliente.h"
#include "common/network/Protocolo.h"
#include "common/models/Estados.h"
#include "common/network/SerializadorJSON.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <chrono>
#include <algorithm>

LogicaNegocio* LogicaNegocio::s_instance = nullptr;

LogicaNegocio::LogicaNegocio(QObject* parent) 
  : QObject(parent), m_siguienteIdPedido(1), m_siguienteIdInstanciaPlato(1) {
}

LogicaNegocio* LogicaNegocio::instance() {
  if (s_instance == nullptr) {
    s_instance = new LogicaNegocio();
  }
  return s_instance;
}

void LogicaNegocio::registrarManejador(ManejadorCliente* manejador) {
  std::lock_guard<std::mutex> lock(m_mutex);
  m_manejadoresActivos.push_back(manejador);
}

void LogicaNegocio::eliminarManejador(ManejadorCliente* manejador) {
  std::lock_guard<std::mutex> lock(m_mutex);
  m_manejadoresActivos.erase(std::remove(m_manejadoresActivos.begin(),
        m_manejadoresActivos.end(), manejador), m_manejadoresActivos.end());
}

void LogicaNegocio::enviarEstadoInicial(ManejadorCliente* cliente) {
  std::lock_guard<std::mutex> lock(m_mutex);
  QJsonObject estado;
  TipoActor tipo = cliente->getTipoActor();

  if (tipo == TipoActor::MANAGER_CHEF || tipo == TipoActor::RANKING) {
      //estado = getEstadoParaManagerYRanking(true);
  } else if (tipo == TipoActor::ESTACION_COCINA) {
      //estado = getEstadoParaEstacion(cliente->getNombreEstacion().toStdString());
  } else if (tipo == TipoActor::RECEPCIONISTA) {
      QJsonObject mensaje;
      QJsonArray menuArray;

      for (const auto& par : m_menu) {
      const PlatoDefinicion& plato = par.second;
      QJsonObject platoJson;
      platoJson["id"] = plato.id;
      platoJson["nombre"] = QString::fromStdString(plato.nombre);
      platoJson["precio"] = plato.costo;
      platoJson["tiempo_preparacion"] = plato.tiempo_preparacion_estimado;
      platoJson["categoria"] = QString::fromStdString(plato.estacion);
      menuArray.append(platoJson);
      }

      mensaje[Protocolo::EVENTO] = "ACTUALIZACION_MENU";
      mensaje[Protocolo::DATA] = QJsonObject{ {"menu", menuArray} };

      emit enviarRespuesta(cliente, mensaje);
      return; // Ya enviamos el estado especial para el recepcionista, salimos
  }

  if (!estado.isEmpty()) {
      emit enviarRespuesta(cliente, estado);
  }
}


void LogicaNegocio::procesarMensaje(const QJsonObject& mensaje, ManejadorCliente* remitente) {
  QString comando = mensaje[Protocolo::COMANDO].toString();

  if (comando == Protocolo::NUEVO_PEDIDO) {
    //procesarNuevoPedido(mensaje, remitente);
  } else if (comando == Protocolo::PREPARAR_PEDIDO) {
    //procesarPrepararPedido(mensaje, remitente);
  } else if (comando == Protocolo::CANCELAR_PEDIDO) {
    //procesarCancelarPedido(mensaje, remitente);
  } else if (comando == Protocolo::MARCAR_PLATO_TERMINADO) {
    //procesarMarcarPlatoTerminado(mensaje, remitente);
  } else if (comando == Protocolo::CONFIRMAR_ENTREGA) {
    //procesarConfirmarEntrega(mensaje, remitente);
  } else if (comando == Protocolo::DEVOLVER_PLATO) {
    //procesarDevolverPlato(mensaje, remitente);
  } else if (comando == "SOLICITAR_ESTADO") {
    //enviarEstadoInicial(remitente);  // <<=== NUEVO
  }else{
    qWarning() << "Comando desconocido recibido:" << comando;
    return;
  }
}