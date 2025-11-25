#ifndef VENTANAMANAGER_H
#define VENTANAMANAGER_H

#include <QWidget>
#include <vector>
#include <unordered_map>
#include "common/models/PedidoMesa.h"
#include "common/models/PlatoDefinicion.h"

class ColumnaPedidos;

class VentanaManager : public QWidget {
  Q_OBJECT
public:
  explicit VentanaManager(QWidget* parent = nullptr);

public slots:
  void actualizarVistas(const std::vector<PedidoMesa>& pendientes,
                      const std::vector<PedidoMesa>& enProgreso,
                      const std::vector<PedidoMesa>& terminados,
                      const std::unordered_map<int, PlatoDefinicion>& menu);

signals:
  void prepararPedidoSolicitado(long long idPedido);
  void cancelarPedidoSolicitado(long long idPedido);
  void enviarPedidoSolicitado(long long idPedido);
  void rechazarPedidoSolicitado(long long idPedido);

private:
  void setupUI();

  ColumnaPedidos* m_columnaPendiente;
  ColumnaPedidos* m_columnaEnProgreso;
  ColumnaPedidos* m_columnaTerminado;
};

#endif
