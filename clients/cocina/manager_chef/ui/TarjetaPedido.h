#ifndef TARJETAPEDIDO_H
#define TARJETAPEDIDO_H

#include <QFrame>
#include <unordered_map>

class QVBoxLayout;
class QHBoxLayout;
class QPushButton;
class TarjetaPlato;

class TarjetaPedido : public QFrame {
  Q_OBJECT
public:
  explicit TarjetaPedido(long long idPedido, const QString& titulo, QWidget* parent = nullptr);
  
  void agregarPlato(long long idInstancia, const QString& nombrePlato, const QString& estado);
  void actualizarEstadoPlato(long long idInstancia, const QString& nuevoEstado);

  void agregarAcciones(bool esPrimerPedido);
  void agregarAccionesTerminado();
  void quitarAcciones();

  // Métodos para cambiar modo visual en lugar de crear/destruir botones
  void setModoPendiente();
  void setModoTerminado();
  void setModoProgreso();
  
signals:
  void prepararPedido(long long idPedido);
  void cancelarPedido(long long idPedido);
  void enviarPedido(long long idPedido);
  void rechazarPedido(long long idPedido);
  void rechazarPlato(long long idPedido, long long idInstancia);

private:
  QVBoxLayout* m_layoutPlatos;
  QWidget* m_contenedorBotones; // Footer

  QPushButton* m_btnPreparar;
  QPushButton* m_btnCancelar;
  QPushButton* m_btnEnviar;
  QPushButton* m_btnRechazar;

  long long m_idPedido;
  std::unordered_map<long long, TarjetaPlato*> m_mapaPlatos;
};

#endif
