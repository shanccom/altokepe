#include "TarjetaPedido.h"
#include "TarjetaPlato.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

TarjetaPedido::TarjetaPedido(long long idPedido, const QString& titulo, QWidget* parent)
  : QFrame(parent), m_idPedido(idPedido) {
  setAttribute(Qt::WA_StyledBackground, true);

  this->setProperty("class", "TarjetaPedido");

  QVBoxLayout* mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setSpacing(0);

  // Cabecera
  QLabel* tituloLabel = new QLabel(titulo, this);
  tituloLabel->setProperty("class", "HeaderPedido");
  mainLayout->addWidget(tituloLabel);
  tituloLabel->setAlignment(Qt::AlignCenter);

  // Lista de platos
  QWidget* cuerpoPlatos = new QWidget(this);
  m_layoutPlatos = new QVBoxLayout(cuerpoPlatos);
  m_layoutPlatos->setContentsMargins(0, 5, 0, 5);
  m_layoutPlatos->setSpacing(0); // Separación manejada por el borde inferior de TarjetaPlato
  mainLayout->addWidget(cuerpoPlatos);

  // Área de Botones (Footer)
  m_contenedorBotones = new QWidget(this);
  QHBoxLayout* btnLayout = new QHBoxLayout(m_contenedorBotones);
  btnLayout->setContentsMargins(10, 5, 10, 10);
  btnLayout->setSpacing(10);
  
  // Crear TODOS los botones una vez
  m_btnCancelar = new QPushButton("Cancelar", this);
  m_btnCancelar->setObjectName("btnAccionNegativa");
  
  m_btnPreparar = new QPushButton("Preparar", this);
  m_btnPreparar->setObjectName("btnAccionPositiva");
  
  m_btnRechazar = new QPushButton("Rechazar", this);
  m_btnRechazar->setObjectName("btnAccionNegativa");

  m_btnEnviar = new QPushButton("Enviar", this);
  m_btnEnviar->setObjectName("btnAccionPositiva");

  // Conectar señales
  connect(m_btnPreparar, &QPushButton::clicked, [this](){ emit prepararPedido(m_idPedido); });
  connect(m_btnCancelar, &QPushButton::clicked, [this](){ emit cancelarPedido(m_idPedido); });
  connect(m_btnEnviar, &QPushButton::clicked, [this](){ emit enviarPedido(m_idPedido); });
  connect(m_btnRechazar, &QPushButton::clicked, [this](){ emit rechazarPedido(m_idPedido); });

  // Añadir al layout
  btnLayout->addWidget(m_btnCancelar);
  btnLayout->addStretch();
  btnLayout->addWidget(m_btnPreparar);
  btnLayout->addWidget(m_btnRechazar);
  btnLayout->addWidget(m_btnEnviar);
  
  mainLayout->addWidget(m_contenedorBotones);

  // Por defecto ocultamos el footer hasta que se asigne un modo
  m_contenedorBotones->setVisible(false);
}

void TarjetaPedido::agregarPlato(long long idInstancia, const QString& nombrePlato, const QString& estado) {
  QWidget* filaContainer = new QWidget(this);
  QHBoxLayout* filaLayout = new QHBoxLayout(filaContainer);
  filaLayout->setContentsMargins(0, 0, 10, 0);

  TarjetaPlato* platoWidget = new TarjetaPlato(nombrePlato, estado, this);
  m_mapaPlatos[idInstancia] = platoWidget;

  QPushButton* btnDevolver = new QPushButton("x", this);
  btnDevolver->setObjectName("btnDevolverPlato");
  btnDevolver->setFixedSize(24, 24);
  btnDevolver->setToolTip("Devolver/Rechazar este plato");

  connect(btnDevolver, &QPushButton::clicked, this, [this, idInstancia]() {
    emit rechazarPlato(m_idPedido, idInstancia);
  });

  filaLayout->addWidget(platoWidget, 1);
  filaLayout->addWidget(btnDevolver, 0);

  m_layoutPlatos->addWidget(filaContainer);
}

void TarjetaPedido::actualizarEstadoPlato(long long idInstancia, const QString& nuevoEstado) {
  if (m_mapaPlatos.count(idInstancia)) {
    m_mapaPlatos.at(idInstancia)->actualizarEstado(nuevoEstado);
  }
}

void TarjetaPedido::setConfiguracionPendiente(bool esElPrimero) {
  m_contenedorBotones->setVisible(true);
  m_btnCancelar->setVisible(true);

  // El botón Preparar SOLO está visible si es el primero en la cola (Lógica FIFO),
  m_btnPreparar->setVisible(esElPrimero);

  m_btnRechazar->setVisible(false);
  m_btnEnviar->setVisible(false);
}

void TarjetaPedido::setModoProgreso() {
  // En progreso no hay botones globales de pedido, o quizás solo "Cancelar"
  m_contenedorBotones->setVisible(false); 
}

void TarjetaPedido::setModoTerminado() {
  m_contenedorBotones->setVisible(true);

  m_btnCancelar->setVisible(false);
  m_btnPreparar->setVisible(false);

  m_btnRechazar->setVisible(true);
  m_btnEnviar->setVisible(true);
}


