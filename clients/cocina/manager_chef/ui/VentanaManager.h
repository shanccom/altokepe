#ifndef VENTANAMANAGER_H
#define VENTANAMANAGER_H

#include <QWidget>
#include <vector>
#include <unordered_map>

#include "common/models/PedidoMesa.h"
#include "common/models/PlatoDefinicion.h"

class ColumnaPedidos;
class TarjetaPedido;

class VentanaManager : public QWidget {
  Q_OBJECT
public:
  explicit VentanaManager(QWidget* parent = nullptr);

public slots:
  void cargarEstadoInicial(
    const std::vector<PedidoMesa>& pendientes,
    const std::vector<PedidoMesa>& enProgreso,
    const std::vector<PedidoMesa>& terminados,
    const std::unordered_map<int, PlatoDefinicion>& menu
  );

  void onPedidoNuevo(const PedidoMesa& pedido, const std::unordered_map<int, PlatoDefinicion>& menu);
  void onPedidoAMover(long long idPedido, const QString& columnaDestino);
  void onPedidoAEliminar(long long idPedido);
  void onActualizarEstadoPlato(long long idPedido, long long idInstancia, const QString& nuevoEstado);

signals:
  void prepararPedidoSolicitado(long long idPedido);
  void cancelarPedidoSolicitado(long long idPedido);
  void enviarPedidoSolicitado(long long idPedido);
  void rechazarPedidoSolicitado(long long idPedido);
  void rechazarPlatoSolicitado(long long idPedido, long long idInstancia);

private:
  void setupUI();
  TarjetaPedido* crearTarjeta(const PedidoMesa& pedido,
      const std::unordered_map<int, PlatoDefinicion>& menu);

  void actualizarBotonesColumnaPendiente();

  ColumnaPedidos* m_columnaPendiente;
  ColumnaPedidos* m_columnaEnProgreso;
  ColumnaPedidos* m_columnaTerminado;

  std::unordered_map<long long, TarjetaPedido*> m_mapaTarjetas;
};

#endif
