#include "RankingClient.h"
#include "common/network/Protocolo.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QTcpSocket>
#include <QDebug>

RankingClient::RankingClient(QObject *parent) : QObject(parent) {
  m_socket = new QTcpSocket(this);

  connect(m_socket, &QTcpSocket::connected, this, &RankingClient::onConectado);
  connect(m_socket, &QTcpSocket::readyRead, this, &RankingClient::onDatosRecibidos);
  connect(m_socket, &QTcpSocket::errorOccurred, this, &RankingClient::onErrorSocket);
  connect(m_socket, &QTcpSocket::disconnected, this, &RankingClient::onDesconectado);
}

void RankingClient::conectar(const QString &host, quint16 puerto) {
  qDebug() << "Conectando al servidor en" << host << ":" << puerto;
  m_socket->connectToHost(host, puerto);
}

void RankingClient::onConectado() {
  qDebug() << "Conectado al servidor. Identificándose...";
  
  // Protocolo: Enviar identificación
  QJsonObject mensaje;
  mensaje["comando"] = "IDENTIFICARSE";
  mensaje["rol"] = "Ranking";

  QJsonDocument doc(mensaje);
  m_socket->write(doc.toJson(QJsonDocument::Compact) + "\n");
}

void RankingClient::onDatosRecibidos() {
  m_buffer.append(m_socket->readAll());

  while (m_buffer.contains('\n')) {
    int pos = m_buffer.indexOf('\n');
    QByteArray linea = m_buffer.left(pos);
    m_buffer.remove(0, pos + 1);

    if (linea.isEmpty()) continue;

    // VALIDACIÓN 1: JSON válido
    QJsonDocument doc = QJsonDocument::fromJson(linea);
    if (doc.isNull() || !doc.isObject()) {
      qWarning() << "JSON inválido recibido del servidor";
      emit errorDatos("Datos malformados recibidos del servidor");
      continue;
    }

    QJsonObject obj = doc.object();
    
    // VALIDACIÓN 2: Campo 'evento' existe y es string
    if (!obj.contains("evento") || !obj["evento"].isString()) {
      qWarning() << "Mensaje sin campo 'evento' válido";
      emit errorDatos("Mensaje sin tipo de evento");
      continue;
    }
    
    QString evento = obj["evento"].toString();
    
    // MANEJO DE ERRORES DEL SERVIDOR (Protocolo común)
    if (evento == Protocolo::ERROR) {
      QString mensajeError = obj.value(Protocolo::MENSAJE_ERROR).toString("Error desconocido del servidor");
      qWarning() << "Error del servidor:" << mensajeError;
      emit errorServidor(mensajeError);
      continue;
    }
    
    // VALIDACIÓN 3: Evento de actualización de ranking
    if (evento == Protocolo::ACTUALIZACION_RANKING) {
      // VALIDACIÓN 4: Campo 'data' existe y es objeto
      if (!obj.contains("data") || !obj["data"].isObject()) {
        qWarning() << "Evento ACTUALIZACION_RANKING sin campo 'data' válido";
        emit errorDatos("Estructura de datos de ranking inválida");
        continue;
      }
      
      QJsonObject data = obj["data"].toObject();
      
      // VALIDACIÓN 5: Datos contienen 'menu' y 'ranking' como arrays
      bool menuValido = data.contains("menu") && data["menu"].isArray();
      bool rankingValido = data.contains("ranking") && data["ranking"].isArray();
      
      if (!menuValido || !rankingValido) {
        qWarning() << "Datos de ranking incompletos - menu:" << menuValido << "ranking:" << rankingValido;
        emit errorDatos("Datos de ranking incompletos");
        continue;
      }
      
      qDebug() << "Datos recibidos del servidor (menu + ranking).";
      emit datosActualizados(data);
    }
    // Ignorar eventos desconocidos silenciosamente
  }
}

void RankingClient::onErrorSocket(QAbstractSocket::SocketError error) {
  QString mensajeError;
  
  switch (error) {
    case QAbstractSocket::ConnectionRefusedError:
      mensajeError = "Conexión rechazada. El servidor no está disponible.";
      break;
    case QAbstractSocket::RemoteHostClosedError:
      mensajeError = "El servidor cerró la conexión.";
      break;
    case QAbstractSocket::HostNotFoundError:
      mensajeError = "Servidor no encontrado. Verifica la dirección.";
      break;
    case QAbstractSocket::SocketTimeoutError:
      mensajeError = "Timeout de conexión. El servidor no responde.";
      break;
    case QAbstractSocket::NetworkError:
      mensajeError = "Error de red. Verifica tu conexión.";
      break;
    default:
      mensajeError = m_socket->errorString();
      break;
  }
  
  qCritical() << "Error de socket:" << mensajeError;
  emit errorConexion(mensajeError);
}

void RankingClient::onDesconectado() {
  qWarning() << "Desconectado del servidor";
  emit desconectado();
}
