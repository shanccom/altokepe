#include "ColumnaPedidos.h"
#include "TarjetaPedido.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QScrollArea>

ColumnaPedidos::ColumnaPedidos(const QString& titulo, QWidget* parent)
  : QWidget(parent) {
  setAttribute(Qt::WA_StyledBackground, true); 

  this->setProperty("class", "ColumnaPedidos");

  QVBoxLayout* mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(0, 0, 0, 0);

  QLabel* tituloLabel = new QLabel(titulo, this);
  tituloLabel->setAlignment(Qt::AlignCenter);
  tituloLabel->setProperty("class", "TituloColumna");
  mainLayout->addWidget(tituloLabel);

  QScrollArea* scrollArea = new QScrollArea(this);
  scrollArea->setWidgetResizable(true);
  scrollArea->setFrameShape(QFrame::NoFrame);

  QWidget* scrollContent = new QWidget();
  m_layoutContenido = new QVBoxLayout(scrollContent);
  m_layoutContenido->setAlignment(Qt::AlignTop);
  scrollContent->setLayout(m_layoutContenido);

  scrollArea->setWidget(scrollContent);
  mainLayout->addWidget(scrollArea);

  setLayout(mainLayout);
}

bool ColumnaPedidos::estaVacia() const {
  return m_layoutContenido->count() == 0;
}

QList<TarjetaPedido*> ColumnaPedidos::obtenerTodosLosPedidos() const {
  QList<TarjetaPedido*> lista;
  for (int i = 0; i < m_layoutContenido->count(); ++i) {
    QLayoutItem* item = m_layoutContenido->itemAt(i);
    if (item && item->widget()) {
      TarjetaPedido* tarjeta = qobject_cast<TarjetaPedido*>(item->widget());
      if (tarjeta) lista.append(tarjeta);
    }
  }
  return lista;
}

void ColumnaPedidos::agregarPedido(TarjetaPedido* pedido) {
  m_layoutContenido->addWidget(pedido);
}

void ColumnaPedidos::limpiarPedidos() {
  QLayoutItem* item;
  while ((item = m_layoutContenido->takeAt(0)) != nullptr) {
    if (item->widget()) {
      delete item->widget();
    }
    delete item;
  }
}
