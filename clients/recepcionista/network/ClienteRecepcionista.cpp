#include "ClienteRecepcionista.h"
#include "common/network/Protocolo.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QTcpSocket>

ClienteRecepcionista::ClienteRecepcionista(QObject *parent)
  : QObject(parent), socket(new QTcpSocket(this)) {

  connect(socket, &QTcpSocket::readyRead, this, &ClienteRecepcionista::leerMensaje);
}

void ClienteRecepcionista::conectarAlServidor(const QString &host, quint16 puerto) {
  socket->connectToHost(host, puerto);
  if (socket->waitForConnected(5000)) {
    qDebug() << "Conectado al servidor";

    // Identificarse como Recepcionista
    QJsonObject msg{
      {Protocolo::COMANDO, Protocolo::IDENTIFICARSE},
      {"rol", "Recepcionista"},
      {"id", 101}
    };
    socket->write(QJsonDocument(msg).toJson(QJsonDocument::Compact) + "\n");
  } else {
    qDebug() << "Error al conectar con el servidor.";
  }
}

void ClienteRecepcionista::enviarNuevoPedido(int mesa, int idRecepcionista, const QJsonArray &platos) {
  QJsonObject pedido{
    {Protocolo::COMANDO, Protocolo::NUEVO_PEDIDO},
    {"mesa", mesa},
    {"id_recepcionista", idRecepcionista},
    {"platos", platos}
  };
  socket->write(QJsonDocument(pedido).toJson(QJsonDocument::Compact) + "\n");
}

void ClienteRecepcionista::leerMensaje() {
  while (socket->canReadLine()) {
    QByteArray linea = socket->readLine().trimmed();
    QJsonDocument doc = QJsonDocument::fromJson(linea);
    if (!doc.isNull() && doc.isObject()) {
      QJsonObject obj = doc.object();
      qDebug() << "Mensaje recibido:";
      qDebug().noquote() << QJsonDocument(obj).toJson(QJsonDocument::Indented);

      if (obj["evento"].toString() == "ACTUALIZACION_MENU") {
        QJsonObject data = obj["data"].toObject();
        QJsonArray menu = data["menu"].toArray();
        emit menuActualizado(menu);
      }
    } else {
      qWarning() << "JSON inválido recibido:" << linea;
    }
  }
}
