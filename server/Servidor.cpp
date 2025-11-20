#include "Servidor.h"
#include "ManejadorCliente.h"
#include <QThread>
#include <QDebug>

Servidor::Servidor(QObject* parent) : QTcpServer(parent) {}

void Servidor::incomingConnection(qintptr socketDescriptor) {
}