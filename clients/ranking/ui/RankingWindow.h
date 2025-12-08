#pragma once
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QScrollArea>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>

class RankingWindow : public QWidget {
  Q_OBJECT

public:
  explicit RankingWindow(QWidget *parent = nullptr);

public slots:
  // Este slot reacciona a la señal del cliente con datos completos (menu + ranking)
  void actualizarDatos(const QJsonObject &data);

private:
  void mostrarMenuAgrupado(const QJsonArray& menu);

  QTableWidget *m_tablaRanking;
  QJsonArray m_menu;
  
  QScrollArea* m_scrollMenu;
  QWidget* m_menuContainer;
  QGridLayout* m_gridMenuLayout;
};
