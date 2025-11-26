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
  : QObject(parent), 
    m_siguienteIdPedido(1), 
    m_siguienteIdInstanciaPlato(1) 
{
    cargarMenu(); 
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
      return;
  }

  if (!estado.isEmpty()) {
      emit enviarRespuesta(cliente, estado);
  }
}

void LogicaNegocio::procesarMensaje(const QJsonObject& mensaje, ManejadorCliente* remitente) {
  QString comando = mensaje[Protocolo::COMANDO].toString();

  if (comando == Protocolo::NUEVO_PEDIDO) {
    procesarNuevoPedido(mensaje, remitente); 
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
    //enviarEstadoInicial(remitente);
  } else {
    qWarning() << "Comando desconocido recibido:" << comando;
    return;
  }
}

void LogicaNegocio::cargarMenu() {
    QString ruta = "menu.json";
    QFile archivo(ruta);

    if (!archivo.open(QIODevice::ReadOnly)) {
        qCritical() << "No se pudo abrir menu.json para cargar el menú.";
        return;
    }

    QByteArray contenido = archivo.readAll();
    archivo.close();

    QJsonDocument doc = QJsonDocument::fromJson(contenido);
    if (!doc.isObject()) {
        qCritical() << "menu.json no tiene un formato JSON válido.";
        return;
    }

    QJsonObject obj = doc.object();
    QJsonArray arr = obj["menu"].toArray();

    for (const auto& item : arr) {
        QJsonObject platoJson = item.toObject();
        PlatoDefinicion p;

        p.id = platoJson["id"].toInt();
        p.nombre = platoJson["nombre"].toString().toStdString();
        p.estacion = platoJson["categoria"].toString().toStdString();
        p.costo = platoJson["precio"].toDouble();
        p.tiempo_preparacion_estimado = platoJson["tiempo_preparacion"].toInt();

        m_menu[p.id] = p;
    }

    qInfo() << "Menú cargado correctamente con" << m_menu.size() << "platos.";
}

void LogicaNegocio::procesarNuevoPedido(const QJsonObject& mensaje, ManejadorCliente* remitente) {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (!mensaje.contains("data") || !mensaje["data"].isObject()) {
        qWarning() << "NUEVO_PEDIDO recibido sin campo data";
        return;
    }

    QJsonObject data = mensaje["data"].toObject();

    if (!data.contains("platos") || !data["platos"].isArray()) {
        qWarning() << "NUEVO_PEDIDO recibido sin lista de platos";
        return;
    }

    long long idPedido = m_siguienteIdPedido++;

    PedidoMesa pedido;
    pedido.id = idPedido;
    pedido.estado = EstadoPedido::PENDIENTE;
    pedido.instancias.clear();

    QJsonArray arr = data["platos"].toArray();

    for (auto item : arr) {
        int idPlato = item.toInt();

        if (m_menu.find(idPlato) == m_menu.end()) {
            qWarning() << "Plato inválido recibido en NUEVO_PEDIDO:" << idPlato;
            continue;
        }

        const PlatoDefinicion& def = m_menu[idPlato];

        InstanciaPlato instancia;
        instancia.id_instancia = m_siguienteIdInstanciaPlato++;
        instancia.id_plato = def.id;
        instancia.estacion = def.estacion;
        instancia.nombre_plato = def.nombre;
        instancia.estado = EstadoPlato::PENDIENTE;

        pedido.instancias.push_back(instancia);

        INFO_PLATO_PRIORIDAD prio(
            instancia.id_instancia,
            def.tiempo_preparacion_estimado
        );

        m_colasPorEstacion[def.estacion].push(prio);
    }

    m_pedidosActivos[idPedido] = pedido;
    m_colaManagerChef.push(idPedido);

    QJsonObject msgRecep;
    msgRecep[Protocolo::EVENTO] = "PEDIDO_REGISTRADO";
    msgRecep["id_pedido"] = (int)idPedido;

    emit enviarRespuesta(remitente, msgRecep);

    for (auto cli : m_manejadoresActivos) {
        if (cli->getTipoActor() == TipoActor::MANAGER_CHEF) {
            emit enviarRespuesta(cli, msgRecep);
        }
    }

    qInfo() << "Pedido" << idPedido << "registrado con" << pedido.instancias.size() << "platos.";
}
