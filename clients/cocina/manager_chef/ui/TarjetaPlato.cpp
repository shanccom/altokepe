#include "TarjetaPlato.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QStyle>

TarjetaPlato::TarjetaPlato(const QString& nombrePlato, const QString& estadoInicial, QWidget* parent)
  : QWidget(parent) {
  setAttribute(Qt::WA_StyledBackground, true);
  
  this->setProperty("class", "TarjetaPlato");

  QHBoxLayout* layout = new QHBoxLayout(this);
  layout->setContentsMargins(8, 8, 8, 8);
  layout->setSpacing(10);

  m_nombreLabel = new QLabel(nombrePlato, this);
  m_nombreLabel->setProperty("tipo", "nombre_plato");
  m_nombreLabel->setWordWrap(true);

  m_estadoLabel = new QLabel(estadoInicial, this);
  m_estadoLabel->setAlignment(Qt::AlignCenter);
  m_estadoLabel->setProperty("tipo", "estado"); 
  
  layout->addWidget(m_nombreLabel, 1);
  layout->addWidget(m_estadoLabel, 0);
  
  setLayout(layout);
  actualizarEstado(estadoInicial);
}

void TarjetaPlato::actualizarEstado(const QString& nuevoEstado) {
  m_estadoLabel->setText(nuevoEstado);
  m_estadoLabel->setProperty("estado_plato", nuevoEstado);
  m_estadoLabel->style()->unpolish(m_estadoLabel);
  m_estadoLabel->style()->polish(m_estadoLabel);
}
