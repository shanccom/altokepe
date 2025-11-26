#include "RankingClient.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QTcpSocket>
#include <QDebug>

RankingClient::RankingClient(QObject *parent) : QObject(parent) {
  m_socket = new QTcpSocket(this);

  connect(m_socket, &QTcpSocket::connected, this, &RankingClient::onConectado);
  connect(m_socket, &QTcpSocket::readyRead, this, &RankingClient::onDatosRecibidos);
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

    QJsonDocument doc = QJsonDocument::fromJson(linea);
    if (!doc.isObject()) continue;

    QJsonObject obj = doc.object();
    
    // Verificar si es una actualización de ranking
    if (obj["evento"].toString() == "ACTUALIZACION_RANKING") {
       QJsonObject data = obj["data"].toObject();
       QJsonArray ranking = data["ranking"].toArray();
       
       qDebug() << "Ranking recibido con" << ranking.size() << "elementos.";
       emit rankingActualizado(ranking);
    }
  }
}
