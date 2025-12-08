#include "VentanaManager.h"
#include "ColumnaPedidos.h"
#include "TarjetaPedido.h"
#include "common/adapter/AdaptadorSerializadorJSON.h"

#include <QHBoxLayout>
#include <QDebug>

VentanaManager::VentanaManager(QWidget* parent) : QWidget(parent) {
  setupUI();
}

void VentanaManager::setupUI() {
  setWindowTitle("Gestor de cocina: Manager Chef");
  resize(1200, 800);

  m_columnaPendiente = new ColumnaPedidos("Pendiente", this);
  m_columnaEnProgreso = new ColumnaPedidos("En Progreso", this);
  m_columnaTerminado = new ColumnaPedidos("Terminado", this);

  m_columnaPendiente->setObjectName("columnaPendiente");
  m_columnaEnProgreso->setObjectName("columnaEnProgreso");
  m_columnaTerminado->setObjectName("columnaTerminado");

  QHBoxLayout* mainLayout = new QHBoxLayout(this);
  mainLayout->setContentsMargins(15, 15, 15, 15);
  mainLayout->setSpacing(20);

  mainLayout->addWidget(m_columnaPendiente);
  mainLayout->addWidget(m_columnaEnProgreso);
  mainLayout->addWidget(m_columnaTerminado);

  setLayout(mainLayout);
}

TarjetaPedido* VentanaManager::crearTarjeta(
  const PedidoMesa& pedido,
  const std::unordered_map<int, PlatoDefinicion>& menu
) {
  QString titulo = QString("Mesa %1 - Pedido #%2").arg(pedido.numero_mesa).arg(pedido.id_pedido);
  auto* tarjeta = new TarjetaPedido(pedido.id_pedido, titulo, this);

  // Conectar señales de la tarjeta a la ventana
  connect(tarjeta, &TarjetaPedido::prepararPedido, this, &VentanaManager::prepararPedidoSolicitado);
  connect(tarjeta, &TarjetaPedido::cancelarPedido, this, &VentanaManager::cancelarPedidoSolicitado);
  connect(tarjeta, &TarjetaPedido::enviarPedido, this, &VentanaManager::enviarPedidoSolicitado);
  connect(tarjeta, &TarjetaPedido::rechazarPedido, this, &VentanaManager::rechazarPedidoSolicitado);
  connect(tarjeta, &TarjetaPedido::rechazarPlato, this, &VentanaManager::rechazarPlatoSolicitado);

  AdaptadorSerializadorJSON adaptador;
  for (const auto& platoInst : pedido.platos) {
    QString nombrePlato = menu.count(platoInst.id_plato_definicion)
      ? QString::fromStdString(menu.at(platoInst.id_plato_definicion).nombre)
      : "ID Desconocido";

    QString estado = adaptador.estadoPlatoToString(platoInst.estado);
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
  m_columnaPendiente->limpiarPedidos();
  m_columnaEnProgreso->limpiarPedidos();
  m_columnaTerminado->limpiarPedidos();
  m_mapaTarjetas.clear();

  // Pendientes
  for (const auto& pedido : pendientes) {
    auto* tarjeta = crearTarjeta(pedido, menu);
    m_columnaPendiente->agregarPedido(tarjeta);
    m_mapaTarjetas[pedido.id_pedido] = tarjeta;
    // Por defecto en modo progreso (oculto) hasta que actualicemos la lógica del primero
    tarjeta->setModoProgreso(); 
  }

  // En progreso
  for (const auto& pedido : enProgreso) {
    auto* tarjeta = crearTarjeta(pedido, menu);
    m_columnaEnProgreso->agregarPedido(tarjeta);
    tarjeta->setModoProgreso();
    m_mapaTarjetas[pedido.id_pedido] = tarjeta;
  }

  // Terminados
  for (const auto& pedido : terminados) {
    auto* tarjeta = crearTarjeta(pedido, menu);
    m_columnaTerminado->agregarPedido(tarjeta);
    tarjeta->setModoTerminado();
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

  // Desvincular visualmente antes de mover
  tarjeta->hide();
  tarjeta->setParent(nullptr);

  if (columnaDestino == "progreso") {
    m_columnaEnProgreso->agregarPedido(tarjeta);
    tarjeta->setModoProgreso();
  } else if (columnaDestino == "terminado") {
    m_columnaTerminado->agregarPedido(tarjeta);
    tarjeta->setModoTerminado();
  }

  tarjeta->show();

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
  QList<TarjetaPedido*> pedidos = m_columnaPendiente->obtenerTodosLosPedidos();

  for (int i = 0; i < pedidos.size(); ++i) {
    TarjetaPedido* tarjeta = pedidos[i];
    tarjeta->setConfiguracionPendiente(i == 0);
  }
}
