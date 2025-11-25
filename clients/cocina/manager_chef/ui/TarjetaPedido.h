#ifndef TARJETAPEDIDO_H
#define TARJETAPEDIDO_H

#include <QWidget>
#include <unordered_map>

class QVBoxLayout;
class QHBoxLayout;
class TarjetaPlato;

class TarjetaPedido : public QWidget {
  Q_OBJECT
public:
  explicit TarjetaPedido(long long idPedido, const QString& titulo, QWidget* parent = nullptr);
  
  void agregarPlato(long long idInstancia, const QString& nombrePlato, const QString& estado);
  void actualizarEstadoPlato(long long idInstancia, const QString& nuevoEstado);

  void agregarAcciones(bool esPrimerPedido);
  void agregarAccionesTerminado();
  void quitarAcciones();
  
signals:
  void prepararPedido(long long idPedido);
  void cancelarPedido(long long idPedido);
  void enviarPedido(long long idPedido);
  void rechazarPedido(long long idPedido);

private:
  long long m_idPedido;
  QVBoxLayout* m_layoutPlatos;
  QHBoxLayout* m_layoutBotones = nullptr;
  std::unordered_map<long long, TarjetaPlato*> m_mapaPlatos;
};

#endif
