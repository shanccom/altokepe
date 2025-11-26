#include "TarjetaPedido.h"
#include "TarjetaPlato.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

TarjetaPedido::TarjetaPedido(long long idPedido, const QString& titulo, QWidget* parent)
  : QWidget(parent), m_idPedido(idPedido) {

  this->setProperty("class", "TarjetaPedido");
  QVBoxLayout* mainLayout = new QVBoxLayout(this);
  QLabel* tituloLabel = new QLabel(titulo, this);
  tituloLabel->setProperty("class", "TituloPedido");
  mainLayout->addWidget(tituloLabel);
  m_layoutPlatos = new QVBoxLayout();
  mainLayout->addLayout(m_layoutPlatos);
  mainLayout->addStretch();
  setLayout(mainLayout);
}

void TarjetaPedido::agregarPlato(long long idInstancia, const QString& nombrePlato, const QString& estado) {
  TarjetaPlato* platoWidget = new TarjetaPlato(nombrePlato, estado, this);
  m_mapaPlatos[idInstancia] = platoWidget;
  m_layoutPlatos->addWidget(platoWidget);
}

void TarjetaPedido::actualizarEstadoPlato(long long idInstancia, const QString& nuevoEstado) {
  if (m_mapaPlatos.count(idInstancia)) {
    m_mapaPlatos.at(idInstancia)->actualizarEstado(nuevoEstado);
  }
}

void TarjetaPedido::quitarAcciones() {
  if (!m_layoutBotones) return;
  
  QLayoutItem* item;
  while ((item = m_layoutBotones->takeAt(0)) != nullptr) {
    if (item->widget()) delete item->widget();
    delete item;
  }

  delete m_layoutBotones;
  m_layoutBotones = nullptr;
}

void TarjetaPedido::agregarAcciones(bool esPrimerPedido) {
  if (!esPrimerPedido) return;
  quitarAcciones();

  m_layoutBotones = new QHBoxLayout();
  QPushButton* btnCancelar = new QPushButton("Cancelar", this);
  QPushButton* btnPreparar = new QPushButton("Preparar", this);
  btnCancelar->setObjectName("btnCancelar");
  btnPreparar->setObjectName("btnPreparar");

  connect(btnPreparar, &QPushButton::clicked, this, [this](){ emit prepararPedido(m_idPedido); });
  connect(btnCancelar, &QPushButton::clicked, this, [this](){ emit cancelarPedido(m_idPedido); });

  m_layoutBotones->addStretch();
  m_layoutBotones->addWidget(btnCancelar);
  m_layoutBotones->addWidget(btnPreparar);

  static_cast<QVBoxLayout*>(layout())->addLayout(m_layoutBotones);
}

void TarjetaPedido::agregarAccionesTerminado() {
  quitarAcciones();

  m_layoutBotones = new QHBoxLayout();
  QPushButton* btnRechazar = new QPushButton("Rechazar", this);
  QPushButton* btnEnviar = new QPushButton("Enviar", this);

  btnRechazar->setObjectName("btnRechazar");
  btnEnviar->setObjectName("btnEnviar");

  connect(btnEnviar, &QPushButton::clicked, this, [this]() { emit enviarPedido(m_idPedido); });
  connect(btnRechazar, &QPushButton::clicked, this, [this]() { emit rechazarPedido(m_idPedido); });

  m_layoutBotones->addStretch();
  m_layoutBotones->addWidget(btnRechazar);
  m_layoutBotones->addWidget(btnEnviar);

  static_cast<QVBoxLayout*>(layout())->addLayout(m_layoutBotones);
}
