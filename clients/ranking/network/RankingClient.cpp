#include "RankingClient.h"
#include <QDebug>

RankingClient::RankingClient(QObject *parent) : QObject(parent) {
  m_simuladorTimer = new QTimer(this);
  connect(m_simuladorTimer, &QTimer::timeout, this,
          &RankingClient::generarDatosFalsos);
}

void RankingClient::iniciarSimulacion() {
  qDebug() << "Simulación iniciada: Esperando datos...";
  m_simuladorTimer->start(3000); // Generar datos cada 3 segundos
}

void RankingClient::generarDatosFalsos() {
  QJsonArray ranking;

  // Dato 1
  QJsonObject plato1;
  plato1["nombre"] = "Lomo Saltado";
  plato1["cantidad"] = rand() % 20;
  ranking.append(plato1);

  // Dato 2
  QJsonObject plato2;
  plato2["nombre"] = "Ceviche";
  plato2["cantidad"] = rand() % 20;
  ranking.append(plato2);

  qDebug()
      << "RankingClient: Recibido evento (simulado), notificando a la UI...";

  emit rankingActualizado(ranking);
}
