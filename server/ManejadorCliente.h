#ifndef MANEJADORCLIENTE_H
#define MANEJADORCLIENTE_H

#include <QObject>
#include <QTcpSocket>
#include <QString>
#include "common/models/Estados.h"

class ManejadorCliente : public QObject {
  Q_OBJECT
public:
  explicit ManejadorCliente(qintptr socketDescriptor, QObject* parent = nullptr);
  virtual ~ManejadorCliente();

  TipoActor getTipoActor() const;
  int getIdActor() const;
  QString getNombreEstacion() const;

public slots:
  void procesar();
  void enviarMensaje(const QJsonObject& mensaje);

private slots:
  void listoParaLeer();
  void desconectado();

signals:
  void finished();

private:
  void procesarBuffer();
  void identificarCliente(const QJsonObject& data);

  qintptr m_socketDescriptor;
  QTcpSocket* m_socket = nullptr;
  QByteArray m_buffer;

  TipoActor m_tipoActor = TipoActor::DESCONOCIDO;
  int m_idActor = -1;
  QString m_nombreEstacion;
};

#endif