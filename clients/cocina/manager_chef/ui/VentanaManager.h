#ifndef VENTANAMANAGER_H
#define VENTANAMANAGER_H

#include <QWidget>

class ColumnaPedidos;

class VentanaManager : public QWidget {
  Q_OBJECT
public:
  explicit VentanaManager(QWidget* parent = nullptr);

public slots:
  void actualizarVistas(); // Slot para recibir datos del backend y redibujar

private slots:
  // Slots para manejar las acciones emitidas por las tarjetas
  void onPrepararPedido(long long idPedido);
  void onCancelarPedido(long long idPedido);
  void onEnviarPedido(long long idPedido);
  void onRechazarPedido(long long idPedido);

private:
  void setupUI();

  ColumnaPedidos* m_columnaPendiente;
  ColumnaPedidos* m_columnaEnProgreso;
  ColumnaPedidos* m_columnaTerminado;
};

#endif
