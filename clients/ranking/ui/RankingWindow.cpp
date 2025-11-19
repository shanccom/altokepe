#include "RankingWindow.h"

RankingWindow::RankingWindow(QWidget *parent) : QWidget(parent) {
  // Configuración básica de la ventana
  auto layout = new QVBoxLayout(this);

  m_titulo = new QLabel("🏆 Ranking de Platos (En Vivo)", this);
  QFont font = m_titulo->font();
  font.setPointSize(16);
  font.setBold(true);
  m_titulo->setFont(font);
  m_titulo->setAlignment(Qt::AlignCenter);

  m_lista = new QListWidget(this);

  layout->addWidget(m_titulo);
  layout->addWidget(m_lista);

  resize(400, 500);
}

void RankingWindow::actualizarLista(const QJsonArray &rankingData) {
  m_lista->clear(); // Limpiamos la lista vieja

  // Recorremos el array limpio que nos llegó
  for (const auto &val : rankingData) {
    QJsonObject obj = val.toObject();
    QString nombre = obj["nombre"].toString();
    int cantidad = obj["cantidad"].toInt();

    QString texto = QString("%1 - Ventas: %2").arg(nombre).arg(cantidad);
    m_lista->addItem(texto);
  }
}
