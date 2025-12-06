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
#include "../repository/MenuRepository.h"

class RankingWindow : public QWidget {
  Q_OBJECT

public:
  explicit RankingWindow(MenuRepository* repository, QWidget *parent = nullptr);

public slots:
  // Este slot reacciona a la señal del cliente
  void actualizarRanking(const QJsonArray &rankingData);

private:
  void mostrarMenuAgrupado(const QJsonArray& menu);

  QTableWidget *m_tablaRanking;
  QJsonArray m_menu;
  
  QScrollArea* m_scrollMenu;
  QWidget* m_menuContainer;
  QGridLayout* m_gridMenuLayout;
};
