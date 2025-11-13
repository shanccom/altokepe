#include "ClienteTCP.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

ClienteTCP::ClienteTCP(QObject* parent) : QObject(parent) {
  m_socket = new QTcpSocket(this);
  connect(m_socket, &QTcpSocket::connected, this, &ClienteTCP::onConectado);
  connect(m_socket, &QTcpSocket::disconnected, this, &ClienteTCP::onDesconectado);
  connect(m_socket, &QTcpSocket::readyRead, this, &ClienteTCP::listoParaLeer);
}

void ClienteTCP::conectar(const QString& host, quint16 port) {
  qDebug() << "Intentando conectar a" << host << ":" << port;
  m_socket->connectToHost(host, port);
}

void ClienteTCP::enviarMensaje(const QJsonObject& mensaje) {
  if (m_socket->state() == QAbstractSocket::ConnectedState) {
    // El servidor espera mensajes terminados en newline para separarlos.
    m_socket->write(QJsonDocument(mensaje).toJson(QJsonDocument::Compact) + "\n");
  } else {
    qWarning() << "No se puede enviar mensaje, socket no conectado.";
  }
}

void ClienteTCP::onConectado() {
  qDebug() << "Conectado al servidor.";
  emit conectado();
}

void ClienteTCP::onDesconectado() {
  qDebug() << "Desconectado del servidor.";
  emit desconectado();
}

void ClienteTCP::listoParaLeer() {
  m_buffer.append(m_socket->readAll());

  // Procesamos el buffer buscando mensajes completos (separados por newline)
  while (m_buffer.contains('\n')) {
    int newlinePos = m_buffer.indexOf('\n');
    QByteArray jsonData = m_buffer.left(newlinePos);
    m_buffer.remove(0, newlinePos + 1);

    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    if (!doc.isNull() && doc.isObject()) {
      emit nuevoMensajeRecibido(doc.object());
    } else {
      qWarning() << "Mensaje JSON inválido recibido:" << QString(jsonData);
    }
  }
}
