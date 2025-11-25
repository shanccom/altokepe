#include "VentanaManager.h"
#include "ColumnaPedidos.h"
#include "TarjetaPedido.h"
#include "common/network/SerializadorJSON.h"
#include <QHBoxLayout>
#include <QDebug>

VentanaManager::VentanaManager(QWidget* parent)
  : QWidget(parent) {
  setupUI();
}

void VentanaManager::setupUI() {
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

void VentanaManager::actualizarVistas(const std::vector<PedidoMesa>& pendientes,
                                    const std::vector<PedidoMesa>& enProgreso,
                                    const std::vector<PedidoMesa>& terminados,
                                    const std::unordered_map<int, PlatoDefinicion>& menu) {

  qDebug() << "Actualizando vistas con" << pendientes.size() << "pendientes," <<
    enProgreso.size() << "en progreso," << terminados.size() << "terminados.";

  m_columnaPendiente->limpiarPedidos();
  m_columnaEnProgreso->limpiarPedidos();
  m_columnaTerminado->limpiarPedidos();

  auto crearTarjeta = [&](const PedidoMesa& pedido) {
    QString titulo = QString("Pedido Mesa %1").arg(pedido.numero_mesa);
    auto* tarjeta = new TarjetaPedido(pedido.id_pedido, titulo, this);
    for (const auto& platoInst : pedido.platos) {
      QString nombrePlato = menu.count(platoInst.id_plato_definicion) ?
        QString::fromStdString(menu.at(platoInst.id_plato_definicion).nombre) : "ID Desconocido";
      QString estado = SerializadorJSON::estadoPlatoToString(platoInst.estado);
      tarjeta->agregarPlato(nombrePlato, estado);
    }
    return tarjeta;
  };

  // Columna Pendiente
  bool esPrimerPedido = true;
  for (const auto& pedido : pendientes) {
    auto* tarjeta = crearTarjeta(pedido);
    tarjeta->agregarAcciones(esPrimerPedido);
    if (esPrimerPedido) {
      connect(tarjeta, &TarjetaPedido::prepararPedido, this, &VentanaManager::prepararPedidoSolicitado);
      connect(tarjeta, &TarjetaPedido::cancelarPedido, this, &VentanaManager::cancelarPedidoSolicitado);
      esPrimerPedido = false;
    }
    m_columnaPendiente->agregarPedido(tarjeta);
  }

  // Columna En Progreso
  for (const auto& pedido : enProgreso) {
    auto* tarjeta = crearTarjeta(pedido);
    m_columnaEnProgreso->agregarPedido(tarjeta);
  }

  // Columna Terminado
  for (const auto& pedido : terminados) {
    auto* tarjeta = crearTarjeta(pedido);
    tarjeta->agregarAccionesTerminado();
    connect(tarjeta, &TarjetaPedido::enviarPedido, this, &VentanaManager::enviarPedidoSolicitado);
    connect(tarjeta, &TarjetaPedido::rechazarPedido, this, &VentanaManager::rechazarPedidoSolicitado);
    m_columnaTerminado->agregarPedido(tarjeta);
  }
}
