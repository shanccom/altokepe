#include "TarjetaPlato.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QString>

TarjetaPlato::TarjetaPlato(const QString& nombrePlato, const QString& estadoInicial, QWidget* parent)
  : QWidget(parent) {
  
  setObjectName("TarjetaPlato");
  
  QHBoxLayout* layout = new QHBoxLayout(this);
  m_nombreLabel = new QLabel(nombrePlato, this);
  m_estadoLabel = new QLabel(estadoInicial, this);
  
  // Estilo para el estado
  m_estadoLabel->setAlignment(Qt::AlignCenter);
  m_estadoLabel->setStyleSheet("background-color: #f0c040; color: #333; border-radius: 8px; padding: 4px; font-weight: bold;");

  layout->addWidget(m_nombreLabel);
  layout->addStretch();
  layout->addWidget(m_estadoLabel);
  
  setLayout(layout);
}

void TarjetaPlato::actualizarEstado(const QString& nuevoEstado) {
  m_estadoLabel->setText(nuevoEstado);

  static const QMap<QString, QString> estadoColores = {
    {"En Espera", "#aaaaaa"},
    {"En Progreso", "#f0c040"},
    {"Finalizado", "#5cb85c"},
    {"Entregado", "#337ab7"},
    {"Cancelado", "#d9534f"}
  };

  QString color = estadoColores.value(nuevoEstado, "#aaaaaa");
  m_estadoLabel->setStyleSheet(
      QString("background-color: %1; color: white; border-radius: 8px; padding: 4px; font-weight: bold;").arg(color));
}
