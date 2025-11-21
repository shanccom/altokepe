#include "common/network/Protocolo.h"
#include "common/models/Estados.h"
#include "ManejadorCliente.h"
#include "LogicaNegocio.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

ManejadorCliente::ManejadorCliente(qintptr socketDescriptor, QObject* parent)
  : QObject(parent), m_socketDescriptor(socketDescriptor) {
}

ManejadorCliente::~ManejadorCliente() {
  qDebug() << "Manejador de cliente destruido para socket" << m_socketDescriptor;
}

TipoActor ManejadorCliente::getTipoActor() const { return m_tipoActor; }
int ManejadorCliente::getIdActor() const { return m_idActor; }
QString ManejadorCliente::getNombreEstacion() const { return m_nombreEstacion; }

void ManejadorCliente::procesar() {
  m_socket = new QTcpSocket();
  if (!m_socket->setSocketDescriptor(m_socketDescriptor)) {
    qCritical() << "No se pudo establecer el descriptor de socket" << m_socketDescriptor;
    emit finished();
    return;
  }

  connect(m_socket, &QTcpSocket::readyRead, this, &ManejadorCliente::listoParaLeer);
  connect(m_socket, &QTcpSocket::disconnected, this, &ManejadorCliente::desconectado);

  LogicaNegocio::instance()->registrarManejador(this);
  qDebug() << "Cliente conectado en socket" << m_socketDescriptor;
}

void ManejadorCliente::listoParaLeer() {
  m_buffer.append(m_socket->readAll());
  procesarBuffer();
}

void ManejadorCliente::procesarBuffer() {
  while (m_buffer.contains('\n')) {
    int newlinePos = m_buffer.indexOf('\n');
    QByteArray jsonData = m_buffer.left(newlinePos);
    m_buffer.remove(0, newlinePos + 1);
    if (jsonData.isEmpty()) continue;
    
    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    if (doc.isNull() || !doc.isObject()) {
      qWarning() << "Mensaje JSON inválido recibido en socket" << m_socketDescriptor;
      continue;
    }

    QJsonObject mensaje = doc.object();
    qDebug() << "Mensaje recibido:" << mensaje;  // ← AGREGA ESTO
    qDebug() << "Comando:" << mensaje[Protocolo::COMANDO].toString();

    if (m_tipoActor == TipoActor::DESCONOCIDO) {
      if (mensaje[Protocolo::COMANDO].toString() == Protocolo::IDENTIFICARSE) {
        identificarCliente(mensaje);
      } else {
        qWarning() << "Cliente no identificado intentó enviar un comando. Se ignora.";
      }
    } else {
      LogicaNegocio::instance()->procesarMensaje(mensaje, this);
    }
  }
}

void ManejadorCliente::identificarCliente(const QJsonObject& data) {
  QString rolString = data["rol"].toString();

  if (rolString == "Recepcionista") {
    m_tipoActor = TipoActor::RECEPCIONISTA;
  } else if (rolString == "ManagerChef") {
    m_tipoActor = TipoActor::MANAGER_CHEF;
  } else if (rolString == "EstacionCocina") {
    m_tipoActor = TipoActor::ESTACION_COCINA;
  } else if (rolString == "Ranking") {
    m_tipoActor = TipoActor::RANKING;
  } else {
    m_tipoActor = TipoActor::DESCONOCIDO;
    qWarning() << "Rol desconocido recibido:" << rolString << ". Desconectando.";
    m_socket->disconnectFromHost();
    return;
  }

  switch (m_tipoActor) {
    case TipoActor::RECEPCIONISTA:
      m_idActor = data["id"].toInt(-1);
      if (m_idActor == -1) {
        qWarning() << "Recepcionista no proporcionó un 'id' válido. Desconectando.";
        m_socket->disconnectFromHost();
        return;
      }
      qDebug() << "Cliente" << m_socketDescriptor << "identificado como RECEPCIONISTA con ID:" << m_idActor;
      break;

    case TipoActor::ESTACION_COCINA:
      m_nombreEstacion = data["nombre_estacion"].toString();
      if (m_nombreEstacion.isEmpty()) {
        qWarning() << "EstacionCocina no proporcionó 'nombre_estacion'. Desconectando.";
        m_socket->disconnectFromHost();
        return;
      }
      qDebug() << "Cliente" << m_socketDescriptor << "identificado como ESTACION_COCINA con Nombre:" << m_nombreEstacion;
      break;

    case TipoActor::MANAGER_CHEF:
      qDebug() << "Cliente" << m_socketDescriptor << "identificado como MANAGER_CHEF.";
      break;
    
    case TipoActor::RANKING:
      qDebug() << "Cliente" << m_socketDescriptor << "identificado como RANKING.";
      break;

    default:
      break;
  }
  
  // Le pedimos a la lógica de negocio que le envíe el estado inicial
  if (m_tipoActor != TipoActor::DESCONOCIDO) LogicaNegocio::instance()->enviarEstadoInicial(this);
}

void ManejadorCliente::desconectado() {
  qDebug() << "Cliente desconectado del socket" << m_socketDescriptor;
  LogicaNegocio::instance()->eliminarManejador(this);
  m_socket->deleteLater();
  emit finished();
}

void ManejadorCliente::enviarMensaje(const QJsonObject& mensaje) {
  if (m_socket && m_socket->isOpen()) {
    QJsonDocument doc(mensaje); 
    m_socket->write(doc.toJson(QJsonDocument::Compact) + "\n");
  }
}