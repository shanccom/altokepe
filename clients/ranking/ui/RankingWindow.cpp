#include "RankingWindow.h"
#include <QDebug>
#include <algorithm>

RankingWindow::RankingWindow(MenuRepository* repository, QWidget *parent) : QWidget(parent) {
  setWindowTitle("Ranking de Platos Vendidos");
  resize(1000, 600);
  this->setStyleSheet("background-color: #f4f7f6;");

  // --- Configuración de la Tabla de Ranking (Izquierda) ---
  m_tablaRanking = new QTableWidget(this);
  m_tablaRanking->setColumnCount(4);
  m_tablaRanking->setHorizontalHeaderLabels({ "Puesto", "Nombre", "Unidades", "Precio" });
  m_tablaRanking->setColumnWidth(3, 100); 
  m_tablaRanking->horizontalHeader()->setStretchLastSection(true);
  m_tablaRanking->verticalHeader()->setVisible(false);
  m_tablaRanking->setEditTriggers(QAbstractItemView::NoEditTriggers);
  m_tablaRanking->setSelectionMode(QAbstractItemView::NoSelection);

  m_tablaRanking->setStyleSheet(R"(
      QHeaderView::section {
          background-color: #f0ad4e;
          color: white;
          font-weight: bold;
          padding: 8px;
          border: none;
      }
      QTableWidget {
          font-size: 15px;
          gridline-color: #dce1e6;
          color: #333333;
          font-weight: bold;
          background-color: #ffffff;
          border: 1px solid #dce1e6;
          border-radius: 8px;
      }
      QTableWidget::item {
          padding: 5px;
          color: #333333;
          border-bottom: 1px solid #f0f0f0;
      }
  )");

  // --- Configuración del Menú (Derecha) ---
  
  // Título
  QLabel* tituloMenu = new QLabel("Carta Menú");
  tituloMenu->setAlignment(Qt::AlignCenter);
  tituloMenu->setStyleSheet("font-size: 40px; font-weight: bold; color: #e65100; font-family: 'Segoe UI', sans-serif;");
  tituloMenu->setContentsMargins(0, 10, 0, 0);

  // Frase decorativa
  QLabel* fraseMenu = new QLabel("No dejes para mañana lo que puedes comer hoy");
  fraseMenu->setAlignment(Qt::AlignCenter);
  fraseMenu->setStyleSheet("font-size: 18px; font-style: italic; color: #6d4c41; margin-bottom: 10px;");

  // Scroll Area para el menú
  m_scrollMenu = new QScrollArea(this);
  m_menuContainer = new QWidget();
  m_gridMenuLayout = new QGridLayout(m_menuContainer);
  m_menuContainer->setLayout(m_gridMenuLayout);
  m_scrollMenu->setWidget(m_menuContainer);
  m_scrollMenu->setWidgetResizable(true);
  m_scrollMenu->setFrameShape(QFrame::NoFrame);

  // Layout del lado derecho (Menú)
  QVBoxLayout* menuLayout = new QVBoxLayout();
  menuLayout->setContentsMargins(0, 0, 0, 0);
  menuLayout->setSpacing(10);
  menuLayout->addWidget(tituloMenu);
  menuLayout->addWidget(fraseMenu);
  menuLayout->addWidget(m_scrollMenu);

  QWidget* menuWidget = new QWidget();
  menuWidget->setLayout(menuLayout);

  // --- Layout Principal (Split) ---
  auto* mainLayout = new QHBoxLayout(this);
  mainLayout->addWidget(m_tablaRanking, 4); // 40% ancho
  mainLayout->addWidget(menuWidget, 6);     // 60% ancho
  setLayout(mainLayout);

  // --- Cargar Menú usando Repositorio ---
  if (repository) {
      m_menu = repository->obtenerMenu();
      if (!m_menu.isEmpty()) {
          mostrarMenuAgrupado(m_menu);
      } else {
          qWarning() << "El repositorio devolvió un menú vacío.";
      }
  } else {
      qCritical() << "Repositorio nulo en RankingWindow";
  }
}

void RankingWindow::actualizarRanking(const QJsonArray &rankingData) {
  m_tablaRanking->setRowCount(0);

  struct ItemVenta {
      QString nombre;
      int unidades;
      double precio;
  };

  QVector<ItemVenta> datos;
  QMap<QString, double> preciosRef;

  for(const auto& val : m_menu) {
      QJsonObject obj = val.toObject();
      preciosRef[obj["nombre"].toString()] = obj["costo"].toDouble();
  }

  for (const QJsonValue& val : rankingData) {
      QJsonObject obj = val.toObject();
      QString nombre = obj["nombre"].toString();
      int unidades = obj["cantidad"].toInt();
      
      double precio = 0.0;
      if(obj.contains("precio")) {
          precio = obj["precio"].toDouble();
      } else {
          precio = preciosRef.value(nombre, 0.0);
      }

      datos.append({nombre, unidades, precio});
  }

  // Ordenar por unidades descendente
  std::sort(datos.begin(), datos.end(), [](const ItemVenta& a, const ItemVenta& b) {
      return a.unidades > b.unidades;
  });

  m_tablaRanking->setRowCount(datos.size());

  for (int i = 0; i < datos.size(); ++i) {
      const auto& item = datos[i];

      // Columna 0: Puesto
      QTableWidgetItem* puestoItem = new QTableWidgetItem(QString::number(i + 1));
      puestoItem->setTextAlignment(Qt::AlignCenter);
      if (i == 0) {
          puestoItem->setBackground(QColor("#ffff66"));  // amarillo para el 1er puesto
          puestoItem->setFont(QFont("Arial", 10, QFont::Bold));
      }
      m_tablaRanking->setItem(i, 0, puestoItem);

      // Columna 1: Nombre
      QTableWidgetItem* nombreItem = new QTableWidgetItem(item.nombre);
      m_tablaRanking->setItem(i, 1, nombreItem);

      // Columna 2: Unidades
      QTableWidgetItem* unidadesItem = new QTableWidgetItem(QString::number(item.unidades));
      unidadesItem->setTextAlignment(Qt::AlignCenter);
      m_tablaRanking->setItem(i, 2, unidadesItem);

      // Columna 3: Precio
      QTableWidgetItem* precioItem = new QTableWidgetItem(QString("S/. %1").arg(item.precio, 0, 'f', 2));
      precioItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
      m_tablaRanking->setItem(i, 3, precioItem);
  }
}

void RankingWindow::mostrarMenuAgrupado(const QJsonArray& menu) {
  QMap<QString, QList<QPair<QString, double>>> platosPorEstacion;

  for (const QJsonValue& val : menu) {
      QJsonObject obj = val.toObject();
      QString estacion = obj["estacion"].toString();
      QString nombre = obj["nombre"].toString();
      double costo = obj["costo"].toDouble();

      platosPorEstacion[estacion].append(qMakePair(nombre, costo));
  }

  QLayoutItem* item;
  while ((item = m_gridMenuLayout->takeAt(0)) != nullptr) {
      if (item->widget()) item->widget()->deleteLater();
      delete item;
  }

  int col = 0;
  int row = 0;
  const int colCount = 2; // 2 columnas de categorías

  // Estilos inline para los componentes del menú
  QString groupStyle = R"(
      QGroupBox {
          border: 1px solid #dce1e6;
          border-radius: 8px;
          margin-top: 10px;
          padding: 15px;
          font-weight: bold;
          font-size: 16px;
          background-color: #ffffff;
      }
      QGroupBox::title {
          subcontrol-origin: margin;
          subcontrol-position: top center;
          padding: 5px 10px;
          color: #444444;
          background-color: #D2D2D2;
          border-radius: 6px;
      }
  )";

  QString itemStyleBase = "font-size: 15px; padding: 6px; border-radius: 6px; background-color: #f8f9fa; border: 1px solid #e0e0e0;";
  QString nameStyle = itemStyleBase + "color: #333333; font-weight: bold;";
  QString priceStyle = itemStyleBase + "color: #28a745; font-weight: bold;";

  for (auto it = platosPorEstacion.begin(); it != platosPorEstacion.end(); ++it) {
      QString categoria = it.key();
      QList<QPair<QString, double>> platos = it.value();

      QGroupBox* grupo = new QGroupBox(categoria);
      grupo->setStyleSheet(groupStyle);
      
      QVBoxLayout* vbox = new QVBoxLayout();
      vbox->setSpacing(5);

      for (const auto& par : platos) {
          QWidget* filaWidget = new QWidget();
          QHBoxLayout* filaLayout = new QHBoxLayout(filaWidget);
          filaLayout->setContentsMargins(0, 0, 0, 0);
          filaLayout->setSpacing(10);

          QLabel* nombreLabel = new QLabel(par.first);
          QLabel* precioLabel = new QLabel(QString("S/ %1").arg(par.second, 0, 'f', 2));

          nombreLabel->setStyleSheet(nameStyle);
          precioLabel->setStyleSheet(priceStyle);
          
          nombreLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
          precioLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
          precioLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

          filaLayout->addWidget(nombreLabel);
          filaLayout->addWidget(precioLabel);
          vbox->addWidget(filaWidget);
      }
      
      vbox->addStretch();
      grupo->setLayout(vbox);
      m_gridMenuLayout->addWidget(grupo, row, col);

      col++;
      if (col >= colCount) {
          col = 0;
          row++;
      }
  }
}
