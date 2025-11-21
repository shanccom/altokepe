#include "Servidor.h"
#include "ManejadorCliente.h"
#include <QThread>
#include <QDebug>

Servidor::Servidor(QObject* parent) : QTcpServer(parent) {}

void Servidor::incomingConnection(qintptr socketDescriptor) {
  qDebug() << "Nueva conexión entrante con descriptor:" << socketDescriptor;

  QThread* thread = new QThread;
  ManejadorCliente* manejador = new ManejadorCliente(socketDescriptor);

  connect(thread, &QThread::started, manejador, &ManejadorCliente::procesar);
  connect(manejador, &ManejadorCliente::finished, thread, &QThread::quit);
  connect(manejador, &ManejadorCliente::finished, manejador, &ManejadorCliente::deleteLater);
  connect(thread, &QThread::finished, thread, &QThread::deleteLater);

  // En un futuro aca tiene que ir la logica de negocio

  thread->start();

}