#pragma once
#include <QJsonArray>
#include <QJsonObject>
#include <QObject>
#include <QTimer> // Solo para la simulación

class RankingClient : public QObject {
  Q_OBJECT

public:
  explicit RankingClient(QObject *parent = nullptr);

  void iniciarSimulacion();

signals:
  // SEÑAL DEL OBSERVER: La Vista se suscribirá a esto.
  // No le importa de dónde vienen los datos (red o simulación), solo que
  // llegaron.
  void rankingActualizado(const QJsonArray &rankingData);

private slots:
  void generarDatosFalsos();

private:
  QTimer *m_simuladorTimer;
};
