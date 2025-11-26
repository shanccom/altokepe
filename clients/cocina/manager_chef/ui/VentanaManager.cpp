#include "VentanaManager.h"
#include "ColumnaPedidos.h"
#include "TarjetaPedido.h"
#include "common/network/SerializadorJSON.h"

#include <QHBoxLayout>
#include <QDebug>
#include <QJsonObject>

VentanaManager::VentanaManager(QWidget* parent) : QWidget(parent) {
  setupUI();
}

void VentanaManager::setupUI() {
  setWindowTitle("Vista del Manager Chef");
  resize(1200, 800);

  m_columnaPendiente = new ColumnaPedidos("Pendiente", this);
  m_columnaEnProgreso = new ColumnaPedidos("En Progreso", this);
  m_columnaTerminado = new ColumnaPedidos("Terminado", this);

  m_columnaPendiente->setObjectName("columnaPendiente");
  m_columnaEnProgreso->setObjectName("columnaEnProgreso");
  m_columnaTerminado->setObjectName("columnaTerminado");

  QHBoxLayout* mainLayout = new QHBoxLayout(this);
  mainLayout->addWidget(m_columnaPendiente);
  mainLayout->addWidget(m_columnaEnProgreso);
  mainLayout->addWidget(m_columnaTerminado);
  setLayout(mainLayout);
}

TarjetaPedido* VentanaManager::crearTarjeta(
    const PedidoMesa& pedido,
    const std::unordered_map<int, PlatoDefinicion>& menu
) {
  QString titulo = QString("Pedido Mesa %1").arg(pedido.numero_mesa).arg(pedido.id_pedido);
  auto* tarjeta = new TarjetaPedido(pedido.id_pedido, titulo, this);

  for (const auto& platoInst : pedido.platos) {
    QString nombrePlato = menu.count(platoInst.id_plato_definicion)
      ? QString::fromStdString(menu.at(platoInst.id_plato_definicion).nombre)
      : "ID Desconocido";

    QString estado = SerializadorJSON::estadoPlatoToString(platoInst.estado);
    tarjeta->agregarPlato(platoInst.id_instancia, nombrePlato, estado);
  }

  return tarjeta;
}

void VentanaManager::cargarEstadoInicial(
    const std::vector<PedidoMesa>& pendientes,
    const std::vector<PedidoMesa>& enProgreso,
    const std::vector<PedidoMesa>& terminados,
    const std::unordered_map<int, PlatoDefinicion>& menu
) {
  qDebug() << "Cargando estado inicial de la UI...";

  m_columnaPendiente->limpiarPedidos();
  m_columnaEnProgreso->limpiarPedidos();
  m_columnaTerminado->limpiarPedidos();
  m_mapaTarjetas.clear();

  for (const auto& pedido : pendientes) {
    onPedidoNuevo(pedido, menu);
  }

  for (const auto& pedido : enProgreso) {
    auto* tarjeta = crearTarjeta(pedido, menu);
    m_columnaEnProgreso->agregarPedido(tarjeta);
    m_mapaTarjetas[pedido.id_pedido] = tarjeta;
  }

  for (const auto& pedido : terminados) {
    auto* tarjeta = crearTarjeta(pedido, menu);
    tarjeta->agregarAccionesTerminado();

    connect(tarjeta, &TarjetaPedido::enviarPedido,
            this, &VentanaManager::enviarPedidoSolicitado);

    connect(tarjeta, &TarjetaPedido::rechazarPedido,
            this, &VentanaManager::rechazarPedidoSolicitado);

    m_columnaTerminado->agregarPedido(tarjeta);
    m_mapaTarjetas[pedido.id_pedido] = tarjeta;
  }
}

void VentanaManager::onPedidoNuevo(
    const PedidoMesa& pedido,
    const std::unordered_map<int, PlatoDefinicion>& menu
) {
  auto* tarjeta = crearTarjeta(pedido, menu);
  m_columnaPendiente->agregarPedido(tarjeta);

  m_mapaTarjetas[pedido.id_pedido] = tarjeta;

  actualizarBotonesColumnaPendiente();
}

void VentanaManager::onPedidoAMover(long long idPedido, const QString& columnaDestino) {
  if (!m_mapaTarjetas.count(idPedido)) return;

  TarjetaPedido* tarjeta = m_mapaTarjetas[idPedido];

  // Quitar de su columna actual
  tarjeta->setParent(nullptr);
  tarjeta->quitarAcciones();

  if (columnaDestino == "progreso") {
    m_columnaEnProgreso->agregarPedido(tarjeta);
  }
  else if (columnaDestino == "terminado") {
    m_columnaTerminado->agregarPedido(tarjeta);

    tarjeta->agregarAccionesTerminado();

    connect(tarjeta, &TarjetaPedido::enviarPedido,
            this, &VentanaManager::enviarPedidoSolicitado);

    connect(tarjeta, &TarjetaPedido::rechazarPedido,
            this, &VentanaManager::rechazarPedidoSolicitado);
  }

  actualizarBotonesColumnaPendiente();
}

void VentanaManager::onPedidoAEliminar(long long idPedido) {
  if (!m_mapaTarjetas.count(idPedido)) return;

  TarjetaPedido* tarjeta = m_mapaTarjetas[idPedido];

  tarjeta->setParent(nullptr);
  delete tarjeta;

  m_mapaTarjetas.erase(idPedido);

  actualizarBotonesColumnaPendiente();
}

void VentanaManager::onActualizarEstadoPlato(
    long long idPedido,
    long long idInstancia,
    const QString& nuevoEstado
) {
  if (m_mapaTarjetas.count(idPedido)) {
    m_mapaTarjetas[idPedido]->actualizarEstadoPlato(idInstancia, nuevoEstado);
  }
}

void VentanaManager::actualizarBotonesColumnaPendiente() {
  TarjetaPedido* primerPedido = m_columnaPendiente->getPrimerPedido();
  if (!primerPedido) return;

  primerPedido->agregarAcciones(true);

  // Evitar duplicar conexiones
  disconnect(primerPedido, &TarjetaPedido::prepararPedido,
             this, &VentanaManager::prepararPedidoSolicitado);

  disconnect(primerPedido, &TarjetaPedido::cancelarPedido,
             this, &VentanaManager::cancelarPedidoSolicitado);

  connect(primerPedido, &TarjetaPedido::prepararPedido,
          this, &VentanaManager::prepararPedidoSolicitado);

  connect(primerPedido, &TarjetaPedido::cancelarPedido,
          this, &VentanaManager::cancelarPedidoSolicitado);
}
