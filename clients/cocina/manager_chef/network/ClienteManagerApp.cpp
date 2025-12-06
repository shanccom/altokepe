#include "ClienteManagerApp.h"
#include "../ui/VentanaManager.h"
#include "../network/ClienteTCP.h"
#include "common/network/Protocolo.h"
#include "common/network/SerializadorJSON.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

ClienteManagerApp::ClienteManagerApp(QObject* parent) : QObject(parent) {
  m_ventana = new VentanaManager();
  m_clienteTCP = new ClienteTCP(this);

  // Conexion de Red
  connect(m_clienteTCP, &ClienteTCP::conectado, this, &ClienteManagerApp::onConectado);
  connect(m_clienteTCP, &ClienteTCP::nuevoMensajeRecibido, this, &ClienteManagerApp::onMensajeRecibido);

  // Conexiones de Acción (UI a Lógica)
  connect(m_ventana, &VentanaManager::prepararPedidoSolicitado, this, &ClienteManagerApp::onPrepararPedido);
  connect(m_ventana, &VentanaManager::cancelarPedidoSolicitado, this, &ClienteManagerApp::onCancelarPedido);
  connect(m_ventana, &VentanaManager::enviarPedidoSolicitado, this, &ClienteManagerApp::onEnviarPedido);
  connect(m_ventana, &VentanaManager::rechazarPedidoSolicitado, this, &ClienteManagerApp::onRechazarPedido);
  connect(m_ventana, &VentanaManager::rechazarPlatoSolicitado, this, &ClienteManagerApp::onRechazarPlato);
}

void ClienteManagerApp::iniciar() {
  m_clienteTCP->conectar("127.0.0.1", 5555); // Conectar a localhost
  m_ventana->show();
}

void ClienteManagerApp::onConectado() {
  qDebug() << "Conexión exitosa. Identificándose como ManagerChef...";
  QJsonObject identificacion;
  identificacion[Protocolo::COMANDO] = Protocolo::IDENTIFICARSE;
  identificacion["rol"] = "ManagerChef";
  // El Manager Chef no necesita un ID específico, el rol es suficiente.

  m_clienteTCP->enviarMensaje(identificacion);
}

void ClienteManagerApp::onMensajeRecibido(const QJsonObject& mensaje) {
  QString evento = mensaje[Protocolo::EVENTO].toString();
  QJsonObject data = mensaje[Protocolo::DATA].toObject();

  qDebug() << "Evento recibido:" << evento;

  if (evento == Protocolo::ACTUALIZACION_ESTADO_GENERAL) {
    if (data.contains("menu")) {
      m_menu.clear();
      QJsonArray menuArray = data["menu"].toArray();
      for (const QJsonValue& val : menuArray) {
        PlatoDefinicion plato = SerializadorJSON::jsonToPlatoDefinicion(val.toObject());
        m_menu[plato.id] = plato;
      }
      qDebug() << "Menú inicial recibido con" << m_menu.size() << "platos.";
    }

    auto deserializarPedidos = [this](const QJsonObject& data, const QString& clave) {
      std::vector<PedidoMesa> pedidos;
      QJsonArray pedidosJson = data[clave].toArray();
      for (const QJsonValue& val : pedidosJson) {
        pedidos.push_back(SerializadorJSON::jsonToPedidoMesa(val.toObject()));
      }
      return pedidos;
    };

    std::vector<PedidoMesa> pendientes = deserializarPedidos(data, "pedidos_pendientes");
    std::vector<PedidoMesa> enProgreso = deserializarPedidos(data, "pedidos_en_progreso");
    std::vector<PedidoMesa> terminados = deserializarPedidos(data, "pedidos_terminados");

    m_ventana->cargarEstadoInicial(pendientes, enProgreso, terminados, m_menu);
<<<<<<< HEAD
  } else if (evento == "PEDIDO_NUEVO") {
    PedidoMesa pedido = SerializadorJSON::jsonToPedidoMesa(data);
=======

  } else if (evento == Protocolo::PEDIDO_REGISTRADO) {
    PedidoMesa pedido = m_serializador.jsonToPedidoMesa(data);
>>>>>>> fbf1d3658d5f145ed08873dd1f488a0751640cc8
    m_ventana->onPedidoNuevo(pedido, m_menu);

  } else if (evento == Protocolo::PLATO_EN_PREPARACION) {
    long long idPedido = data["id_pedido"].toVariant().toLongLong();
    long long idInstancia = data["id_instancia"].toInt();
    QString estado = data["nuevo_estado"].toString();

    m_ventana->onActualizarEstadoPlato(idPedido, idInstancia, estado);
    m_ventana->onPedidoAMover(idPedido, "progreso");

  } else if (evento == Protocolo::PLATO_TERMINADO) {
    long long idPedido = data["id_pedido"].toVariant().toLongLong();
    long long idInstancia = data["id_instancia"].toInt();
    bool pedidoListo = data["pedido_listo"].toBool();
    QString estado = data["nuevo_estado"].toString();

    m_ventana->onActualizarEstadoPlato(idPedido, idInstancia, estado);
    if (pedidoListo) m_ventana->onPedidoAMover(idPedido, "terminado");

  } else if (evento == Protocolo::PEDIDO_CANCELADO || evento == Protocolo::PEDIDO_ENTREGADO) {
    long long idPedido = data["id_pedido"].toVariant().toLongLong();
    m_ventana->onPedidoAEliminar(idPedido);

  } else if (evento == Protocolo::PLATO_DEVUELTO) {
    long long idPedido = data["id_pedido"].toVariant().toLongLong();
    long long idInstancia = data["id_instancia"].toVariant().toLongLong();
    QString estado = data["nuevo_estado"].toString();

    m_ventana->onActualizarEstadoPlato(idPedido, idInstancia, estado);
    m_ventana->onPedidoAMover(idPedido, "progreso");
  }
}

void ClienteManagerApp::onPrepararPedido(long long idPedido) {
  qDebug() << "Solicitando preparar pedido con ID:" << idPedido;

  QJsonObject data;
  data["id_pedido"] = QJsonValue::fromVariant(QVariant::fromValue(idPedido));
  QJsonObject comando;
  comando[Protocolo::COMANDO] = Protocolo::PREPARAR_PEDIDO;
  comando[Protocolo::DATA] = data;
  m_clienteTCP->enviarMensaje(comando);
}

void ClienteManagerApp::onCancelarPedido(long long idPedido) {
  qDebug() << "Solicitando cancelar pedido con ID:" << idPedido;

  QJsonObject data;
  data["id_pedido"] = QJsonValue::fromVariant(QVariant::fromValue(idPedido));
  QJsonObject comando;
  comando[Protocolo::COMANDO] = Protocolo::CANCELAR_PEDIDO;
  comando[Protocolo::DATA] = data;
  m_clienteTCP->enviarMensaje(comando);
}

void ClienteManagerApp::onEnviarPedido(long long idPedido) {
  qDebug() << "Solicitando ENVIAR platos del pedido:" << idPedido;

  QJsonObject data;
  data["id_pedido"] = QJsonValue::fromVariant(QVariant::fromValue(idPedido));
  QJsonObject comando;
  comando[Protocolo::COMANDO] = Protocolo::CONFIRMAR_ENTREGA;
  comando[Protocolo::DATA] = data;
  m_clienteTCP->enviarMensaje(comando);
}

void ClienteManagerApp::onRechazarPedido(long long idPedido) {
  qDebug() << "Solicitando RECHAZAR/DEVOLVER platos del pedido:" << idPedido;

  QJsonObject data;
  data["id_pedido"] = QJsonValue::fromVariant(QVariant::fromValue(idPedido));
  QJsonObject comando;
  comando[Protocolo::COMANDO] = Protocolo::DEVOLVER_PLATO;
  comando[Protocolo::DATA] = data;
  // En una implementación más detallada, se podría especificar qué plato devolver.
  // Por ahora, se asume que se devuelven todos los que no estén entregados.
  m_clienteTCP->enviarMensaje(comando);
}

void ClienteManagerApp::onRechazarPlato(long long idPedido, long long idInstancia) {
  qDebug() << "Solicitando DEVOLVER plato específico:" << idInstancia << "del pedido:" << idPedido;

  QJsonObject data;
  data["id_pedido"] = QJsonValue::fromVariant(QVariant::fromValue(idPedido));
  data["id_instancia"] = QJsonValue::fromVariant(QVariant::fromValue(idInstancia));

  QJsonObject comando;
  comando[Protocolo::COMANDO] = Protocolo::DEVOLVER_PLATO;
  comando[Protocolo::DATA] = data;

  m_clienteTCP->enviarMensaje(comando);
}
