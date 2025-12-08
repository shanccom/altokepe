#pragma once
#include <QJsonArray>
#include <QJsonObject>
#include <QObject>
#include <QTcpSocket>

class RankingClient : public QObject {
  Q_OBJECT

public:
  explicit RankingClient(QObject *parent = nullptr);

  void conectar(const QString &host, quint16 puerto);

signals:
  // SEÑAL DEL OBSERVER: La Vista se suscribirá a esto.
  // Facade: Ocultamos la complejidad del protocolo de red.
  void datosActualizados(const QJsonObject &data);
  
  // SEÑALES DE ERROR: Notifican diferentes tipos de errores
  void errorConexion(const QString& mensaje);
  void errorDatos(const QString& mensaje);
  void errorServidor(const QString& mensaje);
  void desconectado();

private slots:
  void onConectado();
  void onDatosRecibidos();
  void onErrorSocket(QAbstractSocket::SocketError error);
  void onDesconectado();

private:
  QTcpSocket *m_socket;
  QByteArray m_buffer;
};

