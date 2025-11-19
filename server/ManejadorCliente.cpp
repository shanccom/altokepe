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

ManejadorCliente::~ManejajadorCliente() {
  qDebug() << "Manejador de cliente destruido para socket" << m_socketDescriptor;
}

// Getters
TipoActor ManejadorCliente::getTipoActor() const { return m_tipoActor; }
int ManejadorCliente::getIdActor() const { return m_idActor; }
QString ManejadorCliente::getNombreEstacion() const { return m_nombreEstacion; }

// Métodos principales vacíos
void ManejadorCliente::procesar() {}
void ManejadorCliente::listoParaLeer() {}
void ManejadorCliente::procesarBuffer() {}
void ManejadorCliente::identificarCliente(const QJsonObject&) {}
void ManejadorCliente::desconectado() {}
void ManejadorCliente::enviarMensaje(const QJsonObject&) {}
