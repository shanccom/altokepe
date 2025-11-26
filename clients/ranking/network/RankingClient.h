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
  void rankingActualizado(const QJsonArray &rankingData);

private slots:
  void onConectado();
  void onDatosRecibidos();

private:
  QTcpSocket *m_socket;
  QByteArray m_buffer;
};
