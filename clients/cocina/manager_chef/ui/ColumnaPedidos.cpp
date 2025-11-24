#include "ColumnaPedidos.h"
#include "TarjetaPedido.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QScrollArea>

ColumnaPedidos::ColumnaPedidos(const QString& titulo, QWidget* parent)
  : QWidget(parent) {

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

TarjetaPedido* ColumnaPedidos::getPrimerPedido() const {
  if (estaVacia()) return nullptr;
  QLayoutItem* item = m_layoutContenido->itemAt(0);
  return qobject_cast<TarjetaPedido*>(item->widget());
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
