#ifndef CLIENTETCP_H
#define CLIENTETCP_H

#include <QObject>
#include <QTcpSocket>

class ClienteTCP : public QObject {
  Q_OBJECT
public:
  explicit ClienteTCP(QObject* parent = nullptr);
  void conectar(const QString& host, quint16 port);
  void enviarMensaje(const QJsonObject& mensaje);

signals:
  void conectado();
  void desconectado();
  void nuevoMensajeRecibido(const QJsonObject& mensaje);

private slots:
  void onConectado();
  void onDesconectado();
  void listoParaLeer();

private:
  QTcpSocket* m_socket;
  QByteArray m_buffer;
};

#endif
