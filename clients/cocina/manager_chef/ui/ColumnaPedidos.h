#ifndef COLUMNAPEDIDOS_H
#define COLUMNAPEDIDOS_H

#include <QWidget>

class QVBoxLayout;
class TarjetaPedido;

class ColumnaPedidos : public QWidget {
  Q_OBJECT
public:
  explicit ColumnaPedidos(const QString& titulo, QWidget* parent = nullptr);
  void agregarPedido(TarjetaPedido* pedido);
  void limpiarPedidos();
  bool estaVacia() const;
  TarjetaPedido* getPrimerPedido() const;

private:
  QVBoxLayout* m_layoutContenido;
};

#endif
