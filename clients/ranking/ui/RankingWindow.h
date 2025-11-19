#pragma once
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QListWidget>
#include <QVBoxLayout>
#include <QWidget>

class RankingWindow : public QWidget {
  Q_OBJECT

public:
  explicit RankingWindow(QWidget *parent = nullptr);

public slots:
  // Este slot reacciona a la señal del cliente
  void actualizarLista(const QJsonArray &rankingData);

private:
  QListWidget *m_lista;
  QLabel *m_titulo;
};
