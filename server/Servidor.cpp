#include "Servidor.h"
#include "ManejadorCliente.h"
#include "LogicaNegocio.h"
#include <QThread>
#include <QDebug>
#include <QJsonObject>

Servidor::Servidor(QObject* parent) : QTcpServer(parent) {}

void Servidor::incomingConnection(qintptr socketDescriptor) {

  QThread* thread = new QThread;
  ManejadorCliente* manejador = new ManejadorCliente(socketDescriptor);
  
  manejador->moveToThread(thread);

  connect(thread, &QThread::started, manejador, &ManejadorCliente::procesar);
  connect(manejador, &ManejadorCliente::finished, thread, &QThread::quit);
  connect(manejador, &ManejadorCliente::finished, manejador, &ManejadorCliente::deleteLater);
  connect(thread, &QThread::finished, thread, &QThread::deleteLater);

  connect(LogicaNegocio::instance(), &LogicaNegocio::enviarRespuesta, manejador,
    [manejador](ManejadorCliente* clienteDestino, const QJsonObject& mensaje) {
      
      // Caso 1: Mensaje directo a un cliente específico
      if (clienteDestino == manejador) {
        manejador->enviarMensaje(mensaje);
        return;
      }
      // Caso 2: Broadcast (clienteDestino == nullptr)
      if (clienteDestino == nullptr) {
        if (mensaje.value("evento").toString() == "ACTUALIZACION_RANKING") {
             if (manejador->getTipoActor() == TipoActor::RANKING) {
                 manejador->enviarMensaje(mensaje);
             }
        }
      }
    }, Qt::QueuedConnection);

  thread->start();
}