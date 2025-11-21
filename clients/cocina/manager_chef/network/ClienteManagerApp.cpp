#include "ClienteManagerApp.h"
#include "../network/ClienteTCP.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

#include "common/network/Protocolo.h"
#include "common/network/SerializadorJSON.h"

ClienteManagerApp::ClienteManagerApp(QObject* parent) : QObject(parent) {
  m_clienteTCP = new ClienteTCP(this);

  connect(m_clienteTCP, &ClienteTCP::conectado, this, &ClienteManagerApp::onConectado);
  connect(m_clienteTCP, &ClienteTCP::nuevoMensajeRecibido, this, &ClienteManagerApp::onMensajeRecibido);
}

void ClienteManagerApp::iniciar() {
  m_clienteTCP->conectar("127.0.0.1", 5555); // Conectar a localhost
}

void ClienteManagerApp::onConectado() {
  qDebug() << "Conexión exitosa. Identificándose como ManagerChef...";
  QJsonObject identificacion;
  identificacion[Protocolo::COMANDO] = Protocolo::IDENTIFICARSE;
  identificacion["rol"] = "ManagerChef";

  m_clienteTCP->enviarMensaje(identificacion);
}

void ClienteManagerApp::onMensajeRecibido(const QJsonObject& mensaje) {
  QString evento = mensaje[Protocolo::EVENTO].toString();
  QJsonObject data = mensaje[Protocolo::DATA].toObject();

  qDebug() << "Evento recibido:" << evento;
  qDebug() << "Data recibida:" << data;
}
