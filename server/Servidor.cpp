#include "Servidor.h"
#include "ManejadorCliente.h"
#include "LogicaNegocio.h"
#include <QThread>
#include <QDebug>

Servidor::Servidor(QObject* parent) : QTcpServer(parent) {}

void Servidor::incomingConnection(qintptr socketDescriptor) {
  qDebug() << "Nueva conexión entrante con descriptor:" << socketDescriptor;

  QThread* thread = new QThread;
  ManejadorCliente* manejador = new ManejadorCliente(socketDescriptor);
  
  manejador->moveToThread(thread);

  connect(thread, &QThread::started, manejador, &ManejadorCliente::procesar);
  connect(manejador, &ManejadorCliente::finished, thread, &QThread::quit);
  connect(manejador, &ManejadorCliente::finished, manejador, &ManejadorCliente::deleteLater);
  connect(thread, &QThread::finished, thread, &QThread::deleteLater);

  connect(LogicaNegocio::instance(), &LogicaNegocio::enviarRespuesta, manejador,
    [manejador](ManejadorCliente* clienteDestino, const QJsonObject& mensaje) {
      // La lambda se ejecuta para CADA manejador.
      // Solo si este manejador es el destinatario, se envía el mensaje.
      if (manejador == clienteDestino) {
        manejador->enviarMensaje(mensaje);
      }
    }, Qt::QueuedConnection);

  thread->start();
}