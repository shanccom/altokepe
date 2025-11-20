#pragma once
#include <QJsonArray>
#include <QJsonObject>
#include <QObject>
#include <QTimer> // Solo para la simulación de hoy

class RankingClient : public QObject {
  Q_OBJECT

public:
  explicit RankingClient(QObject *parent = nullptr);

  // Método para simular que el servidor nos manda datos (Patrón: Mocking)
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
