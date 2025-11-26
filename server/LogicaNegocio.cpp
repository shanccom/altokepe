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

  if (tipo == TipoActor::MANAGER_CHEF) {
      //estado = getEstadoParaManager(true);
  } else if (tipo == TipoActor::RANKING) {
      estado = getEstadoParaRanking();
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

QJsonObject LogicaNegocio::getEstadoParaRanking() {
    // Nota: NO usamos lock aquí porque el llamador (enviarEstadoInicial) ya tiene el lock
    
    // 1. Convertir Mapa a Vector para ordenar
    struct ItemRanking {
        QString nombre;
        int cantidad;
    };
    std::vector<ItemRanking> lista;

    for (auto const& [id, cantidad] : m_conteoPlatosRanking) {
        if (m_menu.find(id) != m_menu.end()) {
            lista.push_back({QString::fromStdString(m_menu[id].nombre), cantidad});
        }
    }

    // 2. Ordenar (Mayor a menor cantidad)
    std::sort(lista.begin(), lista.end(), [](const ItemRanking& a, const ItemRanking& b) {
        return a.cantidad > b.cantidad;
    });

    // 3. Construir JSON
    QJsonArray rankingArray;
    for (const auto& item : lista) {
        QJsonObject obj;
        obj["nombre"] = item.nombre;
        obj["cantidad"] = item.cantidad;
        rankingArray.append(obj);
    }

    QJsonObject mensaje;
    mensaje["evento"] = "ACTUALIZACION_RANKING";
    mensaje["data"] = QJsonObject{ {"ranking", rankingArray} };
    
    return mensaje;
}

void LogicaNegocio::registrarVenta(int idPlato) {
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_conteoPlatosRanking[idPlato]++;
    }

    // Notificar a todos (Observer)
    emit enviarRespuesta(nullptr, getEstadoParaRanking());
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
  } else if (comando == "SIMULAR_VENTA") {
    // Comando de DEBUG para probar el ranking sin implementar todo el flujo de pedidos
    int idPlato = mensaje["id_plato"].toInt(1); // Por defecto plato ID 1
    qDebug() << "DEBUG: Simulando venta del plato ID:" << idPlato;
    registrarVenta(idPlato);
  } else {
    qWarning() << "Comando desconocido recibido:" << comando;
    return;
  }
}