
#include "VentanaManager.h"
#include "ColumnaPedidos.h"
#include "TarjetaPedido.h"
#include <QHBoxLayout>

VentanaManager::VentanaManager(QWidget* parent)
  : QWidget(parent) {
  setupUI();
  // Simulación de datos iniciales
  actualizarVistas();
}

void VentanaManager::setupUI() {
  setWindowTitle("Vista del Manager Chef");
  resize(1200, 800);

  QHBoxLayout* mainLayout = new QHBoxLayout(this);

  m_columnaPendiente = new ColumnaPedidos("Pendiente", this);
  m_columnaEnProgreso = new ColumnaPedidos("En Progreso", this);
  m_columnaTerminado = new ColumnaPedidos("Terminado", this);

  // Asignar ObjectNames para aplicar estilos QSS específicos
  m_columnaPendiente->setObjectName("columnaPendiente");
  m_columnaEnProgreso->setObjectName("columnaEnProgreso");
  m_columnaTerminado->setObjectName("columnaTerminado");

  mainLayout->addWidget(m_columnaPendiente);
  mainLayout->addWidget(m_columnaEnProgreso);
  mainLayout->addWidget(m_columnaTerminado);

  setLayout(mainLayout);
}

void VentanaManager::actualizarVistas() {
  // Esta función será llamada cuando lleguen nuevos datos del servidor.
  // Borrará el contenido actual de las columnas y lo volverá a llenar
  // con la información actualizada.

  m_columnaPendiente->limpiarPedidos();
  m_columnaEnProgreso->limpiarPedidos();
  m_columnaTerminado->limpiarPedidos();

  // Reemplazar esto con datos reales del ClienteTCP

  {
    // Pedido 4 (primer elemento, interactivo)
    auto* pedido4 = new TarjetaPedido(4, "Pedido Mesa 4", this);
    pedido4->agregarPlato("Americano", "Pendiente");
    pedido4->agregarPlato("Rocoto relleno", "Pendiente");
    pedido4->agregarAcciones(true); // Es el primero
    connect(pedido4, &TarjetaPedido::prepararPedido, this, &VentanaManager::onPrepararPedido);
    connect(pedido4, &TarjetaPedido::cancelarPedido, this, &VentanaManager::onCancelarPedido);
    m_columnaPendiente->agregarPedido(pedido4);

    // Pedido 1 (no interactivo)
    auto* pedido1 = new TarjetaPedido(1, "Pedido Mesa 1", this);
    pedido1->agregarPlato("Tallarines verdes", "Pendiente");
    pedido1->agregarPlato("Helado vainilla", "Pendiente");
    pedido1->agregarAcciones(false); // No es el primero
    m_columnaPendiente->agregarPedido(pedido1);
  }

  // Columna En Progreso
  {
    auto* pedido6 = new TarjetaPedido(6, "Pedido Mesa 6", this);
    pedido6->agregarPlato("Arroz chaufa", "En cola");
    pedido6->agregarPlato("Torta 3 leches", "Cocinando");
    m_columnaEnProgreso->agregarPedido(pedido6);
  }

  // Columna Terminado
  {
    auto* pedido3 = new TarjetaPedido(3, "Pedido Mesa 3", this);
    pedido3->agregarPlato("Helado vainilla", "Terminado");
    pedido3->agregarPlato("Cake", "Terminado");
    pedido3->agregarAccionesTerminado();
    connect(pedido3, &TarjetaPedido::enviarPedido, this, &VentanaManager::onEnviarPedido);
    connect(pedido3, &TarjetaPedido::rechazarPedido, this, &VentanaManager::onRechazarPedido);
    m_columnaTerminado->agregarPedido(pedido3);
  }
}

// Implementación de los slots de acción
void VentanaManager::onPrepararPedido(long long idPedido) {
  // Llamar a ClienteTCP para enviar el comando "PREPARAR_PEDIDO" al servidor
  qDebug() << "Preparar pedido con ID:" << idPedido;
}

void VentanaManager::onCancelarPedido(long long idPedido) {
  // Llamar a ClienteTCP para enviar el comando "CANCELAR_PEDIDO" al servidor
  qDebug() << "Cancelar pedido con ID:" << idPedido;
}

void VentanaManager::onEnviarPedido(long long idPedido) {
  // Llamar a ClienteTCP para enviar comando de "ENVIAR_PEDIDO_TERMINADO"
  qDebug() << "Enviar pedido terminado con ID:" << idPedido;
}

void VentanaManager::onRechazarPedido(long long idPedido) {
  // Llamar a ClienteTCP para enviar comando de "RECHAZAR_PEDIDO_TERMINADO"
  qDebug() << "Rechazar pedido terminado con ID:" << idPedido;
}
